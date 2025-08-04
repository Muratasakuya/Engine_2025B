#include "AnimationManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Core/Debug/SpdLogger.h>
#include <Engine/Core/Graphics/Descriptors/SRVDescriptor.h>
#include <Engine/Core/Graphics/Lib/DxUtils.h>
#include <Engine/Asset/ModelLoader.h>
#include <Engine/Asset/Filesystem.h>
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	AnimationManager classMethods
//============================================================================

AnimationManager::~AnimationManager() {

	stop_ = true;
	jobCv_.notify_all();
	if (worker_.joinable()) worker_.join();
}

void AnimationManager::Init(ID3D12Device* device, SRVDescriptor* srvDescriptor, ModelLoader* modelLoader) {

	device_ = nullptr;
	device_ = device;

	srvDescriptor_ = nullptr;
	srvDescriptor_ = srvDescriptor;

	modelLoader_ = nullptr;
	modelLoader_ = modelLoader;

	baseDirectoryPath_ = "./Assets/Models/";

	stop_ = false;
	worker_ = std::thread([this] {
		while (!stop_) {
			AnimJob job;
			{
				std::unique_lock lk(jobMutex_);
				jobCv_.wait(lk, [&] { return stop_ || !jobs_.empty(); });
				if (stop_) break;
				job = jobs_.front();
				jobs_.pop_front();
			}

			// 依存：モデルがまだなら後回し（再投入）
			if (!modelLoader_->Search(job.model)) { // 非同期ロード完了待ちを避ける

				SpdLogger::Log("[Anim][WaitModel] anim=" + job.anim + " model=" + job.model);

				std::this_thread::sleep_for(std::chrono::milliseconds(2));
				std::scoped_lock lk(jobMutex_);
				jobs_.push_back(job); // 後ろに戻す
				continue;
			}

			// Assimp 読み込みの時間
			auto t0 = std::chrono::high_resolution_clock::now();
			// ... importer.ReadFile / 解析
			auto t1 = std::chrono::high_resolution_clock::now();

			SpdLogger::Log("[Anim][Loaded] anim=" + job.anim +
				" (" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count()) + "ms)");

			// 既存の Load と同じ処理をバックグラウンド実行
			// 1) ファイル探索
			std::filesystem::path filePath;
			bool found = false;
			for (const auto& entry : std::filesystem::recursive_directory_iterator(baseDirectoryPath_)) {
				if (entry.is_regular_file() && entry.path().stem().string() == job.anim) {
					std::string extension = entry.path().extension().string();
					if (extension == ".gltf") { filePath = entry.path(); found = true; break; }
				}
			}
			if (!found) { continue; }

			// 2) 読み込み（Assimp）
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(filePath.string(), 0);
			if (scene->mNumAnimations == 0) { continue; }

			// 3) 既存と同じ解析
			std::unordered_map<std::string, AnimationData> localAnims;
			for (uint32_t i = 0; i < scene->mNumAnimations; ++i) {
				aiAnimation* animationAssimp = scene->mAnimations[i];
				AnimationData animation;

				std::string newName = (scene->mNumAnimations == 1)
					? job.model
					: job.model + "_" + animationAssimp->mName.C_Str();

				animation.duration = static_cast<float>(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);

				for (uint32_t c = 0; c < animationAssimp->mNumChannels; ++c) {
					aiNodeAnim* nodeAnim = animationAssimp->mChannels[c];
					NodeAnimation& node = animation.nodeAnimations[nodeAnim->mNodeName.C_Str()];
					// T
					for (uint32_t k = 0; k < nodeAnim->mNumPositionKeys; ++k) {
						aiVectorKey& kv = nodeAnim->mPositionKeys[k];
						KeyframeVector3 f; f.time = float(kv.mTime / animationAssimp->mTicksPerSecond);
						f.value = { -kv.mValue.x, kv.mValue.y, kv.mValue.z };
						node.translate.keyframes.push_back(f);
					}
					// R
					for (uint32_t k = 0; k < nodeAnim->mNumRotationKeys; ++k) {
						aiQuatKey& kv = nodeAnim->mRotationKeys[k];
						KeyframeQuaternion f; f.time = float(kv.mTime / animationAssimp->mTicksPerSecond);
						f.value = { kv.mValue.x, -kv.mValue.y, -kv.mValue.z, kv.mValue.w };
						node.rotate.keyframes.push_back(f);
					}
					// S
					for (uint32_t k = 0; k < nodeAnim->mNumScalingKeys; ++k) {
						aiVectorKey& kv = nodeAnim->mScalingKeys[k];
						KeyframeVector3 f; f.time = float(kv.mTime / animationAssimp->mTicksPerSecond);
						f.value = { kv.mValue.x, kv.mValue.y, kv.mValue.z };
						node.scale.keyframes.push_back(f);
					}
				}
				localAnims.emplace(newName, std::move(animation));
			}

			// 4) スケルトンとクラスター生成（モデルが必要）
			//    既存実装を流用
			//    skeletons_[animName] = CreateSkeleton(modelLoader_->GetModelData(model).rootNode);
			//    skinClusters_[animName] = CreateSkinCluster(model, animName);
			{
				std::scoped_lock lk(animMutex_);
				for (auto& [newName, anim] : localAnims) {
					animations_[newName] = std::move(anim);
					                    // 既存の流れと同じ
						skeletons_[newName] = CreateSkeleton(modelLoader_->GetModelData(job.model).rootNode);
					skinClusters_[newName] = CreateSkinCluster(job.model, newName);
					                    // ★骨とスキンクラはモデル名をキーに“未作成なら一度だけ”作る
						if (!skeletons_.contains(job.anim)) {
						skeletons_[job.anim] = CreateSkeleton(modelLoader_->GetModelData(job.model).rootNode);
						
					}
					if (!skinClusters_.contains(job.anim)) {
						skinClusters_[job.anim] = CreateSkinCluster(job.model, job.anim);
						
					}
				}
			}
		}
		});
}

