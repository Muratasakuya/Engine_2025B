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

static const uint CYL_MAX_VERTS = (CYL_MAX_DIVIDE + 1) * 2;
static const uint CYL_MAX_TRIS = CYL_MAX_DIVIDE * 2;
static const float PI = 3.141592653589793f;

//============================================================================
//	CBuffer
//============================================================================

cbuffer CameraData : register(b0) {
	
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
	uint divide = clamp(instance.divide, 3, (uint) CYL_MAX_DIVIDE);
	SetMeshOutputCounts((divide + 1) * 2, divide * 2);
	
	// 行列計算
	float4x4 wvp = mul(instance.world, viewProjection);
	// 角度ステップ
	float angleStep = PI * 2.0f / (float) divide;
	
	// cylinderを生成
	// 頂点
	[loop]
	for (uint i = 0; i <= divide; ++i) {

		float a = angleStep * (float) i;
		float s = sin(a);
		float c0 = cos(a);

		float3 pTop = float3(s * instance.topRadius, instance.height, c0 * instance.topRadius);
		float3 pBot = float3(s * instance.bottomRadius, 0.0f, c0 * instance.bottomRadius);

		float u = (float) i / (float) divide;

		MSOutput vertex;
		vertex.instanceID = instanceIndex;

		vertex.position = mul(float4(pTop, 1), wvp);
		vertex.texcoord = float2(-u, 0.0f);
		verts[i] = vertex;

		vertex.position = mul(float4(pBot, 1), wvp);
		vertex.texcoord = float2(-u, 1.0f);
		verts[divide + 1 + i] = vertex;
	}

	// インデックス
	[loop]
	for (uint i = 0; i < divide; ++i) {
		
		uint t0 = i;
		uint t1 = i + 1;
		uint b0 = (divide + 1) + i;
		uint b1 = (divide + 1) + i + 1;
		
		polys[i * 2 + 0] = uint3(t0, b0, t1);
		polys[i * 2 + 1] = uint3(b0, b1, t1);
	}
}