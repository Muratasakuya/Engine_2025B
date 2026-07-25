#include "AnimationManager.h"

using namespace SakuEngine;

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Core/Debug/SpdLogger.h>
#include <Engine/Core/Graphics/Descriptors/SRVDescriptor.h>
#include <Engine/Core/Graphics/DxLib/DxUtils.h>
#include <Engine/Asset/ModelLoader.h>
#include <Engine/Asset/Filesystem.h>
#include <Engine/Utility/Algorithm/Algorithm.h>

//============================================================================
//	AnimationManager classMethods
//============================================================================

/// <summary>
/// アニメーション管理に必要な外部リソースを保持し、読み込み要求を処理するワーカースレッドを開始する。
/// </summary>
/// <param name="device">GPUリソース作成に使用するD3D12デバイス。</param>
/// <param name="srvDescriptor">スキンクラスター用SRVを確保するディスクリプタ管理クラス。</param>
/// <param name="modelLoader">アニメーションと紐づくモデルデータの取得元。</param>
void AnimationManager::Init(ID3D12Device* device,
	SRVDescriptor* srvDescriptor, ModelLoader* modelLoader) {

	device_ = nullptr;
	device_ = device;

	srvDescriptor_ = nullptr;
	srvDescriptor_ = srvDescriptor;

	modelLoader_ = nullptr;
	modelLoader_ = modelLoader;

	baseDirectoryPath_ = "./Assets/Models/";

	// ワーカースレッド起動
	loadWorker_.Start([this](AnimationAsyncKey&& key) {
		this->LoadAsync(key); });
}

