#include "BaseGameObject.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Component/EntityComponent.h>

//============================================================================
//	BaseGameObject classMethods
//============================================================================

void BaseGameObject::CreateModel(const std::string& modelName,
	const std::optional<std::string>& animationName) {

	object_ = nullptr;
	object_ = EntityComponent::GetInstance()->AddComponent(modelName, animationName);

	// 初期化
	object_->transform.Init();
	object_->transform.UpdateMatrix();
	for (auto& material : object_->materials) {
		material.Init();
	}
	object_->renderingData.blendMode = kBlendModeNormal;
	object_->renderingData.drawEnable = true;
}