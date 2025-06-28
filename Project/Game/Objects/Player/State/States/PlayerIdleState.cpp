#include "PlayerIdleState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/GameTimer.h>
#include <Game/Objects/Player/Entity/Player.h>

//============================================================================
//	PlayerIdleState classMethods
//============================================================================

void PlayerIdleState::Enter(Player& player) {

	player.SetNextAnimation("player_idle",true, nextAnimDuration_);
}

void PlayerIdleState::Update([[maybe_unused]] Player& player) {
}

void PlayerIdleState::Exit([[maybe_unused]] Player& player) {
}

void PlayerIdleState::ImGui([[maybe_unused]] const Player& player) {

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.001f);
}

void PlayerIdleState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
}

void PlayerIdleState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
}