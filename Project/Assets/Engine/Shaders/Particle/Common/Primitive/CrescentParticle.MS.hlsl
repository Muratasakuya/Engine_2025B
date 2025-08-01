//============================================================================
//	include
//============================================================================

#include "../ParticleOutput.hlsli"
#include "../ParticleCommonSturctures.hlsli"

#include "../../../Math/Math.hlsli"

//============================================================================
// Constant
//============================================================================

#ifndef CRESCENT_MAX_DIVIDE
#define CRESCENT_MAX_DIVIDE 30
#endif

//============================================================================
//	CBuffer
//============================================================================

ConstantBuffer<PerView> gPerView : register(b0);

//============================================================================
//	StructuredBuffer
//============================================================================

struct Crescent {
	
	float outerRadius;
	float innerRadius;
	float startAngle;
	float endAngle;
	float lattice;
	float thickness;
	float2 pivot;
	uint divide;
	uint uvMode;
};

StructuredBuffer<Crescent> gCrescents : register(t0);
StructuredBuffer<Transform> gTransform : register(t1);

//============================================================================
//	Functions
//============================================================================

float2 ArcPoint(const Crescent crescent, float t, bool outer) {
	
	float angRad = lerp(crescent.startAngle, crescent.endAngle, t);
	float thicknessR = crescent.outerRadius - crescent.innerRadius;
	float innerDynamic = crescent.outerRadius - thicknessR * sin(t * PI);

	float latticeOff = (outer ? 1.0f : -1.0f) * crescent.lattice * 0.5f * sin(t * PI);
	float r = (outer ? crescent.outerRadius : innerDynamic) + latticeOff;

	return float2(cos(angRad), sin(angRad)) * r;
}

//============================================================================
//	Main
//============================================================================
[numthreads(CRESCENT_MAX_DIVIDE * 4, 1, 1)]
[outputtopology("triangle")]
void main(uint groupThreadId : SV_GroupThreadID, uint groupId : SV_GroupID,
out vertices MSOutput verts[(CRESCENT_MAX_DIVIDE + 1) * 4], out indices uint3 polys[(CRESCENT_MAX_DIVIDE * 8) + 4]) {
	
	// dispatchMeshでの1次元グループID
	uint instanceIndex = groupId;
	// バッファアクセス
	Crescent crescent = gCrescents[instanceIndex];
	Transform transform = gTransform[instanceIndex];
	
	// 頂点数、出力三角形数
	uint divide = min(crescent.divide, (uint) CRESCENT_MAX_DIVIDE);
	uint vertCount = (divide + 1) * 4;
	uint triCount = divide * 8 + 4;
	SetMeshOutputCounts(vertCount, triCount);

	if (groupThreadId < vertCount) {
		
		uint segIndex = groupThreadId >> 2;
		uint localIdx = groupThreadId & 3;
		bool outer = (localIdx == 0 || localIdx == 2);
		bool frontSide = (localIdx <= 1);

		float t = segIndex / (float) divide;
		float2 pos2 = ArcPoint(crescent, t, outer);

		// ピボットでオフセットを設定
		float2 halfExtent = float2(crescent.outerRadius, crescent.outerRadius);
		float2 pivotOff = lerp(-halfExtent, halfExtent, crescent.pivot);
		pos2 -= pivotOff;

		// 幅設定
		float halfT = 0.5f * crescent.thickness;
		float z = frontSide ? halfT : -halfT;

		// world行列を作成
		float4x4 worldMatrix = MakeWorldMatrix(transform, gPerView.billboardMatrix, gPerView.cameraPos);
		worldMatrix[3].xyz = transform.translation;
		
		// 親の設定
		worldMatrix = SetParent(transform, worldMatrix);

		// 行列計算
		float4x4 wvp = mul(worldMatrix, gPerView.viewProjection);
		float4 pos = mul(float4(pos2, z, 1.0f), wvp);

		// 頂点情報を設定
		MSOutput vertex;
		vertex.position = pos;
		vertex.instanceID = instanceIndex;

		// UV設定
		// 前面
		if (frontSide) {
			
			vertex.texcoord = (crescent.uvMode == 0) ? float2(t, outer ? 0.0f : 1.0f) : float2(outer ? 0.0f : 1.0f, t);
		}
		// 背面
		else {
			
			vertex.texcoord = (crescent.uvMode == 0) ? float2(t, outer ? 0.0f : 1.0f) : float2(outer ? 0.0f : 1.0f, t);
		}

		verts[groupThreadId] = vertex;
	}

	if (groupThreadId < divide) {
		
		uint i = groupThreadId;
		uint index0 = i * 4;
		uint index1 = (i + 1) * 4;

		// 各頂点index
		uint OF0 = index0 + 0; // Outer Front
		uint IF0 = index0 + 1; // Inner Front
		uint OB0 = index0 + 2; // Outer Back
		uint IB0 = index0 + 3; // Inner Back

		uint OF1 = index1 + 0;
		uint IF1 = index1 + 1;
		uint OB1 = index1 + 2;
		uint IB1 = index1 + 3;

		uint baseFront = 0;
		uint baseBack = baseFront + divide * 2;
		uint baseOuter = baseBack + divide * 2;
		uint baseInner = baseOuter + divide * 2;
		uint baseCap = baseInner + divide * 2;

		//  +Z
		uint fIndex = baseFront + i * 2;
		polys[fIndex + 0] = uint3(OF0, IF0, OF1);
		polys[fIndex + 1] = uint3(OF1, IF0, IF1);

		// -Z
		uint bIndex = baseBack + i * 2;
		polys[bIndex + 0] = uint3(OB1, IB0, OB0);
		polys[bIndex + 1] = uint3(OB1, IB1, IB0);

		// 外側の三角形
		uint oIndex = baseOuter + i * 2;
		polys[oIndex + 0] = uint3(OF0, OB0, OF1);
		polys[oIndex + 1] = uint3(OF1, OB0, OB1);

		// 内側の三角形
		uint inIndex = baseInner + i * 2;
		polys[inIndex + 0] = uint3(IF1, IB0, IF0);
		polys[inIndex + 1] = uint3(IF1, IB1, IB0);
	}

	// 始点と終点の三角形生成
	if (groupThreadId == 0) {

		uint polyIndex = divide * 4;

		uint OF0 = 0;
		uint IF0 = 1;
		uint OB0 = 2;
		uint IB0 = 3;

		uint OFN = polyIndex + 0;
		uint IFN = polyIndex + 1;
		uint OBN = polyIndex + 2;
		uint IBN = polyIndex + 3;

		uint capBase = divide * 8;

		// startAngle
		polys[capBase + 0] = uint3(OF0, OB0, IF0);
		polys[capBase + 1] = uint3(IF0, OB0, IB0);

		// endAngle
		polys[capBase + 2] = uint3(OFN, IFN, OBN);
		polys[capBase + 3] = uint3(IFN, IBN, OBN);
	}
}