//============================================================================
//	include
//============================================================================

#include "EffectMesh.hlsli"

//============================================================================
// Constant
//============================================================================

#ifndef CYL_MAX_DIVIDE
#define CYL_MAX_DIVIDE 32
#endif

#define CYL_FLAG_TOPCAP    0x1
#define CYL_FLAG_BOTTOMCAP 0x2

static const uint CYL_MAX_VERTS = CYL_MAX_DIVIDE * 4 + 2;
static const uint CYL_MAX_TRIS = CYL_MAX_DIVIDE * 4;
static const float PI = 3.141592f;

//============================================================================
//	CBuffer
//============================================================================

cbuffer CameraData : register(b1) {
	
	float4x4 viewProjection;
};

//============================================================================
//	StructuredBuffer
//============================================================================

struct Cylinder {
	
	float topRadius;
	float bottomRadius;
	float height;
	uint divide;
	uint flags;
	float4x4 world;
};
StructuredBuffer<Cylinder> gCylinders : register(t0);

//============================================================================
//	Main
//============================================================================
[numthreads(1, 1, 1)]
[outputtopology("triangle")]
void main(uint groupThreadId : SV_GroupThreadID, uint groupId : SV_GroupID,
out vertices MSOutput verts[CYL_MAX_VERTS], out indices uint3 polys[CYL_MAX_TRIS]) {
	
	// dispatchMeshでの1次元グループID
	uint instanceIndex = groupId;
	Cylinder instance = gCylinders[instanceIndex];
	
	// 下限、上限を超えないようにする
	uint divide = max(3, min(instance.divide, (uint) CYL_MAX_DIVIDE));
	
	// 実際の有効数を計算
	uint vertexCount = divide * 2;
	if (instance.flags & CYL_FLAG_TOPCAP) {
		
		vertexCount += (divide + 1);
	}
	if (instance.flags & CYL_FLAG_BOTTOMCAP) {
		
		vertexCount += (divide + 1);
	}
	
	// 三角形数
	uint primitiveCount = divide * 2;
	if (instance.flags & CYL_FLAG_TOPCAP) {
		
		primitiveCount += divide;
	}
	if (instance.flags & CYL_FLAG_BOTTOMCAP) {
		
		primitiveCount += divide;
	}
	SetMeshOutputCounts(vertexCount, primitiveCount);
	
	// 行列計算
	float4x4 wvp = mul(instance.world, viewProjection);
	 // 角度ステップ
	float angleStep = PI * 2.0f / (float) divide;
	
	// cylinderを生成
	[loop]
	for (uint i = 0; i < divide; ++i) {
		
		float a = angleStep * (float) i;
		float s = sin(a);
		float c = cos(a);

		float3 pTop = float3(s * instance.topRadius, instance.height, c * instance.topRadius);
		float3 pBot = float3(s * instance.bottomRadius, 0.0f, c * instance.bottomRadius);

		float u = (float) i / (float) divide;
		float2 uvTop = float2(u, 0.0f);
		float2 uvBot = float2(u, 1.0f);

		uint vTop = i;
		uint vBot = divide + i;

		// 頂点出力
		MSOutput vertex;
		vertex.instanceID = instanceIndex;

		vertex.position = mul(float4(pTop, 1), wvp);
		vertex.texcoord = uvTop;
		verts[vTop] = vertex;

		vertex.position = mul(float4(pBot, 1), wvp);
		vertex.texcoord = uvBot;
		verts[vBot] = vertex;
	}
	
	[loop]
	for (uint i = 0; i < divide; ++i) {
		
		uint iNext = (i + 1) % divide;
		uint t0 = i;
		uint t1 = iNext;
		uint b0 = divide + i;
		uint b1 = divide + iNext;

		uint tBase = i * 2;
		verts[tBase + 0] = uint3(t0, t1, b0);
		verts[tBase + 1] = uint3(b0, t1, b1);
	}

	uint triCursor = divide * 2;
	uint vertCursor = divide * 2;
	if (instance.flags & CYL_FLAG_TOPCAP) {
		
		uint vCenter = vertCursor;
		uint vRing0 = vertCursor + 1;
		vertCursor += (divide + 1);

		// center
		{
			MSOutput vertex;
			vertex.instanceID = instanceIndex;
			vertex.position = mul(float4(0.0f, instance.height, 0.0f, 1.0f), wvp);
			vertex.texcoord = float2(0.5f, 0.5f);
			verts[vCenter] = vertex;
		}
		
		[loop]
		for (uint i = 0; i <= divide; ++i) {
			
			float a = angleStep * (float) i;
			float s = sin(a);
			float c = cos(a);

			float3 p = float3(s * instance.topRadius, instance.height, c * instance.topRadius);
			float2 uv = float2(0.5f + s * 0.5f, 0.5f - c * 0.5f);

			MSOutput vertex;
			vertex.instanceID = instanceIndex;
			vertex.position = mul(float4(p, 1), wvp);
			vertex.texcoord = uv;
			verts[vRing0 + i] = vertex;
		}

		[loop]
		for (uint i = 0; i < divide; ++i) {
			
			verts[triCursor + i] = uint3(vCenter, vRing0 + i, vRing0 + i + 1);
		}
		triCursor += divide;
	}

	if (instance.flags & CYL_FLAG_BOTTOMCAP) {
		
		uint vCenter = vertCursor;
		uint vRing0 = vertCursor + 1;
		vertCursor += (divide + 1);

		// center
		{
			MSOutput vertex;
			vertex.instanceID = instanceIndex;
			vertex.position = mul(float4(0.0f, 0.0f, 0.0f, 1.0f), wvp);
			vertex.texcoord = float2(0.5f, 0.5f);
			verts[vCenter] = vertex;
		}

		[loop]
		for (uint i = 0; i <= divide; ++i) {
			
			float a = angleStep * (float) i;
			float s = sin(a);
			float c = cos(a);

			float3 p = float3(s * instance.bottomRadius, 0.0f, c * instance.bottomRadius);
			float2 uv = float2(0.5f + s * 0.5f, 0.5f + c * 0.5f);

			MSOutput vertex;
			vertex.instanceID = instanceIndex;
			vertex.position = mul(float4(p, 1), wvp);
			vertex.texcoord = uv;
			verts[vRing0 + i] = vertex;
		}

		[loop]
		for (uint i = 0; i < divide; ++i) {
			
			polys[triCursor + i] = uint3(vCenter, vRing0 + i + 1, vRing0 + i);
		}
		triCursor += divide;
	}
}