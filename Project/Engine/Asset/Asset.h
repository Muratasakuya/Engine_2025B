#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/TextureManager.h>
#include <Engine/Asset/ModelLoader.h>
#include <Engine/Asset/AnimationManager.h>
#include <Engine/Scene/Methods/IScene.h>

// c++
#include <memory>
#include <cctype>

//============================================================================
//	enum class
//============================================================================

// 読み込みの仕方
enum class AssetLoadType {

	Synch, // 同期
	Async  // 非同期
};

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

	// 初期化
	void Init(ID3D12Device* device, DxCommand* dxCommand, SRVDescriptor* srvDescriptor);
	// ログ出力
	void ReportUsage(bool listAll = false) const;

	// シーンアセットファイルの読み込み
	void LoadSceneAsync(Scene scene, AssetLoadType loadType);

	void LoadTexture(const std::string& textureName);
	void LoadLutTexture(const std::string& textureName);
	void LoadModel(const std::string& modelName);
	void LoadAnimation(const std::string& animationName, const std::string& modelName);

	//--------- textures -----------------------------------------------------

	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandle(const std::string textureName) const;
	uint32_t GetTextureGPUIndex(const std::string& textureName) const;
	const DirectX::TexMetadata& GetMetaData(const std::string textureName) const;

	std::vector<std::string> GetTextureHierarchies() const;
	const std::vector<std::string>& GetTextureKeys() const;

	bool SearchTexture(const std::string& textureName);

	//---------- models ------------------------------------------------------

	const ModelData& GetModelData(const std::string& modelName) const;
	const std::vector<std::string>& GetModelKeys() const;

	bool SearchModel(const std::string& modelName);

	//--------- animation ----------------------------------------------------

	const AnimationData& GetAnimationData(const std::string& animationName) const;
	const Skeleton& GetSkeletonData(const std::string& animationName) const;
	const SkinCluster& GetSkinClusterData(const std::string& animationName) const;

	void MakeModel(const std::string& modelName,
		const std::vector<MeshVertex>& vertexData,
		const std::vector<uint32_t>& indexData);
	void Export(const std::vector<MeshVertex>& inputVertices,
		const std::vector<uint32_t>& inputIndices, const std::string& filePath);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// assetを管理する
	std::unique_ptr<TextureManager> textureManager_;
	std::unique_ptr<ModelLoader> modelLoader_;
	std::unique_ptr<AnimationManager> animationManager_;
};