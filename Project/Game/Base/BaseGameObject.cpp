#include "BaseGameObject.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Component/Manager/ComponentManager.h>
#include <Engine/Component/Manager/CollisionManager.h>

//============================================================================
//	BaseGameObject classMethods
//============================================================================

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

std::string BaseGameObject::GetObjectName() const {

	std::string className = typeid(*this).name();
	std::string prefix = "class ";
	if (className.find(prefix) == 0) {
		className = className.substr(prefix.length());
	}
	return className;
}