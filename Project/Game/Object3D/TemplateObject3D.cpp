#include "TemplateObject3D.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Component/Manager/ComponentManager.h>

// imgui
#include <imgui.h>

//============================================================================
//	TemplateObject3D classMethods
//============================================================================

TemplateObject3D::TemplateObject3D() {

	colliderA_ = Collider::AddCollider(CollisionShape::Sphere());
	colliderA_->SetType(ColliderType::Type_None, ColliderType::Type_Test);

	colliderB_ = Collider::AddCollider(CollisionShape::OBB());
	colliderB_->SetType(ColliderType::Type_Test, ColliderType::Type_None);
}

void TemplateObject3D::Update() {

	// collision更新
	UpdateCollision();
}

void TemplateObject3D::UpdateCollision() {

	colliderA_->UpdateSphere({ .center = Vector3::AnyInit(0.0f),.radius = 1.0f });

	colliderB_->UpdateOBB({
		.center = Vector3::AnyInit(0.0f),
		.size = Vector3::AnyInit(1.0f),
		.rotate = Quaternion::IdentityQuaternion() });
}

void TemplateObject3D::OnCollisionEnter(const ColliderComponent* collider) {

	if (colliderA_->GetTargetType() == collider->GetType()) {

		int test = 0;
		++test;
	}

	if (colliderB_->GetTargetType() == collider->GetType()) {

		int test = 0;
		++test;
	}
}

void TemplateObject3D::OnCollisionStay(
	[[maybe_unused]] const ColliderComponent* collider) {


}

void TemplateObject3D::OnCollisionExit(
	[[maybe_unused]] const ColliderComponent* collider) {


}