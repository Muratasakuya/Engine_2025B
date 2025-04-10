#include "Collider.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Collision/CollisionManager.h>

//============================================================================
//	Collider classMethods
//============================================================================

CollisionBody* Collider::AddCollider(const CollisionShape::Shapes& shape) {

	CollisionBody* collider = nullptr;
	collider = CollisionManager::GetInstance()->AddCollisionBody(shape);

	// 関数結び付け
	collider->SetOnCollisionEnter([this](CollisionBody* otherCollider) {
		OnCollisionEnter(otherCollider);
		});
	collider->SetOnCollisionStay([this](CollisionBody* otherCollider) {
		OnCollisionStay(otherCollider);
		});
	collider->SetOnCollisionExit([this](CollisionBody* otherCollider) {
		OnCollisionExit(otherCollider);
		});

	return collider;
}

void Collider::RemoveCollider(CollisionBody* collisionBody) {

	CollisionManager::GetInstance()->RemoveCollisionBody(collisionBody);
}