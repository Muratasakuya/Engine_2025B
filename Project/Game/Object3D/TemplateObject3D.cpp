#include "TemplateObject3D.h"

//============================================================================
//	include
//============================================================================

// imgui
#include <imgui.h>

//============================================================================
//	TemplateObject3D classMethods
//============================================================================

TemplateObject3D::TemplateObject3D() {

	BaseGameObject::CreateModel("teapot");

	// colliderA
	colliderA_ = BaseGameObject::AddCollider(CollisionShape::Sphere());
	colliderA_->type_ = ColliderType::Type_None;
	colliderA_->targetType_ = ColliderType::Type_Test;
	colliderA_->radius_ = 4.0f;

	// colliderB
	colliderB_ = BaseGameObject::AddCollider(CollisionShape::Sphere());
	colliderB_->type_ = ColliderType::Type_Test;
	colliderB_->targetType_ = ColliderType::Type_None;
	colliderB_->radius_ = 4.0f;
}

void TemplateObject3D::Update() {

	// imgui更新
	UpdateImGui();
	// 衝突更新
	UpdateCollision();
}

void TemplateObject3D::OnCollisionEnter(const ColliderComponent* collider, const ColliderID colliderId) {

	// colliderIdが"0"のOnCollisionEnter
	if (colliderId == 0) {
		if (collider->type_ == ColliderType::Type_Test) {

			int a = 10;
			a = colliderId;
		}
	}
	// colliderIdが"1"のOnCollisionEnter
	else if (colliderId == 1) {
		if (collider->type_ == ColliderType::Type_Test) {

			int a = 10;
			a = colliderId;
		}
	}
}

void TemplateObject3D::UpdateCollision() {

	colliderA_->Update();
	colliderB_->Update();
}

void TemplateObject3D::UpdateImGui() {

	// imguiの更新
	object_.imguiFunc = [&]() {

		ImGui::DragFloat3("colliderAPos", &colliderA_->centerPos_.x, 0.01f);
		ImGui::DragFloat3("colliderBPos", &colliderB_->centerPos_.x, 0.01f);
		};
	BaseGameObject::SetImGui();
}