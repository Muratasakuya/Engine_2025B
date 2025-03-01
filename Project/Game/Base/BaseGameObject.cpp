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
	object_ = EntityComponent::GetInstance()->AddComponent(
		modelName, animationName, GetObjectName());

	// 初期化
	object_->transform.Init();
	object_->transform.UpdateMatrix();
	for (uint32_t index = 0; index < object_->materials.size(); ++index) {

		object_->uvTransforms[index].scale = Vector3::AnyInit(1.0f);
		object_->materials[index].Init();
	}
	object_->renderingData.blendMode = kBlendModeNormal;
	object_->renderingData.drawEnable = true;
}

std::string BaseGameObject::GetObjectName() const {

	std::string className = typeid(*this).name();
	std::string prefix = "class ";
	if (className.find(prefix) == 0) {
		className = className.substr(prefix.length());
	}
	return className;
}