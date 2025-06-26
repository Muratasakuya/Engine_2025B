#include "Player.h"

//============================================================================
//	include
//============================================================================

//============================================================================
//	Player classMethods
//============================================================================

void Player::InitState() {

	// 初期化、ここで初期状態も設定
	stateController_ = std::make_unique<PlayerStateController>();
	stateController_->Init(*this);
}

void Player::InitHUD() {

	// HUDの初期化
	hudSprites_ = std::make_unique<PlayerHUD>();
	hudSprites_->Init();
}

void Player::DerivedInit() {

	// 状態初期化
	InitState();

	// HUD初期化
	InitHUD();

	// json適応
	ApplyJson();
}

void Player::SetBossEnemy(const BossEnemy* bossEnemy) {

	bossEnemy_ = nullptr;
	bossEnemy_ = bossEnemy;

	stateController_->SetBossEnemy(bossEnemy);
}

void Player::Update() {

	// 状態の更新
	stateController_->SetStatas(stats_);
	stateController_->Update(*this);

	// HUDの更新
	hudSprites_->SetStatas(stats_);
	hudSprites_->Update();
}

void Player::OnCollisionEnter([[maybe_unused]] const CollisionBody* collisionBody) {
}

void Player::DerivedImGui() {

	ImGui::Text("currentHP: %d / %d", stats_.currentHP, stats_.maxHP);
	ImGui::DragInt("maxHP", &stats_.maxHP, 1, 0);
	ImGui::DragInt("currentHP", &stats_.currentHP, 1, 0, stats_.maxHP);
	if (ImGui::Button("ResetHP")) {

		// HPをリセットする
		stats_.currentHP = stats_.maxHP;
	}

	ImGui::SeparatorText("SkilPoint");

	ImGui::Text("currentSkilPoint: %d / %d", stats_.currentSkilPoint, stats_.maxSkilPoint);
	ImGui::DragInt("maxSkilPoint", &stats_.maxSkilPoint, 1, 0);
	ImGui::DragInt("currentSkilPoint", &stats_.currentSkilPoint, 1, 0, stats_.maxSkilPoint);
	if (ImGui::Button("ResetSkilPoint")) {

		// スキル値をリセットする
		stats_.currentSkilPoint = 0;
	}

	if (ImGui::Button("SaveJson...initParameter.json")) {

		SaveJson();
	}

	if (ImGui::CollapsingHeader("HUD")) {

		hudSprites_->ImGui();
	}
}

void Player::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck("Player/initParameter.json", data)) {
		return;
	}

	stats_.maxHP = JsonAdapter::GetValue<int>(data, "maxHP");
	stats_.maxSkilPoint = JsonAdapter::GetValue<int>(data, "maxSkilPoint");
	// 初期化時は最大と同じ値にする
	stats_.currentHP = stats_.maxHP;
}

void Player::SaveJson() {

	Json data;

	data["maxHP"] = stats_.maxHP;
	data["maxSkilPoint"] = stats_.maxSkilPoint;

	JsonAdapter::Save("Player/initParameter.json", data);
}