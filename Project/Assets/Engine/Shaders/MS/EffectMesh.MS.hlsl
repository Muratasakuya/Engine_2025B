//============================================================================
//	include
//============================================================================

#include "EffectMesh.hlsli"

//============================================================================
//	Input
//============================================================================

struct MSInput {
	
	float4 position;
	float2 texcoord;
	float3 normal;
};

//============================================================================
//	CBuffer
//============================================================================

cbuffer TransformationMatrix : register(b0) {
	
	float4x4 world;
	float4x4 worldInverseTranspose;
};

cbuffer CameraData : register(b1) {
	
	float4x4 viewProjection;
};

//============================================================================
//	StructuredBuffer
//============================================================================

struct Meshlet {
	
	uint vertexOffset;
	uint vertexCount;
	uint primitiveOffset;
	uint primitiveCount;
	float4 color;
};

StructuredBuffer<MSInput> gVertices : register(t0);
StructuredBuffer<uint> gIndices : register(t1);
StructuredBuffer<Meshlet> gMeshlets : register(t2);
StructuredBuffer<uint> gPrimitives : register(t3);

//============================================================================
//	Function
//============================================================================

// 10bit * 3
uint3 UnpackPrimitiveIndex(uint packedIndex) {
	
	 // 下位10bit、1つ目の頂点インデックス
	 // 中間10bit、2つ目の頂点インデックス
	 // 上位10bit、3つ目の頂点インデックス
	
	return uint3(
	packedIndex & 0x3FF,
	(packedIndex >> 10) & 0x3FF,
	(packedIndex >> 20) & 0x3FF);
}

//============================================================================
//	Main
//============================================================================
[numthreads(128, 1, 1)]
[outputtopology("triangle")]
void main(
uint groupThreadId : SV_GroupThreadID, // グループ内のスレッドID
uint groupId : SV_GroupID, // グループID
out vertices MSOutput verts[64], // 出力頂点
out indices uint3 polys[126] // 出力三角形インデックス
) {
	
	// 現在のグループに対応するmeshletを取得
	Meshlet meshlet = gMeshlets[groupId];
	
	// メッシュシェーダーの出力数、頂点数、プリミティブ数を設定
	SetMeshOutputCounts(meshlet.vertexCount, meshlet.primitiveCount);
	
	// 各スレッドがプリミティブインデックスをアンパックして出力配列に格納
	if (groupThreadId < meshlet.primitiveCount) {
		
		uint packedIndex = gPrimitives[meshlet.primitiveOffset + groupThreadId];
		polys[groupThreadId] = UnpackPrimitiveIndex(packedIndex);
	}
	
	// 各スレッドで頂点データを処理して出力
	if (groupThreadId < meshlet.vertexCount) {
		
		uint index = gIndices[meshlet.vertexOffset + groupThreadId];
		MSInput input = gVertices[index];
		MSOutput output = (MSOutput) 0;
		
		// スクリーン座標に変換
		float4x4 wvp = mul(world, viewProjection);
		output.position = mul(input.position, wvp);
		
		// テクスチャ座標
		output.texcoord = input.texcoord;
		
		// 法線
		output.normal = normalize(mul(input.normal, (float3x3) worldInverseTranspose));
		
		// meshletの色
		output.color = meshlet.color;

		verts[groupThreadId] = output;
	}
}