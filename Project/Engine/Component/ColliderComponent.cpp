#include "ColliderComponent.h"

//============================================================================
//	ColliderComponent classMethods
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Game/Base/BaseGameObject.h>

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

void ColliderComponent::Update() {
	if (!shape_.has_value()) {
		ASSERT(FALSE, "collider not setting");
	}

	if (shape_ && std::holds_alternative <CollisionShape::Sphere>(*shape_)) {
		CollisionShape::Sphere& sphere = std::get<CollisionShape::Sphere>(*shape_);

		sphere.radius = radius_;

	} else if (shape_ && std::holds_alternative <CollisionShape::OBB>(*shape_)) {
		CollisionShape::OBB& obb = std::get<CollisionShape::OBB>(*shape_);

		obb.center = centerPos_;
		obb.rotate = rotate_;
		obb.size = size_;
	}
}