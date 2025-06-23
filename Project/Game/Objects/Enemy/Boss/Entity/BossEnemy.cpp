#include "BossEnemy.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>
#include <Lib/Adapter/JsonAdapter.h>

//============================================================================
//	BossEnemy classMethods
//============================================================================

void BossEnemy::InitWeapon() {

	weapon_ = std::make_unique<BossEnemyWeapon>();
	weapon_->Init("bossEnemyWeapon", "bossEnemyWeapon", "Enemy");

	// 武器を右手を親として動かす
	if (const auto& hand = GetJointTransform("rightHand")) {

		weapon_->SetParent(*hand);
	}
}

void BossEnemy::InitAnimations() {

	// 最初は待機状態で初期化
	animation_->SetPlayAnimation("bossEnemy_idle", true);

	// animationのデータを設定
	animation_->SetAnimationData("bossEnemy_falter");      // 怯み
	animation_->SetAnimationData("bossEnemy_lightAttack"); // 弱攻撃

	// 右手を親として更新させる
	animation_->SetParentJoint("rightHand");
}

void BossEnemy::InitCollision() {

	CollisionBody* body = bodies_.emplace_back(Collider::AddCollider(CollisionShape::OBB().Default()));
	bodyOffsets_.emplace_back(CollisionShape::OBB().Default());

	// タイプ設定
	body->SetType(ColliderType::Type_BossEnemy);
	body->SetTargetType(ColliderType::Type_Player);
}

void BossEnemy::InitState() {

	// 初期化、ここで初期状態も設定
	stateController_ = std::make_unique<BossEnemyStateController>();
	stateController_->Init(*this);

	// playerをセット
	stateController_->SetPlayer(player_);
}

void BossEnemy::InitHUD() {

	// HUDの初期化
	hudSprites_ = std::make_unique<BossEnemyHUD>();
	hudSprites_->Init();
}

void BossEnemy::SetInitTransform() {

	transform_->scale = initTransform_.scale;
	transform_->eulerRotate = initTransform_.eulerRotate;
	transform_->rotation = initTransform_.rotation;
	transform_->translation = initTransform_.translation;
}

void BossEnemy::DerivedInit() {

	// 使用する武器を初期化
	InitWeapon();

	// animation初期化、設定
	InitAnimations();

	// collision初期化、設定
	InitCollision();

	// 状態初期化
	InitState();

	// HUD初期化
	InitHUD();

	// json適応
	ApplyJson();
}

void BossEnemy::SetNextAnimation(const std::string& nextAnimationName,
	bool loopAnimation, float transitionDuration) {

	// 次のanimationを設定
	animation_->SwitchAnimation(nextAnimationName, loopAnimation, transitionDuration);
}

void BossEnemy::SetPlayer(const Player* player) {

	player_ = nullptr;
	player_ = player;

	stateController_->SetPlayer(player);
}

void BossEnemy::Update() {

	// 状態の更新
	stateController_->Update(*this);

	// 武器の更新
	weapon_->Update();

	// HUDの更新
	hudSprites_->SetStatas(stats_);
	hudSprites_->Update();

	// 衝突情報更新
	Collider::UpdateAllBodies(*transform_);
}

void BossEnemy::OnCollisionEnter([[maybe_unused]] const CollisionBody* collisionBody) {


}

void BossEnemy::DerivedImGui() {

	ImGui::SeparatorText("Statas");

	ImGui::Text("currentHP: %d / %d", stats_.currentHP, stats_.maxHP);
	ImGui::DragInt("maxHP", &stats_.maxHP, 1);
	ImGui::DragInt("currentHP", &stats_.currentHP, 1);
	if (ImGui::Button("ResetHP")) {

		// HPをリセットする
		stats_.currentHP = stats_.maxHP;
	}

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

	if (ImGui::CollapsingHeader("State")) {

		stateController_->ImGui();
	}

	if (ImGui::CollapsingHeader("HUD")) {

		hudSprites_->ImGui();
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

	stats_.maxHP = JsonAdapter::GetValue<int>(data, "maxHP");
	// 初期化時は最大HPと同じ値にする
	stats_.currentHP = stats_.maxHP;
}

void BossEnemy::SaveJson() {

	Json data;

	initTransform_.ToJson(data["Transform"]);
	GameEntity3D::SaveMaterial(data);
	Collider::SaveBodyOffset(data);

	data["maxHP"] = stats_.maxHP;

	JsonAdapter::Save("Enemy/Boss/initParameter.json", data);
}