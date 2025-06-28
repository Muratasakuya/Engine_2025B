#include "PlayerSpecialAttackState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/GameTimer.h>
#include <Game/Objects/Player/Entity/Player.h>

//============================================================================
//	PlayerSpecialAttackState classMethods
//============================================================================

void PlayerSpecialAttackState::Enter(Player& player) {

	player.SetNextAnimation("player_skilAttack", false, nextAnimDuration_);
	animationFinished_ = false;
}

void PlayerSpecialAttackState::Update(Player& player) {

	// animationが終わったかチェック
	animationFinished_ = player.IsAnimationFinished();
}

void PlayerSpecialAttackState::Exit([[maybe_unused]] Player& player) {
}

void PlayerSpecialAttackState::ImGui() {

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.001f);
}

void PlayerSpecialAttackState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
}

void PlayerSpecialAttackState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
}