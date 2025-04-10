#include "TemplateObject3D.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/ComponentHelper.h>

// imgui
#include <imgui.h>

//============================================================================
//	TemplateObject3D classMethods
//============================================================================

TemplateObject3D::TemplateObject3D() {

	modelIdA_ = GameObjectHelper::CreateObject3D("teapot", "teapot", "Teapot", "teapot");
	modelIdB_ = GameObjectHelper::CreateObject3D("teapot", "teapot", "Teapot", "teapot");

	GameObjectHelper::CreateObject2D("uvChecker", "uvChecker");

	collisionBodyA_ = Collider::AddCollider(CollisionShape::Sphere());
	collisionBodyA_->SetType(ColliderType::Type_None, ColliderType::Type_Test);

	collisionBodyB_ = Collider::AddCollider(CollisionShape::OBB());
	collisionBodyB_->SetType(ColliderType::Type_Test, ColliderType::Type_None);
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

	collisionBodyA_->UpdateSphere({ .center = transformA->translation,.radius = 1.0f });

	if (!transformB) {
		return;
	}

	collisionBodyB_->UpdateOBB({
		.center = transformB->translation,
		.size = transformB->scale,
		.rotate = transformB->rotation });
}

void TemplateObject3D::OnCollisionEnter(const CollisionBody* collisionBody) {

	if (collisionBodyA_->GetTargetType() == collisionBody->GetType()) {

		int test = 0;
		++test;
	}

	if (collisionBodyB_->GetTargetType() == collisionBody->GetType()) {

		int test = 0;
		++test;
	}
}

void TemplateObject3D::OnCollisionStay(
	[[maybe_unused]] const CollisionBody* collisionBody) {


}

void TemplateObject3D::OnCollisionExit(
	[[maybe_unused]] const CollisionBody* collisionBody) {


}