/// <summary>
/// アニメーション読み込みを要求し、同期的に利用したい呼び出し元のために登録完了まで待機する。
/// </summary>
/// <param name="animationName">読み込むアニメーションファイル名またはstem名。</param>
/// <param name="modelName">骨構造とスキンクラスターの基準にするモデル名。</param>
void AnimationManager::Load(const std::string& animationName, const std::string& modelName) {

	RequestLoadAsync(animationName, modelName);
	// モデルとアニメが来るまで待つ
	for (;;) {
		{
			std::scoped_lock lk(animMutex_);
			if (animations_.find(modelName) != animations_.end()) break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

/// <summary>
/// 重複要求を避けながら、アニメーションとモデル名の組を非同期キューへ登録する。
/// </summary>
/// <param name="animationName">読み込むアニメーションファイル名またはstem名。</param>
/// <param name="modelName">読み込み完了後に関連付けるモデル名。</param>
void AnimationManager::RequestLoadAsync(const std::string& animationName, const std::string& modelName) {

	auto& queue = loadWorker_.RefAsyncQueue();

	// 処理中のキューにあるなら処理させない
	if (queue.IsClearCondition([&](const AnimationAsyncKey& j) {
		return j.animName == animationName && j.modelName == modelName;
		})) {
		return;
	}
	// 重複チェック後にキューを追加
	queue.AddQueue(AnimationAsyncKey{ animationName, modelName });
	SpdLogger::Log("[Animation][Enqueue] anim:" + animationName + " model:" + modelName);
}

/// <summary>
/// 起動中の読み込みキューが空になるまで待機する。
/// </summary>
void AnimationManager::WaitAll() {

	for (;;) {
		if (loadWorker_.GetAsyncQueue().IsEmpty()) {

			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

/// <summary>
/// ワーカースレッド上でアニメーションを解析し、共有データへ登録する。
/// </summary>
/// <param name="key">アニメーション名と関連モデル名をまとめた読み込みキー。</param>
void AnimationManager::LoadAsync(const AnimationAsyncKey& key) {

	// 非同期読み込みではモデルとアニメーションの完了順が前後するため、モデル未登録なら少し待って再キューする。
	// ここで失敗扱いにすると、先にアニメーション要求が来た通常ケースでも再生データが欠落してしまう。
	if (!modelLoader_->Search(key.modelName)) {

		SpdLogger::Log("[Animation][WaitModel] anim:" + key.animName + " model:" + key.modelName);
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
		loadWorker_.RefAsyncQueue().AddQueue(AnimationAsyncKey{ key.animName, key.modelName });
		return;
	}

	std::filesystem::path filePath;
	// stem検索でアセット名と拡張子の揺れを吸収する。
	// 見つからない場合は再キューしても状態が変わらないため、ログだけ残して読み込みを打ち切る。
	if (!Filesystem::FindByStem(baseDirectoryPath_, key.animName, { ".gltf" }, filePath)) {

		SpdLogger::Log("[Animation][Missing] anim:" + key.animName);
		return;
	}

	// Assimpでファイルを開き、アニメーションクリップが存在しないファイルはデータ不備として扱う。
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath.string(), 0);
	if (!scene || scene->mNumAnimations == 0) {

		SpdLogger::Log("[Animation][NoClips] anim:" + key.animName);
		ASSERT(FALSE, "[Animation][NoClips] anim:" + key.animName);
		return;
	}

	// Assimpの各チャンネルをエンジン内のキー形式へ変換し、登録前はローカルに保持する。
	// 共有マップを長時間ロックしないよう、ファイル解析と変換は排他区間の外で完了させる。
	std::unordered_map<std::string, AnimationData> localAnimations{};
	for (uint32_t i = 0; i < scene->mNumAnimations; ++i) {

		aiAnimation* animAssimp = scene->mAnimations[i];
		AnimationData anim;

		// アニメーションの名前設定
		const std::string newName = (scene->mNumAnimations == 1) ?
			key.modelName : (key.modelName + "_" + animAssimp->mName.C_Str());

		anim.duration = static_cast<float>(animAssimp->mDuration / animAssimp->mTicksPerSecond);
		for (uint32_t c = 0; c < animAssimp->mNumChannels; ++c) {

			aiNodeAnim* nodeAnim = animAssimp->mChannels[c];
			NodeAnimation& node = anim.nodeAnimations[nodeAnim->mNodeName.C_Str()];
			// T
			for (uint32_t k = 0; k < nodeAnim->mNumPositionKeys; ++k) {

				aiVectorKey& kv = nodeAnim->mPositionKeys[k];
				KeyframeVector3 f; f.time = float(kv.mTime / animAssimp->mTicksPerSecond);
				f.value = { -kv.mValue.x, kv.mValue.y, kv.mValue.z };
				node.translate.keyframes.push_back(f);
			}
			// R
			for (uint32_t k = 0; k < nodeAnim->mNumRotationKeys; ++k) {

				aiQuatKey& kv = nodeAnim->mRotationKeys[k];
				KeyframeQuaternion f; f.time = float(kv.mTime / animAssimp->mTicksPerSecond);
				f.value = { kv.mValue.x, -kv.mValue.y, -kv.mValue.z, kv.mValue.w };
				node.rotate.keyframes.push_back(f);
			}
			// S
			for (uint32_t k = 0; k < nodeAnim->mNumScalingKeys; ++k) {

				aiVectorKey& kv = nodeAnim->mScalingKeys[k];
				KeyframeVector3 f; f.time = float(kv.mTime / animAssimp->mTicksPerSecond);
				f.value = { kv.mValue.x, kv.mValue.y, kv.mValue.z };
				node.scale.keyframes.push_back(f);
			}
		}
		localAnimations.emplace(newName, std::move(anim));
	}

	// 共有マップへの登録は排他区間にまとめ、描画/再生側から中途半端な状態が見えないようにする。
	SpdLogger::Log("[Animation][Loaded] anim=" + key.animName);
	{
		std::scoped_lock lk(animMutex_);
		for (auto& [name, animation] : localAnimations) {

			animations_[name] = std::move(animation);
		}

		// クラスター、骨データ作成
		if (!skeletons_.contains(key.modelName)) {

			skeletons_[key.modelName] = CreateSkeleton(modelLoader_->GetModelData(key.modelName).rootNode);
		}
		if (!skinClusters_.contains(key.modelName)) {

			skinClusters_[key.modelName] = CreateSkinCluster(key.modelName, key.modelName);
		}
	}
	SpdLogger::Log("[Animation][Registered] model:" + key.modelName + "animations:" + std::to_string(localAnimations.size()));
}

/// <summary>
/// モデルのルートノードからジョイント階層と名前検索用マップを構築する。
/// </summary>
/// <param name="rootNode">スケルトン生成の起点になるモデルノード。</param>
/// <returns>再生時に使用するスケルトンデータ。</returns>
Skeleton AnimationManager::CreateSkeleton(const Node& rootNode) {

	Skeleton skeleton;
	skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

	// 名前とIndexのマッピングを行う
	for (const auto& joint : skeleton.joints) {

		skeleton.jointMap.emplace(joint.name, joint.index);
	}

	return skeleton;
}

/// <summary>
/// ノード階層を再帰的にたどり、親子関係を保持したジョイント配列を作成する。
/// </summary>
/// <param name="node">作成対象のモデルノード。</param>
/// <param name="parent">親ジョイントのインデックス。ルートの場合は空。</param>
/// <param name="joints">生成したジョイントを追加する配列。</param>
/// <returns>作成したジョイントのインデックス。</returns>
int32_t AnimationManager::CreateJoint(const Node& node, const std::optional<int32_t> parent, std::vector<Joint>& joints) {

	Joint joint;
	joint.name = node.name;
	joint.isParentTransform = false;
	joint.localMatrix = node.localMatrix;
	joint.skeletonSpaceMatrix = Matrix4x4::MakeIdentity4x4();
	joint.transform = node.transform;
	// 現在登録されている数をIndexにする
	joint.index = static_cast<int32_t>(joints.size());
	joint.parent = parent;
	// SkeletonのJoint列に追加
	joints.push_back(joint);

	for (const auto& child : node.children) {

		// 子Jointを作成し、そのIndexを登録
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}

	return joint.index;
}

/// <summary>
/// モデルの頂点ウェイト情報とスケルトンを対応付け、GPUスキニング用のスキンクラスターを作成する。
/// </summary>
/// <param name="modelName">参照するモデルデータ名。</param>
/// <param name="animationName">参照するスケルトンデータ名。</param>
/// <returns>スキニングで使用するスキンクラスターデータ。</returns>
SkinCluster AnimationManager::CreateSkinCluster(const std::string& modelName, const std::string& animationName) {

	SkinCluster skinCluster;

	// size確保
	skinCluster.mappedPalette.resize(skeletons_[animationName].joints.size());

	// inverseBindPoseMatrixを格納する場所を作成して単位行列で埋める
	skinCluster.inverseBindPoseMatrices.resize(skeletons_[animationName].joints.size());
	std::generate(skinCluster.inverseBindPoseMatrices.begin(), skinCluster.inverseBindPoseMatrices.end(),
		[]() { return Matrix4x4::MakeIdentity4x4(); });

	// ModelDataを解析してInfluenceを埋める
	for (const auto& jointWeight : modelLoader_->GetModelData(modelName).skinClusterData) {

		// jointWeight.firstはjoint名なので、skeletonに対象となるjointが含まれているか判断
		auto it = skeletons_[animationName].jointMap.find(jointWeight.first);
		// 存在しないjoint名だったら次に進める
		if (it == skeletons_[animationName].jointMap.end()) {
			continue;
		}

		// (*it).secondにはjointのIndexが入っているので、該当のIndexのInverseBindPoseMatrixを代入
		skinCluster.inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
	}

	return skinCluster;
}

/// <summary>
/// 登録済みアニメーションデータを名前で取得する。
/// </summary>
/// <param name="animationName">取得するアニメーション名。</param>
/// <returns>登録済みアニメーションデータへの参照。</returns>
const AnimationData& AnimationManager::GetAnimationData(const std::string& animationName) const {

	std::scoped_lock lk(animMutex_);
	bool find = animations_.find(animationName) != animations_.end();
	if (!find) {

		LOG_WARN("not found animation", animationName);
		ASSERT(find, "not found animation" + animationName);
	}
	return animations_.at(animationName);
}

/// <summary>
/// 登録済みスケルトンデータを名前で取得する。
/// </summary>
/// <param name="animationName">取得するスケルトン名。</param>
/// <returns>登録済みスケルトンデータへの参照。</returns>
const Skeleton& AnimationManager::GetSkeletonData(const std::string& animationName) const {

	std::scoped_lock lk(animMutex_);
	bool find = skeletons_.find(animationName) != skeletons_.end();
	if (!find) {

		LOG_WARN("not found animation", animationName);
		ASSERT(find, "not found animation" + animationName);
	}
	return skeletons_.at(animationName);
}

/// <summary>
/// 登録済みスキンクラスターデータを名前で取得する。
/// </summary>
/// <param name="animationName">取得するスキンクラスター名。</param>
/// <returns>登録済みスキンクラスターデータへの参照。</returns>
const SkinCluster& AnimationManager::GetSkinClusterData(const std::string& animationName) const {

	std::scoped_lock lk(animMutex_);
	bool find = skinClusters_.find(animationName) != skinClusters_.end();
	if (!find) {

		LOG_WARN("not found animation", animationName);
		ASSERT(find, "not found animation" + animationName);
	}
	return skinClusters_.at(animationName);
}
