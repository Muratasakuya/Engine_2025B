#include "PlayerRightHand.h"

//============================================================================
//	include
//============================================================================

// behaviors
#include <Game/Object3D/Player/Behavior/Parts/RightHand/RightHandDashBehavior.h>

//============================================================================
//	PlayerRightHand classMethods
//============================================================================

void PlayerRightHand::InitBehaviors() {

	// dash
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Dash,
		std::make_unique<RightHandDashBehavior>(std::nullopt));
}

void PlayerRightHand::InitBehaviors(const Json& data) {

	// dash
	const Json& behaviorData = data.contains("PlayerRightHandBehavior") ? data["PlayerRightHandBehavior"] : Json();
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Dash,
		std::make_unique<RightHandDashBehavior>(behaviorData));
}

void PlayerRightHand::Init() {

	BasePlayerParts::Init("playerRightHand");

	// json適応
	ApplyJson();
}

void PlayerRightHand::ImGui() {

	ImGui::PushItemWidth(parameter_.itemWidth);

	if (ImGui::CollapsingHeader("DashBehavior")) {

		behaviors_[PlayerBehaviorType::Dash]->ImGui();
	}

	ImGui::PopItemWidth();
}

void PlayerRightHand::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck(parameter_.baseFilePath + "PlayerRightHand.json", data)) {

		// behaviors初期化
		InitBehaviors();
		return;
	}

	// behaviors初期化
	InitBehaviors(data);
}

void PlayerRightHand::SaveJson() {

	Json data;

	for (const auto& behaviors : std::views::values(behaviors_)) {

		behaviors->SaveJson(data["PlayerRightHandBehavior"]);
	}

	JsonAdapter::Save(parameter_.baseFilePath + "PlayerRightHand.json", data);
}