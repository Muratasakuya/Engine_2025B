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

	player.SetNextAnimation("player_attack_2nd", true, nextAnimDuration_);
	animationFinished_ = false;
}

void PlayerAttack_2ndState::Update(Player& player) {

	// animationが終わったかチェック
	animationFinished_ = player.IsAnimationFinished();
}

void PlayerAttack_2ndState::Exit([[maybe_unused]] Player& player) {
}

void PlayerAttack_2ndState::ImGui([[maybe_unused]] const Player& player) {

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.001f);
}

void PlayerAttack_2ndState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
}

void PlayerAttack_2ndState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
}