void AnimationManager::Load(const std::string& animationName, const std::string& modelName) {

	RequestLoadAsync(animationName, modelName);
	// モデルとアニメが来るまで待つ（簡易）
	for (;;) {
		{
			std::scoped_lock lk(animMutex_);
			if (animations_.find(modelName) != animations_.end()) break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

Skeleton AnimationManager::CreateSkeleton(const Node& rootNode) {

	Skeleton skeleton;
	skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

	// 名前とIndexのマッピングを行う
	for (const auto& joint : skeleton.joints) {

		skeleton.jointMap.emplace(joint.name, joint.index);
	}

	return skeleton;
}

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

const AnimationData& AnimationManager::GetAnimationData(const std::string& animationName) const {

	std::scoped_lock lk(animMutex_);
	bool find = animations_.find(animationName) != animations_.end();
	if (!find) { /* ログは既存通り */ }
	ASSERT(find, "not found animation" + animationName); // 元実装踏襲 :contentReference[oaicite:25]{index=25}
	return animations_.at(animationName);
}

const Skeleton& AnimationManager::GetSkeletonData(const std::string& animationName) const {

	std::scoped_lock lk(animMutex_);
	bool find = skeletons_.find(animationName) != skeletons_.end();
	if (!find) { /* ログ */ }
	ASSERT(find, "not found animation" + animationName); // 元実装踏襲 :contentReference[oaicite:26]{index=26}
	return skeletons_.at(animationName);
}

const SkinCluster& AnimationManager::GetSkinClusterData(const std::string& animationName) const {

	std::scoped_lock lk(animMutex_);
	bool find = skinClusters_.find(animationName) != skinClusters_.end();
	if (!find) { /* ログ */ }
	ASSERT(find, "not found animation" + animationName); // 元実装踏襲 :contentReference[oaicite:27]{index=27}
	return skinClusters_.at(animationName);
}

void AnimationManager::RequestLoadAsync(const std::string& animationName, const std::string& modelName) {
	{
		std::scoped_lock lk(animMutex_);
		if (animations_.contains(modelName)) return; // 1本だけなら modelName をキーに上書きしない運用
	}
	std::scoped_lock lk(jobMutex_);
	jobs_.push_back({ animationName, modelName });
	SpdLogger::Log("[Anim][Enqueue] anim=" + animationName + " model=" + modelName);
	jobCv_.notify_one();
}

void AnimationManager::WaitAll() {
	for (;;) {
		{
			std::scoped_lock lk(jobMutex_);
			if (jobs_.empty()) break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}