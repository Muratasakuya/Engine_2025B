#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Asset/AssetStructure.h>
#include <Engine/Asset/Async/AssetLoadWorker.h>

// assimp
#include <Externals/assimp/include/assimp/Importer.hpp>
#include <Externals/assimp/include/assimp/postprocess.h>
#include <Externals/assimp/include/assimp/scene.h>
// c++
#include <unordered_map>
#include <unordered_set>
// front
class SRVDescriptor;
class ModelLoader;

//============================================================================
//	AnimationManager class
//============================================================================
class AnimationManager {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	AnimationManager() = default;
	~AnimationManager() = default;

	void Init(ID3D12Device* device, SRVDescriptor* srvDescriptor, ModelLoader* modelLoader);

	void Load(const std::string& animationName, const std::string& modelName);

	// 非同期処理
	void RequestLoadAsync(const std::string& animationName, const std::string& modelName);
	void WaitAll();

	//--------- accessor -----------------------------------------------------

	const AnimationData& GetAnimationData(const std::string& animationName) const;
	const Skeleton& GetSkeletonData(const std::string& animationName) const;
	const SkinCluster& GetSkinClusterData(const std::string& animationName) const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	// 非同期処理データキー
	struct AnimationAsyncKey {

		std::string animName;
		std::string modelName;
	};

	//--------- variables ----------------------------------------------------

	ID3D12Device* device_;
	SRVDescriptor* srvDescriptor_;
	ModelLoader* modelLoader_;

	std::string baseDirectoryPath_;

	std::unordered_map<std::string, AnimationData> animations_;
	std::unordered_map<std::string, Skeleton> skeletons_;
	std::unordered_map<std::string, SkinCluster> skinClusters_;

	// 非同期処理
	AssetLoadWorker<AnimationAsyncKey> loadWorker_;
	mutable std::mutex animMutex_;

	//--------- functions ----------------------------------------------------

	Skeleton CreateSkeleton(const Node& rootNode);
	int32_t CreateJoint(const Node& node, const std::optional<int32_t> parent, std::vector<Joint>& joints);
	SkinCluster CreateSkinCluster(const std::string& modelName, const std::string& animationName);

	// 非同期処理
	void LoadAsync(AnimationAsyncKey key);
};