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
#define CRESCENT_MAX_DIVIDE 32
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
	float2 pivot;
	uint divide;
	uint uvMode;
};

StructuredBuffer<Crescent> gCrescents : register(t0);
StructuredBuffer<Transform> gTransform : register(t1);

//============================================================================
//	Main
//============================================================================
[numthreads(CRESCENT_MAX_DIVIDE * 2, 1, 1)]
[outputtopology("triangle")]
void main(uint groupThreadId : SV_GroupThreadID, uint groupId : SV_GroupID,
out vertices MSOutput verts[(CRESCENT_MAX_DIVIDE + 1) * 2], out indices uint3 polys[CRESCENT_MAX_DIVIDE * 2]) {
	
	// dispatchMeshでの1次元グループID
	uint instanceIndex = groupId;
	// バッファアクセス
	Crescent crescent = gCrescents[instanceIndex];
	Transform transform = gTransform[instanceIndex];
	
	// 頂点数、出力三角形数
	uint vertCount = (crescent.divide + 1) * 2;
	uint triCount = crescent.divide * 2;
	SetMeshOutputCounts(vertCount, triCount);
	
	// インデックス生成処理
	if (groupThreadId < crescent.divide) {
		
		polys[groupThreadId] = uint3(groupThreadId * 2, groupThreadId * 2 + 1, groupThreadId * 2 + 2);
		polys[groupThreadId + crescent.divide] = uint3(groupThreadId * 2 + 2, groupThreadId * 2 + 1, groupThreadId * 2 + 3);
	}
	
	// 頂点生成
	if (groupThreadId < vertCount) {
		
		bool isOuter = (groupThreadId & 1) == 0;
		uint segIndex = groupThreadId >> 1;
		float t = segIndex / (float) crescent.divide;
		float angRad = lerp(crescent.startAngle, crescent.endAngle, t);

		// 端で厚みを0.0fにして尖らせる
		float thickness = crescent.outerRadius - crescent.innerRadius;
		float innerDynamic = crescent.outerRadius - thickness * sin(t * PI);
		
		// ラティス歪曲
		float latticeOff = (isOuter ? 1.0f : -1.0f) * crescent.lattice * 0.5f * sin(t * PI);
		float r = (isOuter ? crescent.outerRadius : innerDynamic) + latticeOff;
		
		float2 pos2 = float2(cos(angRad), sin(angRad)) * r;

		// ピボット補正
		float2 halfExtent = float2(crescent.outerRadius, crescent.outerRadius);
		float2 pivotOff = lerp(-halfExtent, halfExtent, crescent.pivot);
		pos2 -= pivotOff;

		// world行列を作成
		float4x4 worldMatrix = MakeWorldMatrix(transform, gPerView.billboardMatrix, gPerView.cameraPos);
		worldMatrix[3].xyz = transform.translation;
		
		// 親の設定
		worldMatrix = SetParent(transform, worldMatrix);

		// 行列計算
		float4x4 wvp = mul(worldMatrix, gPerView.viewProjection);
		float4 pos = mul(float4(pos2.x, pos2.y, 0.0f, 1.0f), wvp);
		
		// 頂点情報を設定
		MSOutput vertex;
		vertex.position = pos;
		vertex.texcoord = (crescent.uvMode == 0) ?
		float2(t, isOuter ? 0.0f : 1.0f) : float2(isOuter ? 0.0f : 1.0f, t);
		vertex.instanceID = instanceIndex;

		verts[groupThreadId] = vertex;
	}
}