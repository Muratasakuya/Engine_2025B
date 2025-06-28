#include "PlayerAttack_1stState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/GameTimer.h>
#include <Game/Objects/Player/Entity/Player.h>

//============================================================================
//	PlayerAttack_1stState classMethods
//============================================================================

void PlayerAttack_1stState::Enter(Player& player) {

	player.SetNextAnimation("player_attack_1st", false, nextAnimDuration_);
	animationFinished_ = false;
}

void PlayerAttack_1stState::Update(Player& player) {

	// animationが終わったかチェック
	animationFinished_ = player.IsAnimationFinished();
}

void PlayerAttack_1stState::Exit([[maybe_unused]] Player& player) {
}

void PlayerAttack_1stState::ImGui() {

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.001f);
}

void PlayerAttack_1stState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
}

void PlayerAttack_1stState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
}