#include "PlayerAttack_3rdState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/GameTimer.h>
#include <Game/Objects/Player/Entity/Player.h>

//============================================================================
//	PlayerAttack_3rdState classMethods
//============================================================================

void PlayerAttack_3rdState::Enter(Player& player) {

	player.SetNextAnimation("player_attack_3rd", true, nextAnimDuration_);
	animationFinished_ = false;
}

void PlayerAttack_3rdState::Update(Player& player) {

	// animationが終わったかチェック
	animationFinished_ = player.IsAnimationFinished();
}

void PlayerAttack_3rdState::Exit([[maybe_unused]] Player& player) {
}

void PlayerAttack_3rdState::ImGui() {

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.001f);
}

void PlayerAttack_3rdState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
}

void PlayerAttack_3rdState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
}