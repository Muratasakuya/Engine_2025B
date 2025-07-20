//============================================================================
//	include
//============================================================================

#include "../PrimitiveEffect.hlsli"

//============================================================================
//	CBuffer
//============================================================================

cbuffer PerView : register(b0) {
	
	float4x4 viewProjection;
	float4x4 billboardMatrix;
};

//============================================================================
//	StructuredBuffer
//============================================================================

struct Plane {
	
	float2 size;
};

struct Particle {
	
	float3 translation;
	float3 scale;
	float lifeTime;
	float currentTime;
	float3 velocity;
};

StructuredBuffer<Plane> gPlanes : register(t0);
StructuredBuffer<Particle> gParticles : register(t1);

//============================================================================
//	Main
//============================================================================
[numthreads(4, 1, 1)]
[outputtopology("triangle")]
void main(uint groupThreadId : SV_GroupThreadID, uint groupId : SV_GroupID,
out vertices MSOutput verts[4], out indices uint3 polys[2]) {
	
	// dispatchMeshでの1次元グループID
	uint instanceIndex = groupId;
	Plane plane = gPlanes[instanceIndex];
	Particle particle = gParticles[instanceIndex];
	
	// 頂点数4,出力三角形数2
	SetMeshOutputCounts(4, 2);
	
	// 三角形インデックスは最初の2スレッドで書き込み
	if (groupThreadId < 2) {

		const uint3 index[2] = { uint3(0, 1, 2), uint3(2, 1, 3) };
		polys[groupThreadId] = index[groupThreadId];
	}
	
	// planeを生成
	if (groupThreadId < 4) {
		
		// 縦と横の半分のサイズ
		float halfWidth = plane.size.x * 0.5f;
		float halfHeight = plane.size.y * 0.5f;

		float3 localPos;
		float2 uv = float2(0.0f, 0.0f);
		switch (groupThreadId) {
			// 下左
			case 0:

				localPos = float3(-halfWidth, -halfHeight, 0.0f);
				uv = float2(0.0f, 1.0f);
				break;
			// 下右
			case 1:

				localPos = float3(halfWidth, -halfHeight, 0.0f);
				uv = float2(1.0f, 1.0f);
				break;
			// 上左
			case 2:

				localPos = float3(-halfWidth, halfHeight, 0.0f);
				uv = float2(0.0f, 0.0f);
				break;
			// 上右
			default:

				localPos = float3(halfWidth, halfHeight, 0.0f);
				uv = float2(1.0f, 0.0f);
				break;
		}
		
		// world行列を作成
		float4x4 worldMatrix = billboardMatrix;
		worldMatrix[0] *= particle.scale.x;
		worldMatrix[1] *= particle.scale.y;
		worldMatrix[2] *= particle.scale.z;
		worldMatrix[3].xyz = particle.translation;

		// 行列計算
		float4x4 wvp = mul(worldMatrix, viewProjection);
		float4 pos = mul(float4(localPos, 1.0f), wvp);

		// 頂点情報を設定
		MSOutput vertex;
		vertex.position = pos;
		vertex.texcoord = uv;
		vertex.instanceID = instanceIndex;
		
		vertex.vertexColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
		
		verts[groupThreadId] = vertex;
	}
}