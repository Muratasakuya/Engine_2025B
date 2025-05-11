#include "AnimationManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Core/Debug/Logger.h>
#include <Engine/Core/Graphics/Descriptors/SRVDescriptor.h>
#include <Engine/Core/Graphics/Lib/DxUtils.h>
#include <Engine/Asset/ModelLoader.h>
#include <Engine/Asset/Filesystem.h>

//============================================================================
//	AnimationManager classMethods
//============================================================================

void AnimationManager::Init(ID3D12Device* device, SRVDescriptor* srvDescriptor, ModelLoader* modelLoader) {

	device_ = nullptr;
	device_ = device;

	srvDescriptor_ = nullptr;
	srvDescriptor_ = srvDescriptor;

	modelLoader_ = nullptr;
	modelLoader_ = modelLoader;

	baseDirectoryPath_ = "./Assets/Models/";
}

void AnimationManager::Load(const std::string& animationName, const std::string& modelName) {

	// animation検索
	std::filesystem::path filePath;
	bool found = false;
	for (const auto& entry : std::filesystem::recursive_directory_iterator(baseDirectoryPath_)) {
		if (entry.is_regular_file() &&
			entry.path().stem().string() == animationName) {

			std::string extension = entry.path().extension().string();
			if (extension == ".gltf" || extension == ".fbx") {
				filePath = entry.path();
				found = true;
				break;
			}
		}
	}
	ASSERT(found, "animation not found in directory or its subdirectories: " + animationName);

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath.string(), 0);

	// アニメーションがない場合はエラー
	assert(scene->mNumAnimations != 0);

	// すべてのアニメーションを処理
	for (uint32_t animationIndex = 0; animationIndex < scene->mNumAnimations; ++animationIndex) {
		aiAnimation* animationAssimp = scene->mAnimations[animationIndex];
		AnimationData animation;

		std::string newAnimationName{};
		// アニメーションの数が1個ならmodelの名前をそのまま使う
		if (scene->mNumAnimations == 1) {

			// アニメーション名を設定
			newAnimationName = modelName;
		} else {

			// アニメーション名を設定
			newAnimationName = modelName + "_" + animationAssimp->mName.C_Str();
		}

		// 時間の単位を秒に変換
		animation.duration = static_cast<float>(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);

		// チャンネルごとに処理
		for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {

			aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
			NodeAnimation& nodeAnimation = animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];

			// Translation Keys
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
				aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
				KeyframeVector3 keyframe;
				keyframe.time = static_cast<float>(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
				keyframe.value = { -keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z }; // 右手→左手
				nodeAnimation.translate.keyframes.push_back(keyframe);
			}

			// Rotation Keys
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
				aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
				KeyframeQuaternion keyframe;
				keyframe.time = static_cast<float>(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
				keyframe.value = { keyAssimp.mValue.x, -keyAssimp.mValue.y, -keyAssimp.mValue.z, keyAssimp.mValue.w }; // 右手→左手
				nodeAnimation.rotate.keyframes.push_back(keyframe);
			}

			// Scaling Keys
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
				aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
				KeyframeVector3 keyframe;
				keyframe.time = static_cast<float>(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
				keyframe.value = { keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z }; // スケールはそのまま
				nodeAnimation.scale.keyframes.push_back(keyframe);
			}
		}

		// for分で回した分だけ取得
		animations_[newAnimationName] = animation;
		Logger::Log("load animation: " + newAnimationName);
	}

	// 骨とクラスターを作成する
	skeletons_[animationName] = CreateSkeleton(modelLoader_->GetModelData(modelName).rootNode);
	skinClusters_[animationName] = CreateSkinCluster(modelName, animationName);
}

void AnimationManager::ApplyAnimation(Skeleton& skeleton,
	const AnimationData& animationData, float animationTime) {

	for (auto& joint : skeleton.joints) {

		// 対象のJointのAnimationがあれば、値の適応を行う
		if (auto it = animationData.nodeAnimations.find(joint.name);
			it != animationData.nodeAnimations.end()) {

			const auto& rootNodeAnimation = (*it).second;
			joint.transform.translation = Vector3::CalculateValue(rootNodeAnimation.translate.keyframes, animationTime);
			joint.transform.rotation = Quaternion::CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime);
			joint.transform.scale = Vector3::CalculateValue(rootNodeAnimation.scale.keyframes, animationTime);
		}
	}
}

