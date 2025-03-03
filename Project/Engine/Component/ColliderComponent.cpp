#include "ColliderComponent.h"

//============================================================================
//	ColliderComponent classMethods
//============================================================================

void ColliderComponent::TriggerOnCollisionEnter(ColliderComponent* collider) {

	if (onEnter_) {

		// 衝突した瞬間
		onEnter_(collider);
	}
}

void ColliderComponent::TriggerOnCollisionStay(ColliderComponent* collider) {

	if (onStay_) {

		// 衝突中
		onStay_(collider);
	}
}

void ColliderComponent::TriggerOnCollisionExit(ColliderComponent* collider) {

	if (onExit_) {

		// 離れた瞬間
		onExit_(collider);
	}
}