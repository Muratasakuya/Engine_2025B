#include "Player.h"

//============================================================================
//	include
//============================================================================

//============================================================================
//	Player classMethods
//============================================================================

void Player::InitHUD() {

	// HUDの初期化
	hudSprites_ = std::make_unique<PlayerHUD>();
	hudSprites_->Init();
}

void Player::DerivedInit() {

	// HUD初期化
	InitHUD();

	// json適応
	ApplyJson();
}

void Player::Update() {

	// HUDの更新
	hudSprites_->Update();
}

void Player::DerivedImGui() {

	if (ImGui::CollapsingHeader("HUD")) {

		hudSprites_->ImGui();
	}
}

void Player::ApplyJson() {


}

void Player::SaveJson() {


}