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

	// collision
	colliderA_ = BaseGameObject::AddCollider(CollisionShape::Sphere());
	colliderA_->type_ = ColliderType::Type_None;
	colliderA_->targetType_ = ColliderType::Type_Test;
	colliderA_->radius_ = 4.0f;

	colliderB_ = BaseGameObject::AddCollider(CollisionShape::Sphere());
	colliderB_->type_ = ColliderType::Type_Test;
	colliderB_->targetType_ = ColliderType::Type_None;
	colliderB_->radius_ = 4.0f;
}

void TemplateObject3D::Update() {

	object_.imguiFunc = [&]() {

		ImGui::DragFloat3("colliderAPos", &colliderA_->centerPos_.x, 0.01f);
		ImGui::DragFloat3("colliderBPos", &colliderA_->centerPos_.x, 0.01f);
		};

	// 衝突更新
	UpdateCollision();
}

void TemplateObject3D::UpdateCollision() {

	colliderA_->Update();
	colliderB_->Update();

	colliderA_->onEnter_ = []([[maybe_unused]] ColliderComponent* collider) {

		int a = 10;
		a = 0;
		};
}