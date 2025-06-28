#include "PlayerAttack_2ndState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/GameTimer.h>
#include <Game/Objects/Player/Entity/Player.h>

//============================================================================
//	PlayerAttack_2ndState classMethods
//============================================================================

void PlayerAttack_2ndState::Enter(Player& player) {

	player.SetNextAnimation("player_attack_2nd", false, nextAnimDuration_);
	canExit_ = false;
}

void PlayerAttack_2ndState::Update(Player& player) {

	// animationが終わったかチェック
	canExit_ = player.IsAnimationFinished();

	// 座標、回転補間
	AttackAssist(player);
}

void PlayerAttack_2ndState::Exit([[maybe_unused]] Player& player) {

	// timerをリセット
	attackPosLerpTimer_ = 0.0f;
}

void PlayerAttack_2ndState::ImGui(const Player& player) {

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.001f);
	ImGui::DragFloat("rotationLerpRate", &rotationLerpRate_, 0.001f);
	ImGui::DragFloat("exitTime", &exitTime_, 0.01f);

	PlayerBaseAttackState::ImGui(player);
}

void PlayerAttack_2ndState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
	rotationLerpRate_ = JsonAdapter::GetValue<float>(data, "rotationLerpRate_");
	exitTime_ = JsonAdapter::GetValue<float>(data, "exitTime_");

	PlayerBaseAttackState::ApplyJson(data);
}

void PlayerAttack_2ndState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
	data["rotationLerpRate_"] = rotationLerpRate_;
	data["exitTime_"] = exitTime_;

	PlayerBaseAttackState::SaveJson(data);
}