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

	player.SetNextAnimation("player_idle",true, nextAnimDuration_);
}

void PlayerSkilAttackState::Update([[maybe_unused]] Player& player) {
}

void PlayerSkilAttackState::Exit([[maybe_unused]] Player& player) {
}

void PlayerSkilAttackState::ImGui() {

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.001f);
}

void PlayerSkilAttackState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
}

void PlayerSkilAttackState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
}