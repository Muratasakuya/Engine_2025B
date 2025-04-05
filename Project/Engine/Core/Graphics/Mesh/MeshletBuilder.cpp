#include "MeshletBuilder.h"

//============================================================================
//	MeshletBuilder classMethods
//============================================================================

ResourceMesh MeshletBuilder::ParseMesh(const aiMesh* sourceMesh) {

	// 出力
	ResourceMesh destinationMesh{};

	// マテリアル番号を設定
	destinationMesh.materialId = sourceMesh->mMaterialIndex;

	// 頂点情報の設定
	SetVertex(destinationMesh, sourceMesh);

	// 最適化処理
	Optimize(destinationMesh);

	// meshlet生成処理
	CreateMeshlet(destinationMesh);

	return destinationMesh;
}

void MeshletBuilder::SetVertex(ResourceMesh& destinationMesh, const aiMesh* sourceMesh) {

	// 頂点データ分メモリを確保する
	destinationMesh.vertices.resize(sourceMesh->mNumVertices);

	aiVector3D zeroVector = aiVector3D(0.0f, 0.0f, 0.0f);
	for (uint32_t i = 0; i < sourceMesh->mNumVertices; ++i) {

		aiVector3D* pos = &(sourceMesh->mVertices[i]);
		aiVector3D* normal = &(sourceMesh->mNormals[i]);
		aiVector3D* texcoord = &(sourceMesh->mTextureCoords[0][i]);

		// 頂点情報を格納
		destinationMesh.vertices[i] = MeshVertex(

			Vector4(-pos->x, pos->y, pos->z, 1.0f),
			Vector2(texcoord->x, texcoord->y),
			Vector3(-normal->x, normal->y, normal->z));
	}

	// 頂点インデックスのメモリを確保する
	destinationMesh.indices.resize(sourceMesh->mNumFaces * 3);

	for (auto i = 0u; i < sourceMesh->mNumFaces; ++i) {

		const auto& face = sourceMesh->mFaces[i];

		destinationMesh.indices[i * 3 + 0] = face.mIndices[0];
		destinationMesh.indices[i * 3 + 1] = face.mIndices[1];
		destinationMesh.indices[i * 3 + 2] = face.mIndices[2];
	}
}

void MeshletBuilder::Optimize(ResourceMesh& destinationMesh) {

	std::vector<uint32_t> remap(destinationMesh.indices.size());

	// 重複データを削除するための再マッピング用インデックスを生成
	auto vertexCount = meshopt_generateVertexRemap(

		remap.data(),
		destinationMesh.indices.data(),
		destinationMesh.indices.size(),
		destinationMesh.vertices.data(),
		destinationMesh.vertices.size(),
		sizeof(MeshVertex));

	std::vector<MeshVertex> vertices(vertexCount);
	std::vector<uint32_t> indices(destinationMesh.indices.size());

	// 頂点インデックスを再マッピング
	meshopt_remapIndexBuffer(

		indices.data(),
		destinationMesh.indices.data(),
		destinationMesh.indices.size(),
		remap.data());

	// 頂点データを再マッピング
	meshopt_remapVertexBuffer(

		vertices.data(),
		destinationMesh.vertices.data(),
		destinationMesh.vertices.size(),
		sizeof(MeshVertex),
		remap.data());

	// 不要になったメモリを解放
	remap.clear();
	remap.shrink_to_fit();

	// 最適化したサイズにメモリ量を減らす
	destinationMesh.indices.resize(indices.size());
	destinationMesh.vertices.resize(vertices.size());

	// 頂点キャッシュ最適化
	meshopt_optimizeVertexCache(

		destinationMesh.indices.data(),
		indices.data(),
		indices.size(),
		vertexCount);

	// 不要になったメモリを解放
	indices.clear();
	indices.shrink_to_fit();

	// 頂点フェッチ最適化
	meshopt_optimizeVertexFetch(

		destinationMesh.vertices.data(),
		destinationMesh.indices.data(),
		destinationMesh.indices.size(),
		vertices.data(),
		vertices.size(),
		sizeof(MeshVertex));

	// 不要になったメモリを解放
	vertices.clear();
	vertices.shrink_to_fit();
}

