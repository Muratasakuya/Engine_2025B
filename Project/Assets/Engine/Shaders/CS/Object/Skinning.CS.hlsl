
//============================================================================
//	StructuredBuffer
//============================================================================

struct Vertex {
	
	float4 position;
	float2 texcoord;
	float3 normal;
	float4 color;
	float3 tangent;
	float3 biNormal;
};

struct Well {
	
	float4x4 skeletonSpaceMatrix;
	float4x4 skeletonSpaceInverseTransposeMatrix;
};

struct VertexInfluence {
	
	float4 weight;
	int4 index;
};

struct SkinningInformation {
	
	uint numVertices;
};

StructuredBuffer<Well> gMatrixPalette : register(t0);
StructuredBuffer<Vertex> gInputVertices : register(t1);
StructuredBuffer<VertexInfluence> gInfluences : register(t2);
// Skinning計算後の頂点データ、これをMSに渡す
RWStructuredBuffer<Vertex> gOutputVertices : register(u0);
ConstantBuffer<SkinningInformation> gSkinningInformation : register(b0);

//============================================================================
//	Functions
//============================================================================

// 位置の変換
float4 CalPosition(Vertex input, VertexInfluence influence) {
	
	float4 position;
	
	position = mul(input.position, gMatrixPalette[influence.index.x].skeletonSpaceMatrix) * influence.weight.x;
	position += mul(input.position, gMatrixPalette[influence.index.y].skeletonSpaceMatrix) * influence.weight.y;
	position += mul(input.position, gMatrixPalette[influence.index.z].skeletonSpaceMatrix) * influence.weight.z;
	position += mul(input.position, gMatrixPalette[influence.index.w].skeletonSpaceMatrix) * influence.weight.w;
	position.w = 1.0f;
	
	return position;
}

// 法線の変換
float3 CalNormal(Vertex input, VertexInfluence influence) {
	
	float3 normal;
	
	normal = mul(input.normal, (float3x3) gMatrixPalette[influence.index.x].skeletonSpaceInverseTransposeMatrix) * influence.weight.x;
	normal += mul(input.normal, (float3x3) gMatrixPalette[influence.index.y].skeletonSpaceInverseTransposeMatrix) * influence.weight.y;
	normal += mul(input.normal, (float3x3) gMatrixPalette[influence.index.z].skeletonSpaceInverseTransposeMatrix) * influence.weight.z;
	normal += mul(input.normal, (float3x3) gMatrixPalette[influence.index.w].skeletonSpaceInverseTransposeMatrix) * influence.weight.w;
	// 正規化して戻す
	normal = normalize(normal);
	
	return normal;
}

// 方向ベクトルの変換
float3 TransformDirection(float3 v, VertexInfluence influence) {
	
	float3 outputVector;
	
	outputVector = mul(v, (float3x3) gMatrixPalette[influence.index.x].skeletonSpaceInverseTransposeMatrix) * influence.weight.x;
	outputVector += mul(v, (float3x3) gMatrixPalette[influence.index.y].skeletonSpaceInverseTransposeMatrix) * influence.weight.y;
	outputVector += mul(v, (float3x3) gMatrixPalette[influence.index.z].skeletonSpaceInverseTransposeMatrix) * influence.weight.z;
	outputVector += mul(v, (float3x3) gMatrixPalette[influence.index.w].skeletonSpaceInverseTransposeMatrix) * influence.weight.w;
	// 正規化して戻す
	outputVector = normalize(outputVector);

	return outputVector;
}

//============================================================================
//	Main
//============================================================================
[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	// 頂点インデックス
	uint vertexIndex = DTid.x;
	// 範囲外回避 (バッファオーバーラン)
	if (vertexIndex < gSkinningInformation.numVertices) {
		
		Vertex input = gInputVertices[vertexIndex];
		VertexInfluence influence = gInfluences[vertexIndex];
		
		// Skinning後の頂点を計算
		Vertex skinned;
		
		// 位置の変換
		skinned.position = CalPosition(input, influence);
		// 法線の変換
		skinned.normal = CalNormal(input, influence);
		// 方向ベクトルの変換
		skinned.tangent = TransformDirection(input.tangent, influence);
		skinned.biNormal = TransformDirection(input.biNormal, influence);
		skinned.tangent = normalize(skinned.tangent - skinned.normal * dot(skinned.tangent, skinned.normal));
		skinned.biNormal = normalize(cross(skinned.normal, skinned.tangent));
		
		// 下記の値はそのまま入れる
		skinned.texcoord = input.texcoord;
		skinned.color = input.color;
		
		// Skinning後の頂点データを格納
		gOutputVertices[vertexIndex] = skinned;
	}
	
}