//============================================================================
//	include
//============================================================================

#include "MeshStandard.hlsli"

//============================================================================
//	Input
//============================================================================

struct MSInput {
	
	float4 position;
	float2 texcoord;
	float3 normal;
	float4 color;
	float3 tangent;
	float3 biNormal;
};

//============================================================================
//	CBuffer
//============================================================================

cbuffer InstanceData : register(b0) {
	
	uint meshletCount;
	uint numVertices;
	int isSkinned;
};

cbuffer CameraData : register(b1) {
	
	float4x4 viewProjection;
};

cbuffer ShadowLight : register(b2) {
	
	float4x4 lightViewProjection;
};

//============================================================================
//	StructuredBuffer
//============================================================================

struct TransformationMatrix {
	
	float4x4 world;
	float4x4 worldInverseTranspose;
};

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
StructuredBuffer<TransformationMatrix> gTransforms : register(t4);

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
	
	// DispatchMesh での1次元グループID
	uint groupIdx = groupId.x;
	uint meshletIndex = groupIdx % meshletCount;
	uint instanceIndex = groupIdx / meshletCount;
	
	// 現在のグループに対応するmeshletを取得
	Meshlet meshlet = gMeshlets[meshletIndex];
	
	// インスタンスのワールド行列と逆転置行列を取得
	float4x4 world = gTransforms[instanceIndex].world;
	float3x3 worldInverseTranspose = (float3x3) gTransforms[instanceIndex].worldInverseTranspose;
	
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
		// skinnedMeshだったらindexをインスタンスインデックス * 頂点分足す
		if (isSkinned == 1) {
			
			index += instanceIndex * numVertices;
		}
		MSInput input = gVertices[index];
		MSOutput output = (MSOutput) 0;
		
		// instanceIdを取得
		output.instanceID = instanceIndex;
		
		// スクリーン座標に変換
		float4x4 wvp = mul(world, viewProjection);
		output.position = mul(input.position, wvp);
		
		// テクスチャ座標
		output.texcoord = input.texcoord;
		
		// 法線
		output.normal = mul(input.normal, worldInverseTranspose);
		
		// 接線
		output.tangent = normalize(mul(input.tangent, worldInverseTranspose));
		output.biNormal = normalize(mul(input.biNormal, worldInverseTranspose));
	
		float4 worldPos = mul(input.position, world);
		output.worldPosition = worldPos.xyz;
		
		// lightView空間に変換
		output.positionInLVP = mul(worldPos, lightViewProjection);

		verts[groupThreadId] = output;
	}
}