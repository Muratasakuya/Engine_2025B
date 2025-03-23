#include "ColliderComponent.h"

//============================================================================
//	ColliderComponent classMethods
//============================================================================
#include <Engine/Component/User/Collider.h>
#include <Engine/Core/Debug/Assert.h>

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

void ColliderComponent::UpdateSphere(const CollisionShape::Sphere& sphere) {

	if (std::holds_alternative<CollisionShape::Sphere>(shape_)) {

		std::get<CollisionShape::Sphere>(shape_) = sphere;
	} else {

		ASSERT(FALSE, "collision shape is not 'sphere'");
	}
}

void ColliderComponent::UpdateOBB(const CollisionShape::OBB& obb) {

	if (std::holds_alternative<CollisionShape::OBB>(shape_)) {

		std::get<CollisionShape::OBB>(shape_) = obb;
	} else {

		ASSERT(FALSE, "collision shape is not 'obb'");
	}
}

void ColliderComponent::SetType(ColliderType type, ColliderType target) {

	type_ = type;
	targetType_ = target;
}