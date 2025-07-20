//============================================================================
//	include
//============================================================================

#include "../MeshDepth.hlsli"

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
	
	// インスタンスのワールド行列を取得
	float4x4 world = gTransforms[instanceIndex].world;
	
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
		
		// 座標変換
		output.position = mul(mul(input.position, world), viewProjection);

		verts[groupThreadId] = output;
	}
}