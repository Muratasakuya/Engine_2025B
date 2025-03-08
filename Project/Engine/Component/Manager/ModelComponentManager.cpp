#include "ModelComponentManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Managers/SRVManager.h>
#include <Lib/MathUtils/Algorithm.h>

//============================================================================
//	ModelComponentManager classMethods
//============================================================================

void ModelComponentManager::AddComponent(EntityID entity, std::any args) {

	auto [modelName, animationName, device, asset, srvManager] =
		std::any_cast<std::tuple<
		std::string, std::optional<std::string>, ID3D12Device*, Asset*, SRVManager*>>(args);

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
}

void ModelComponentManager::RemoveComponent(EntityID entity) {

	components_.erase(entity);
}

ModelComponent* ModelComponentManager::GetComponent(EntityID entity) {

	if (Algorithm::Find(components_, entity, true)) {

		return &components_[entity];
	}
	return nullptr;
}