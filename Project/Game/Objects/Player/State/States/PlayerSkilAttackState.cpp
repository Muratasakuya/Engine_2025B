#include "PlayerSkilAttackState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/GameTimer.h>
#include <Game/Objects/Player/Entity/Player.h>

//============================================================================
//	PlayerSkilAttackState classMethods
//============================================================================

void PlayerSkilAttackState::Enter(Player& player) {

	player.SetNextAnimation("player_skilAttack", false, nextAnimDuration_);
	animationFinished_ = false;
}

void PlayerSkilAttackState::Update(Player& player) {

	// animationが終わったかチェック
	animationFinished_ = player.IsAnimationFinished();
}

void PlayerSkilAttackState::Exit([[maybe_unused]] Player& player) {
}

void PlayerSkilAttackState::ImGui([[maybe_unused]] const Player& player) {

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.001f);
}

void PlayerSkilAttackState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
}

void PlayerSkilAttackState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
}