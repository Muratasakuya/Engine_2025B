//============================================================================
//	include
//============================================================================

#include "../ParticleOutput.hlsli"
#include "../ParticleCommonSturctures.hlsli"

#include "../../../Math/Math.hlsli"

//============================================================================
//	CBuffer
//============================================================================

ConstantBuffer<PerView> gPerView : register(b0);

//============================================================================
//	StructuredBuffer
//============================================================================

struct Plane {
	
	float2 size;
};

StructuredBuffer<Plane> gPlanes : register(t0);
StructuredBuffer<Transform> gTransform : register(t1);

//============================================================================
//	Main
//============================================================================
[numthreads(4, 1, 1)]
[outputtopology("triangle")]
void main(uint groupThreadId : SV_GroupThreadID, uint groupId : SV_GroupID,
out vertices MSOutput verts[4], out indices uint3 polys[2]) {
	
	// dispatchMeshでの1次元グループID
	uint instanceIndex = groupId;
	// バッファアクセス
	Plane plane = gPlanes[instanceIndex];
	Transform transform = gTransform[instanceIndex];
	
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
		float4x4 worldMatrix = MakeWorldMatrix(transform, gPerView.billboardMatrix, gPerView.cameraPos);
		worldMatrix[3].xyz = transform.translation;

		// 行列計算
		float4x4 wvp = mul(worldMatrix, gPerView.viewProjection);
		float4 pos = mul(float4(localPos, 1.0f), wvp);

		// 頂点情報を設定
		MSOutput vertex;
		vertex.position = pos;
		vertex.texcoord = uv;
		vertex.instanceID = instanceIndex;
		
		verts[groupThreadId] = vertex;
	}
}