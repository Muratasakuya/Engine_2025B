#include "PlayerDashState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/GameTimer.h>
#include <Game/Objects/Player/Entity/Player.h>

//============================================================================
//	PlayerDashState classMethods
//============================================================================

void PlayerDashState::Enter(Player& player) {

	player.SetNextAnimation("player_dash", true, nextAnimDuration_);
}

void PlayerDashState::Update([[maybe_unused]] Player& player) {
}

void PlayerDashState::Exit([[maybe_unused]] Player& player) {
}

void PlayerDashState::ImGui() {

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.001f);
}

void PlayerDashState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
}

void PlayerDashState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
}