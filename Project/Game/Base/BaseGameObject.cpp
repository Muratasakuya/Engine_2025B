#include "BaseGameObject.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Component/Manager/ComponentManager.h>

//============================================================================
//	BaseGameObject classMethods
//============================================================================

void BaseGameObject::CreateModel(const std::string& modelName,
	const std::optional<std::string>& animationName) {

	object_ = ComponentManager::GetInstance()->CreateObject3D(
		modelName,animationName, GetObjectName());

	uvTransforms_.resize(object_.materials.size());
	for (uint32_t index = 0; index < uvTransforms_.size(); ++index) {

		uvTransforms_[index].scale = Vector3::AnyInit(1.0f);
		object_.materials[index]->uvTransform = Matrix4x4::MakeAffineMatrix(
			uvTransforms_[index].scale, uvTransforms_[index].rotate, uvTransforms_[index].translate);
	}
}

std::string BaseGameObject::GetObjectName() const {

	std::string className = typeid(*this).name();
	std::string prefix = "class ";
	if (className.find(prefix) == 0) {
		className = className.substr(prefix.length());
	}
	return className;
}