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

	GameObjectHelper::CreateObject2D("uvChecker", "uvChecker");

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

	auto transformA = Component::GetComponent<Transform3DComponent>(modelIdA_);
	auto transformB = Component::GetComponent<Transform3DComponent>(modelIdB_);

	if (!transformA) {
		return;
	}

	colliderA_->UpdateSphere({ .center = transformA->translation,.radius = 1.0f });

	if (!transformB) {
		return;
	}

	colliderB_->UpdateOBB({
		.center = transformB->translation,
		.size = transformB->scale,
		.rotate = transformB->rotation });
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