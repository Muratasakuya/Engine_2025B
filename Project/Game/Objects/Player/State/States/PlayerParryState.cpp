#include "PlayerParryState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/GameTimer.h>
#include <Game/Objects/Player/Entity/Player.h>

//============================================================================
//	PlayerParryState classMethods
//============================================================================

void PlayerParryState::Enter(Player& player) {

	player.SetNextAnimation("player_idle", false, nextAnimDuration_);
}

void PlayerParryState::Update([[maybe_unused]] Player& player) {
}

void PlayerParryState::Exit([[maybe_unused]] Player& player) {
}

void PlayerParryState::ImGui([[maybe_unused]] const Player& player) {

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.001f);
}

void PlayerParryState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
}

void PlayerParryState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
}