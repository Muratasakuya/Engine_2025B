#include "ModelComponentManager.h"

//============================================================================
//	ModelComponentManager classMethods
//============================================================================

ModelComponent* ModelComponentManager::AddComponent(
	EntityID entity, const std::string& modelName,
	const std::optional<std::string>& animationName, ID3D12Device* device,
	Asset* asset, SRVManager* srvManager) {

	// Animation有無
	components_[entity].isAnimation = animationName.has_value();

	// 初期化
	components_[entity].renderingData.drawEnable = true;
	components_[entity].renderingData.blendMode = BlendMode::kBlendModeNormal;

	if (components_[entity].isAnimation) {

		components_[entity].animationModel = std::make_unique<AnimationModel>();
		components_[entity].animationModel->Init(modelName, *animationName, device, asset, srvManager);
	} else {

		components_[entity].model = std::make_unique<BaseModel>();
		components_[entity].model->Init(modelName, device, asset);
	}
	return &components_[entity];
}

void ModelComponentManager::RemoveComponent(EntityID entity) {

	components_.erase(entity);
}

ModelComponent* ModelComponentManager::GetComponent(EntityID entity) {

	auto it = components_.find(entity);
	return (it != components_.end()) ? &it->second : nullptr;
}