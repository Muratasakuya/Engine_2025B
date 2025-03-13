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

	teapotId_ = ComponentManager::GetInstance()->CreateObject3D("teapot", std::nullopt, "teapot");

	// colliderA
	colliderA_ = BaseGameObject::AddCollider(CollisionShape::Sphere());
	colliderA_->type_ = ColliderType::Type_None;
	colliderA_->targetType_ = ColliderType::Type_Test;
	colliderA_->radius_ = 4.0f;

	// colliderB
	colliderB_ = BaseGameObject::AddCollider(CollisionShape::OBB());
	colliderB_->type_ = ColliderType::Type_Test;
	colliderB_->targetType_ = ColliderType::Type_None;
	colliderB_->size_ = Vector3::AnyInit(4.0f);
	colliderB_->rotate_.Init();
}

void TemplateObject3D::Update() {

	// 衝突更新
	UpdateCollision();
}

void TemplateObject3D::UpdateCollision() {

	colliderA_->Update();
	colliderB_->Update();
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