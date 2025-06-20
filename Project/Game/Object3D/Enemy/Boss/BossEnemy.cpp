#include "BossEnemy.h"

//============================================================================
//	include
//============================================================================
#include <Lib/Adapter/JsonAdapter.h>

//============================================================================
//	BossEnemy classMethods
//============================================================================

void BossEnemy::InitAnimations() {

	// 最初は待機状態で初期化
	animation_->SetPlayAnimation("bossEnemy_idle", true);

	// animationのデータを設定
	animation_->SetAnimationData("bossEnemy_falter");      // 怯み
	animation_->SetAnimationData("bossEnemy_lightAttack"); // 弱攻撃
}

void BossEnemy::InitCollision() {

	CollisionBody* body = bodies_.emplace_back(Collider::AddCollider(CollisionShape::OBB().Default()));
	bodyOffsets_.emplace_back(CollisionShape::OBB().Default());

	// タイプ設定
	body->SetType(ColliderType::Type_BossEnemy);
	body->SetTargetType(ColliderType::Type_Player);
}

void BossEnemy::SetInitTransform() {

	transform_->scale = initTransform_.scale;
	transform_->eulerRotate = initTransform_.eulerRotate;
	transform_->rotation = initTransform_.rotation;
	transform_->translation = initTransform_.translation;
}

void BossEnemy::DerivedInit() {

	// animation初期化、設定
	InitAnimations();

	// collision初期化、設定
	InitCollision();

	// json適応
	ApplyJson();
}

void BossEnemy::Update() {

	Collider::UpdateAllBodies(*transform_);
}

void BossEnemy::OnCollisionEnter([[maybe_unused]] const CollisionBody* collisionBody) {


}

void BossEnemy::DerivedImGui() {

	if (ImGui::CollapsingHeader("Init")) {
		if (ImGui::Button("SaveJson...initParameter.json")) {

			SaveJson();
		}
		if (ImGui::CollapsingHeader("Transform")) {

			initTransform_.ImGui(itemWidth_);
			SetInitTransform();
		}

		if (ImGui::CollapsingHeader("Collision")) {

			Collider::ImGui(itemWidth_);
		}
	}
}

void BossEnemy::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck("Enemy/Boss/initParameter.json", data)) {
		return;
	}

	initTransform_.FromJson(data["Transform"]);
	SetInitTransform();

	GameEntity3D::ApplyMaterial(data);
	Collider::ApplyBodyOffset(data);
}

void BossEnemy::SaveJson() {

	Json data;

	initTransform_.ToJson(data["Transform"]);
	GameEntity3D::SaveMaterial(data);
	Collider::SaveBodyOffset(data);

	JsonAdapter::Save("Enemy/Boss/initParameter.json", data);
}