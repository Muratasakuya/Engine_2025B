#include "MeshRegistry.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Core/Graphics/Mesh/MeshletBuilder.h>
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	MeshRegistry classMethods
//============================================================================

void MeshRegistry::Init(ID3D12Device* device, Asset* asset) {

	device_ = nullptr;
	device_ = device;

	asset_ = nullptr;
	asset_ = asset;
}

void MeshRegistry::RegisterMesh(const std::string& modelName) {

	// 作成済みの場合生成しない
	if (Algorithm::Find(meshes_, modelName)) {
		return;
	}

	// 頂点、meshlet生成
	const ResourceMesh resourceMesh = CreateMeshlet(modelName);

	// meshの生成
	meshes_[modelName] = std::make_unique<Mesh>();
	meshes_[modelName]->Init(device_, resourceMesh);
}

ResourceMesh MeshRegistry::CreateMeshlet(const std::string& modelName) {

	MeshletBuilder meshletBuilder{};

	Assimp::Importer importer;
	ModelData modelData = asset_->GetModelData(modelName);
	const aiScene* scene = importer.ReadFile(modelData.fullPath,
		aiProcess_FlipWindingOrder |
		aiProcess_FlipUVs |
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ImproveCacheLocality |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_SortByPType);

	// 頂点、meshlet生成
	ResourceMesh resourceMesh = meshletBuilder.ParseMesh(scene);

	return resourceMesh;
}