#include "PlayerLeftHand.h"

//============================================================================
//	include
//============================================================================

// behaviors
#include <Game/Object3D/Player/Behavior/Parts/LeftHand/LeftHandDashBehavior.h>

//============================================================================
//	PlayerLeftHand classMethods
//============================================================================

void PlayerLeftHand::InitBehaviors() {

	// dash
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Dash,
		std::make_unique<LeftHandDashBehavior>(std::nullopt));
}

void PlayerLeftHand::InitBehaviors(const Json& data) {

	// dash
	const Json& behaviorData = data.contains("PlayerLeftHandBehavior") ? data["PlayerLeftHandBehavior"] : Json();
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Dash,
		std::make_unique<LeftHandDashBehavior>(behaviorData));
}

void PlayerLeftHand::Init() {

	BasePlayerParts::Init("playerLeftHand");

	// json適応
	ApplyJson();
}

void PlayerLeftHand::ImGui() {

	ImGui::PushItemWidth(parameter_.itemWidth);

	if (ImGui::CollapsingHeader("DashBehavior")) {

		behaviors_[PlayerBehaviorType::Dash]->ImGui();
	}

	ImGui::PopItemWidth();
}

void PlayerLeftHand::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck(parameter_.baseFilePath + "PlayerLeftHand.json", data)) {

		// behaviors初期化
		InitBehaviors();
		return;
	}

	// behaviors初期化
	InitBehaviors(data);
}

void PlayerLeftHand::SaveJson() {

	Json data;

	for (const auto& behaviors : std::views::values(behaviors_)) {

		behaviors->SaveJson(data["PlayerLeftHandBehavior"]);
	}

	JsonAdapter::Save(parameter_.baseFilePath + "PlayerLeftHand.json", data);
}