void MeshletBuilder::CreateMeshlet(ResourceMesh& destinationMesh) {

	// 頂点、プリミティブ数の最大数
	const size_t kMaxVertices = 64;
	const size_t kMaxPrimitives = 124;

	// メッシュレット最大数の計算
	size_t maxMeshlets = meshopt_buildMeshletsBound(
		destinationMesh.indices.size(),
		kMaxVertices,
		kMaxPrimitives);

	// メッシュレットと補助バッファの確保
	std::vector<meshopt_Meshlet> meshlets(maxMeshlets);
	std::vector<unsigned int> meshletVertices(maxMeshlets * kMaxVertices);
	std::vector<unsigned char> meshletTriangles(maxMeshlets * kMaxPrimitives);

	// メッシュレット構築
	size_t meshletCount = meshopt_buildMeshlets(
		meshlets.data(),
		meshletVertices.data(),
		meshletTriangles.data(),
		destinationMesh.indices.data(),
		destinationMesh.indices.size(),
		reinterpret_cast<const float*>(destinationMesh.vertices.data()),
		destinationMesh.vertices.size(),
		sizeof(MeshVertex),
		kMaxVertices,
		kMaxPrimitives,
		0.0f);

	meshlets.resize(meshletCount);

	// メモリ予約
	destinationMesh.uniqueVertexIndices.reserve(meshletCount * kMaxVertices);
	destinationMesh.primitiveIndices.reserve(meshletCount * kMaxPrimitives);

	// メッシュレットごとにデータを登録
	for (size_t i = 0; i < meshletCount; ++i) {

		const meshopt_Meshlet& meshlet = meshlets[i];

		uint32_t vertexOffset = static_cast<uint32_t>(destinationMesh.uniqueVertexIndices.size());
		uint32_t primitiveOffset = static_cast<uint32_t>(destinationMesh.primitiveIndices.size());

		// 頂点インデックスの登録
		for (size_t j = 0; j < meshlet.vertex_count; ++j) {
			unsigned int index = meshletVertices[meshlet.vertex_offset + j];
			destinationMesh.uniqueVertexIndices.push_back(index);
		}

		// 三角形インデックスの登録、インデックス3つで1三角形
		for (size_t j = 0; j < meshlet.triangle_count; ++j) {

			size_t triBase = meshlet.triangle_offset + j * 3;

			ResourcePrimitiveIndex tris{};
			tris.index0 = meshletTriangles[triBase + 0];
			tris.index1 = meshletTriangles[triBase + 1];
			tris.index2 = meshletTriangles[triBase + 2];

			destinationMesh.primitiveIndices.push_back(tris);
		}

		// メッシュレット情報を登録
		ResourceMeshlet resourceMeshlet = {};
		resourceMeshlet.vertexCount = meshlet.vertex_count;
		resourceMeshlet.vertexOffset = vertexOffset;
		resourceMeshlet.primitiveCount = meshlet.triangle_count;
		resourceMeshlet.primitiveOffset = primitiveOffset;

		std::vector<Color> colors = {

			{1.0f, 1.0f, 1.0f, 1.0f }, // 白
			{ 0.5f, 0.5f, 1.0f, 1.0f }, // 水色
			{ 1.0f, 0.5f, 1.0f, 1.0f }, // ピンク
			{1.0f, 0.0f, 0.0f, 1.0f}, // 赤
			{0.0f, 1.0f, 0.0f, 1.0f}, // 緑
			{0.0f, 0.0f, 1.0f, 1.0f}, // 青
			{1.0f, 1.0f, 0.0f, 1.0f}, // 黄
			{0.0f, 1.0f, 1.0f, 1.0f}, // シアン
			{1.0f, 0.0f, 1.0f, 1.0f}, // マゼンタ
			{1.0f, 0.5f, 0.0f, 1.0f}, // オレンジ
		};

		// meshletの色を指定
		resourceMeshlet.color = colors[i % colors.size()];

		destinationMesh.meshlets.push_back(resourceMeshlet);
	}

	// サイズを実際の使用量に合わせて最適化
	destinationMesh.uniqueVertexIndices.shrink_to_fit();
	destinationMesh.primitiveIndices.shrink_to_fit();
	destinationMesh.meshlets.shrink_to_fit();
}