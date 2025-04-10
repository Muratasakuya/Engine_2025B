#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/TextureManager.h>
#include <Engine/Asset/ModelLoader.h>
#include <Engine/Asset/AnimationManager.h>

// c++
#include <memory>

//============================================================================
//	Asset class
//============================================================================
class Asset {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	Asset() = default;
	~Asset() = default;

	void Init(ID3D12Device* device, DxCommand* dxCommand, SRVDescriptor* srvDescriptor);

	void LoadTexture(const std::string& textureName);
	void LoadModel(const std::string& modelName);
	void LoadAnimation(const std::string& animationName, const std::string& modelName);

	// texture
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle(const std::string textureName) const;
	uint32_t GetTextureGPUIndex(const std::string& textureName) const;

	const DirectX::TexMetadata& GetMetaData(const std::string textureName) const;

	const std::vector<std::string>& GetTextureKeys() const;

	// model
	const ModelData& GetModelData(const std::string& modelName) const;

	const std::vector<std::string>& GetModelKeys() const;
	// animation
	const AnimationData& GetAnimationData(const std::string& animationName) const;

	const Skeleton& GetSkeletonData(const std::string& animationName) const;

	const SkinCluster& GetSkinClusterData(const std::string& animationName) const;

	void SkeletonUpdate(const std::string& animationName);
	void ApplyAnimation(const std::string& animationName, float animationTime);
	void SkinClusterUpdate(const std::string& animationName);

	void BlendAnimation(
		const std::string& oldAnimName, float oldAnimTime,
		const std::string& nextAnimName, float nextAnimTime,
		float alpha);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::unique_ptr<TextureManager> textureManager_;

	std::unique_ptr<ModelLoader> modelLoader_;

	std::unique_ptr<AnimationManager> animationManager_;
};