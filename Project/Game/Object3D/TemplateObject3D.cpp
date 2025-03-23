#include "TemplateObject3D.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Component/User/ComponentHelper.h>

// imgui
#include <imgui.h>

//============================================================================
//	TemplateObject3D classMethods
//============================================================================

TemplateObject3D::TemplateObject3D() {

	modelIdA_ = GameObjectHelper::CreateObject3D("teapot", "teapot", "Teapot", "teapot");
	modelIdB_ = GameObjectHelper::CreateObject3D("teapot", "teapot", "Teapot", "teapot");

	colliderA_ = Collider::AddCollider(CollisionShape::Sphere());
	colliderA_->SetType(ColliderType::Type_None, ColliderType::Type_Test);

	colliderB_ = Collider::AddCollider(CollisionShape::OBB());
	colliderB_->SetType(ColliderType::Type_Test, ColliderType::Type_None);
}

TemplateObject3D::~TemplateObject3D() {

	GameObjectHelper::RemoveObject3D(modelIdA_);
	GameObjectHelper::RemoveObject3D(modelIdB_);
}

void TemplateObject3D::Update() {

	// collision更新
	UpdateCollision();
}

void TemplateObject3D::UpdateCollision() {

	auto transform = Component::GetComponent<Transform3DComponent>(modelIdA_);
	colliderA_->UpdateSphere({ .center = Vector3::AnyInit(0.0f),.radius = 1.0f });

	if (!transform) {
		return;
	}

	colliderB_->UpdateOBB({
		.center = transform->translation,
		.size = transform->scale,
		.rotate = transform->rotation });
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