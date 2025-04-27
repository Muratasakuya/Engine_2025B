#include "PlayerLeftHand.h"

//============================================================================
//	include
//============================================================================

// behaviors
#include <Game/Object3D/Player/Behavior/Parts/LeftHand/LeftHandDashBehavior.h>

//============================================================================
//	PlayerLeftHand classMethods
//============================================================================

void PlayerLeftHand::InitBehaviors(const Json& data) {

	// dash
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Dash,
		std::make_unique<LeftHandDashBehavior>(data));
}

void PlayerLeftHand::Init() {

	BasePlayerParts::Init("playerLeftHand");

	// json適応
	ApplyJson();
}

void PlayerLeftHand::ImGui() {

	ImGui::PushItemWidth(parameter_.itemWidth);

	parameter_.ImGui();

	if (ImGui::CollapsingHeader("DashBehavior")) {

		behaviors_[PlayerBehaviorType::Dash]->ImGui();
	}

	ImGui::PopItemWidth();
}

void PlayerLeftHand::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck(baseBehaviorJsonFilePath_ + "playerLeftHand.json", data)) {
		return;
	}

	// behaviors初期化
	InitBehaviors(data);
}

void PlayerLeftHand::SaveJson() {

	parameter_.SaveJson();

	Json data;

	for (const auto& behaviors : std::views::values(behaviors_)) {

		behaviors->SaveJson(data);
	}
	JsonAdapter::Save(baseBehaviorJsonFilePath_ + "playerLeftHand.json", data);
}