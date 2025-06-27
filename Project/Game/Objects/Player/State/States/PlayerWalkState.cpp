#include "PlayerWalkState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/GameTimer.h>
#include <Game/Objects/Player/Entity/Player.h>

//============================================================================
//	PlayerWalkState classMethods
//============================================================================

void PlayerWalkState::Enter(Player& player) {

	player.SetNextAnimation("player_walk", true, nextAnimDuration_);
}

void PlayerWalkState::Update([[maybe_unused]] Player& player) {
}

void PlayerWalkState::Exit([[maybe_unused]] Player& player) {
}

void PlayerWalkState::ImGui() {

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.001f);
}

void PlayerWalkState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
}

void PlayerWalkState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
}