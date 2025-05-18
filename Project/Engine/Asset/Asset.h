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

	void MakeModel(const std::string& modelName,
		const std::vector<MeshVertex>& vertexData,
		const std::vector<uint32_t>& indexData);
	void Export(const ResourceMesh& modelData, const std::string& filePath);

	// texture
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle(const std::string textureName) const;
	uint32_t GetTextureGPUIndex(const std::string& textureName) const;

	const DirectX::TexMetadata& GetMetaData(const std::string textureName) const;

	std::vector<std::string> GetTextureHierarchies() const;
	const std::vector<std::string>& GetTextureKeys() const;

	bool SearchTexture(const std::string& textureName);

	// model
	const ModelData& GetModelData(const std::string& modelName) const;

	const std::vector<std::string>& GetModelKeys() const;
	// animation
	const AnimationData& GetAnimationData(const std::string& animationName) const;

	const Skeleton& GetSkeletonData(const std::string& animationName) const;

	const SkinCluster& GetSkinClusterData(const std::string& animationName) const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::unique_ptr<TextureManager> textureManager_;

	std::unique_ptr<ModelLoader> modelLoader_;

	std::unique_ptr<AnimationManager> animationManager_;
};