void AnimationManager::SkeletonUpdate(Skeleton& skeleton) {

	// 全てのJointを更新、親が若いので通常ループで処理可能
	for (auto& joint : skeleton.joints) {

		joint.localMatrix =
			Matrix4x4::MakeAxisAffineMatrix(joint.transform.scale, joint.transform.rotation, joint.transform.translation);
		// 親がいれば親の行列を掛ける
		if (joint.parent) {

			joint.skeletonSpaceMatrix = joint.localMatrix * skeleton.joints[*joint.parent].skeletonSpaceMatrix;
		}
		// 親がいないのでlocalMatrixとSkeletonSpaceMatrixは一致する
		else {

			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}
}

void AnimationManager::SkinClusterUpdate(SkinCluster& skinCluster, const Skeleton& skeleton) {

	for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex) {

		assert(jointIndex < skinCluster.inverseBindPoseMatrices.size());

		skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix =
			skinCluster.inverseBindPoseMatrices[jointIndex] *
			skeleton.joints[jointIndex].skeletonSpaceMatrix;
		skinCluster.mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix =
			Matrix4x4::Transpose(Matrix4x4::Inverse(skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix));
	}
}

void AnimationManager::BlendAnimation(Skeleton& skeleton,
	const AnimationData& oldAnimationData, float oldAnimTime,
	const AnimationData& nextAnimationData, float nextAnimTime, float alpha) {

	// すべてのJointを対象
	for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex) {

		auto& jointOld = skeleton.joints[jointIndex];

		const std::string& nodeName = jointOld.name;

		// old の transform
		Vector3 posOld = Vector3(0.0f, 0.0f, 0.0f);
		Quaternion rotOld = Quaternion::IdentityQuaternion();
		Vector3 sclOld = Vector3(1.0f, 1.0f, 1.0f);
		if (auto itOld = oldAnimationData.nodeAnimations.find(nodeName); itOld != oldAnimationData.nodeAnimations.end()) {

			const auto& rootNodeAnimation = itOld->second;
			if (!rootNodeAnimation.translate.keyframes.empty()) {

				posOld = Vector3::CalculateValue(rootNodeAnimation.translate.keyframes, oldAnimTime);
			}
			if (!rootNodeAnimation.rotate.keyframes.empty()) {

				rotOld = Quaternion::CalculateValue(rootNodeAnimation.rotate.keyframes, oldAnimTime);
			}
			if (!rootNodeAnimation.scale.keyframes.empty()) {

				sclOld = Vector3::CalculateValue(rootNodeAnimation.scale.keyframes, oldAnimTime);
			}
		}

		// next の transform
		Vector3 posNext = Vector3(0.0f, 0.0f, 0.0f);
		Quaternion rotNext = Quaternion::IdentityQuaternion();
		Vector3 sclNext = Vector3(1.0f, 1.0f, 1.0f);
		if (auto itNext = nextAnimationData.nodeAnimations.find(nodeName); itNext != nextAnimationData.nodeAnimations.end()) {
			const auto& rootNodeAnimation = itNext->second;

			if (!rootNodeAnimation.translate.keyframes.empty()) {

				posNext = Vector3::CalculateValue(rootNodeAnimation.translate.keyframes, nextAnimTime);
			}
			if (!rootNodeAnimation.rotate.keyframes.empty()) {

				rotNext = Quaternion::CalculateValue(rootNodeAnimation.rotate.keyframes, nextAnimTime);
			}
			if (!rootNodeAnimation.scale.keyframes.empty()) {

				sclNext = Vector3::CalculateValue(rootNodeAnimation.scale.keyframes, nextAnimTime);
			}
		}

		// αブレンド
		Vector3 posBlend = Vector3::Lerp(posOld, posNext, alpha);
		Quaternion rotBlend = Quaternion::Slerp(rotOld, rotNext, alpha);
		Vector3 sclBlend = Vector3::Lerp(sclOld, sclNext, alpha);

		jointOld.transform.translation = posBlend;
		jointOld.transform.rotation = rotBlend;
		jointOld.transform.scale = sclBlend;
	}
}

Skeleton AnimationManager::CreateSkeleton(const Node& rootNode) {

	Skeleton skeleton;
	skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

	// 名前とIndexのマッピングを行う
	for (const auto& joint : skeleton.joints) {

		skeleton.jointMap.emplace(joint.name, joint.index);
	}

	// スケルトンの更新
	SkeletonUpdate(skeleton);

	return skeleton;
}

int32_t AnimationManager::CreateJoint(const Node& node, const std::optional<int32_t> parent, std::vector<Joint>& joints) {

	Joint joint;
	joint.name = node.name;
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

	ASSERT(animations_.find(animationName) != animations_.end(), "not found animation" + animationName);
	return animations_.at(animationName);
}

const Skeleton& AnimationManager::GetSkeletonData(const std::string& animationName) const {

	ASSERT(skeletons_.find(animationName) != skeletons_.end(), "not found animation" + animationName);
	return skeletons_.at(animationName);
}

const SkinCluster& AnimationManager::GetSkinClusterData(const std::string& animationName) const {

	ASSERT(skinClusters_.find(animationName) != skinClusters_.end(), "not found animation" + animationName);
	return skinClusters_.at(animationName);
}