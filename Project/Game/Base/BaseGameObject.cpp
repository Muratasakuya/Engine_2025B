#include "BaseGameObject.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Component/Manager/ComponentManager.h>
#include <Engine/Component/Manager/CollisionManager.h>

//============================================================================
//	BaseGameObject classMethods
//============================================================================

void BaseGameObject::CreateModel(const std::string& modelName,
	const std::optional<std::string>& animationName) {

	object_ = ComponentManager::GetInstance()->CreateObject3D(
		modelName, animationName, GetObjectName());

	uvTransforms_.resize(object_.materials.size());
	for (uint32_t index = 0; index < uvTransforms_.size(); ++index) {

		uvTransforms_[index].scale = Vector3::AnyInit(1.0f);
		object_.materials[index]->uvTransform = Matrix4x4::MakeAffineMatrix(
			uvTransforms_[index].scale, uvTransforms_[index].rotate, uvTransforms_[index].translate);
	}
}

ColliderComponent* BaseGameObject::AddCollider(const CollisionShape::Shapes& shape) {

	ColliderComponent* collider = nullptr;
	collider = CollisionManager::GetInstance()->AddComponent(shape);

	// colliderId付与
	collider->id_ = colliderId_;
	// idを進める
	++colliderId_;

	// 関数結び付け
	collider->SetOnCollisionEnter([this, collider](ColliderComponent* otherCollider) {
		OnCollisionEnter(otherCollider, collider->id_);
		});
	collider->SetOnCollisionStay([this, collider](ColliderComponent* otherCollider) {
		OnCollisionStay(otherCollider, collider->id_);
		});
	collider->SetOnCollisionExit([this, collider](ColliderComponent* otherCollider) {
		OnCollisionExit(otherCollider, collider->id_);
		});

	return collider;
}

void BaseGameObject::RemoveCollider(ColliderComponent* collider) {

	CollisionManager::GetInstance()->RemoveComponent(collider);
}

void BaseGameObject::SetImGui() {

	ComponentManager::GetInstance()->SetImGuiFunction(object_.id, object_.imguiFunc);
}

std::string BaseGameObject::GetObjectName() const {

	std::string className = typeid(*this).name();
	std::string prefix = "class ";
	if (className.find(prefix) == 0) {
		className = className.substr(prefix.length());
	}
	return className;
}