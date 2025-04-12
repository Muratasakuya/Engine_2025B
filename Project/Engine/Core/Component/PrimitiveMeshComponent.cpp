#include "PrimitiveMeshComponent.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/Asset.h>
#include <Engine/Core/Graphics/Mesh/MeshletBuilder.h>

//============================================================================
//	PrimitiveMeshComponent classMethods
//============================================================================

void PrimitiveMeshComponent::Init(ID3D12Device* device, Asset* asset,
	const std::string& modelName) {

	asset_ = nullptr;
	asset_ = asset;

	// 頂点、meshlet生成
	const ResourceMesh resourceMesh = CreateMeshlet(modelName);

	// buffer作成
	primitiveMesh_ = std::make_unique<PrimitiveMesh>();
	primitiveMesh_->Init(device, resourceMesh);
}

ResourceMesh PrimitiveMeshComponent::CreateMeshlet(const std::string& modelName) {

	MeshletBuilder meshletBuilder{};

	Assimp::Importer importer;
	ModelData modelData = asset_->GetModelData(modelName);
	const aiScene* scene = importer.ReadFile(modelData.fullPath, aiProcess_FlipWindingOrder | aiProcess_FlipUVs);

	// 頂点、meshlet生成
	ResourceMesh resourceMesh = meshletBuilder.ParseMesh(scene);

	return resourceMesh;
}
