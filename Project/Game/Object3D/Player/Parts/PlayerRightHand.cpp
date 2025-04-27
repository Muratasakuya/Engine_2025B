#include "PlayerRightHand.h"

//============================================================================
//	include
//============================================================================

// behaviors
#include <Game/Object3D/Player/Behavior/Parts/RightHand/RightHandDashBehavior.h>

//============================================================================
//	PlayerRightHand classMethods
//============================================================================

void PlayerRightHand::InitBehaviors(const Json& data) {

	// dash
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Dash,
		std::make_unique<RightHandDashBehavior>(data));
}

void PlayerRightHand::Init() {

	BasePlayerParts::Init("playerRightHand");

	// json適応
	ApplyJson();
}

void PlayerRightHand::ImGui() {

	ImGui::PushItemWidth(parameter_.itemWidth);

	parameter_.ImGui();

	if (ImGui::CollapsingHeader("DashBehavior")) {

		behaviors_[PlayerBehaviorType::Dash]->ImGui();
	}

	ImGui::PopItemWidth();
}

void PlayerRightHand::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck(baseBehaviorJsonFilePath_ + "playerRightHand.json", data)) {
		return;
	}

	// behaviors初期化
	InitBehaviors(data);
}

void PlayerRightHand::SaveJson() {

	parameter_.SaveJson();

	Json data;

	for (const auto& behaviors : std::views::values(behaviors_)) {

		behaviors->SaveJson(data);
	}
	JsonAdapter::Save(baseBehaviorJsonFilePath_ + "playerRightHand.json", data);
}