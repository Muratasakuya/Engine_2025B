#include "FieldCrossMarkWall.h"

//============================================================================
//	FieldCrossMarkWall classMethods
//============================================================================

void FieldCrossMarkWall::Update() {

	// collision更新
	Collider::UpdateAllBodies(*transform_);
}

void FieldCrossMarkWall::OnCollisionEnter([[maybe_unused]] const CollisionBody* collisionBody) {


}

void FieldCrossMarkWall::OnCollisionStay([[maybe_unused]] const CollisionBody* collisionBody) {


}

void FieldCrossMarkWall::OnCollisionExit([[maybe_unused]] const CollisionBody* collisionBody) {


}