#include "Collider.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/Manager/ComponentManager.h>
#include <Engine/Core/Component/Manager/CollisionManager.h>

//============================================================================
//	Collider classMethods
//============================================================================

ColliderComponent* Collider::AddCollider(const CollisionShape::Shapes& shape) {

	ColliderComponent* collider = nullptr;
	collider = CollisionManager::GetInstance()->AddComponent(shape);

	// 関数結び付け
	collider->SetOnCollisionEnter([this](ColliderComponent* otherCollider) {
		OnCollisionEnter(otherCollider);
		});
	collider->SetOnCollisionStay([this](ColliderComponent* otherCollider) {
		OnCollisionStay(otherCollider);
		});
	collider->SetOnCollisionExit([this](ColliderComponent* otherCollider) {
		OnCollisionExit(otherCollider);
		});

	return collider;
}

void Collider::RemoveCollider(ColliderComponent* collider) {

	CollisionManager::GetInstance()->RemoveComponent(collider);
}