#include "Asset.h"

//============================================================================
//	Asset classMethods
//============================================================================

void Asset::Init(ID3D12Device* device, DxCommand* dxCommand, SRVDescriptor* srvDescriptor) {

	textureManager_ = std::make_unique<TextureManager>();
	textureManager_->Init(device, dxCommand, srvDescriptor);

	modelLoader_ = std::make_unique<ModelLoader>();
	modelLoader_->Init(textureManager_.get());

	animationManager_ = std::make_unique<AnimationManager>();
	animationManager_->Init(device, srvDescriptor, modelLoader_.get());
}

void Asset::LoadTexture(const std::string& textureName) {
	textureManager_->Load(textureName);
}

void Asset::LoadModel(const std::string& modelName) {
	modelLoader_->Load(modelName);
}

void Asset::LoadAnimation(const std::string& animationName, const std::string& modelName) {
	animationManager_->Load(animationName, modelName);
}

void Asset::MakeModel(const std::string& modelName,
	const std::vector<MeshVertex>& vertexData,
	const std::vector<uint32_t>& indexData) {
	modelLoader_->Make(modelName, vertexData, indexData);
}

void Asset::Export(const ResourceMesh& modelData, const std::string& filePath) {
	modelLoader_->Export(modelData, filePath);
}

const D3D12_GPU_DESCRIPTOR_HANDLE& Asset::GetGPUHandle(const std::string textureName) const {
	return textureManager_->GetGPUHandle(textureName);
}

uint32_t Asset::GetTextureGPUIndex(const std::string& textureName) const {
	return textureManager_->GetTextureGPUIndex(textureName);
}

const DirectX::TexMetadata& Asset::GetMetaData(const std::string textureName) const {
	return textureManager_->GetMetaData(textureName);
}

std::vector<std::string> Asset::GetTextureHierarchies() const {
	return textureManager_->GetTextureHierarchies();
}

const std::vector<std::string>& Asset::GetTextureKeys() const {
	return textureManager_->GetTextureKeys();
}

bool Asset::SearchTexture(const std::string& textureName) {
	return textureManager_->Search(textureName);
}

const ModelData& Asset::GetModelData(const std::string& modelName) const {
	return modelLoader_->GetModelData(modelName);
}

const std::vector<std::string>& Asset::GetModelKeys() const {
	return modelLoader_->GetModelKeys();
}

const AnimationData& Asset::GetAnimationData(const std::string& animationName) const {
	return animationManager_->GetAnimationData(animationName);
}

const Skeleton& Asset::GetSkeletonData(const std::string& animationName) const {
	return animationManager_->GetSkeletonData(animationName);
}

const SkinCluster& Asset::GetSkinClusterData(const std::string& animationName) const {
	return animationManager_->GetSkinClusterData(animationName);
}