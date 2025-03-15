#include "Asset.h"

//============================================================================
//	Asset classMethods
//============================================================================

void Asset::Init(ID3D12Device* device, DxCommand* dxCommand, SRVManager* srvManager) {

	textureManager_ = std::make_unique<TextureManager>();
	textureManager_->Init(device, dxCommand, srvManager);

	modelManager_ = std::make_unique<ModelManager>();
	modelManager_->Init(textureManager_.get());

	animationManager_ = std::make_unique<AnimationManager>();
	animationManager_->Init(device, srvManager, modelManager_.get());
}

void Asset::LoadTexture(const std::string& textureName) {
	textureManager_->Load(textureName);
}

void Asset::LoadModel(const std::string& modelName) {
	modelManager_->Load(modelName);
}

void Asset::LoadAnimation(const std::string& animationName, const std::string& modelName) {
	animationManager_->Load(animationName, modelName);
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

const std::vector<std::string>& Asset::GetTextureKeys() const {
	return textureManager_->GetTextureKeys();
}

const ModelData& Asset::GetModelData(const std::string& modelName) const {
	return modelManager_->GetModelData(modelName);
}

const std::vector<std::string>& Asset::GetModelKeys() const {
	return modelManager_->GetModelKeys();
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

void Asset::SkeletonUpdate(const std::string& animationName) {
	animationManager_->SkeletonUpdate(animationName);
}

void Asset::ApplyAnimation(const std::string& animationName, float animationTime) {
	animationManager_->ApplyAnimation(animationName, animationTime);
}

void Asset::SkinClusterUpdate(const std::string& animationName) {
	animationManager_->SkinClusterUpdate(animationName);
}

void Asset::BlendAnimation(const std::string& oldAnimName, float oldAnimTime,
	const std::string& nextAnimName, float nextAnimTime, float alpha) {
	animationManager_->BlendAnimation(oldAnimName, oldAnimTime, nextAnimName, nextAnimTime, alpha);
}