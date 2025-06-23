#include "PlayerRightHand.h"

//============================================================================
//	include
//============================================================================

// behaviors
#include <Game/Objects/Player/Behavior/Parts/RightHand/RightHandWaitBehavior.h>
#include <Game/Objects/Player/Behavior/Parts/RightHand/RightHandWalkBehavior.h>
#include <Game/Objects/Player/Behavior/Parts/RightHand/RightHandDashBehavior.h>
#include <Game/Objects/Player/Behavior/Parts/RightHand/RightHandFirstAttackBehavior.h>
#include <Game/Objects/Player/Behavior/Parts/RightHand/RightHandSecondAttackBehavior.h>
#include <Game/Objects/Player/Behavior/Parts/RightHand/RightHandThirdAttackBehavior.h>

//============================================================================
//	PlayerRightHand classMethods
//============================================================================

void PlayerRightHand::InitBehaviors(const Json& data) {

	// wait
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Wait,
		std::make_unique<RightHandWaitBehavior>(data));
	// walk
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Walk,
		std::make_unique<RightHandWalkBehavior>(data));
	// dash
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Dash,
		std::make_unique<RightHandDashBehavior>(data));
	// attack_1st
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Attack_1st,
		std::make_unique<RightHandFirstAttackBehavior>(data));
	// attack_2nd
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Attack_2nd,
		std::make_unique<RightHandSecondAttackBehavior>(data));
	// attack_3rd
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Attack_3rd,
		std::make_unique<RightHandThirdAttackBehavior>(data));
}

void PlayerRightHand::Init() {

	BasePlayerParts::Init("playerRightHand");

	// json適応
	ApplyJson();
}

void PlayerRightHand::ImGui() {

	ImGui::PushItemWidth(parameter_.itemWidth);

	parameter_.ImGui();

	if (ImGui::CollapsingHeader("Material")) {

		BasePlayerParts::ImGuiMaterial();
	}

	if (ImGui::TreeNode("Wait_Move")) {

		if (ImGui::CollapsingHeader("WaitBehavior")) {

			behaviors_[PlayerBehaviorType::Wait]->ImGui();
		}

		if (ImGui::CollapsingHeader("WalkBehavior")) {

			behaviors_[PlayerBehaviorType::Walk]->ImGui();
		}

		if (ImGui::CollapsingHeader("DashBehavior")) {

			behaviors_[PlayerBehaviorType::Dash]->ImGui();
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Attack")) {

		if (ImGui::CollapsingHeader("Attack_1stBehavior")) {

			behaviors_[PlayerBehaviorType::Attack_1st]->ImGui();
		}

		if (ImGui::CollapsingHeader("Attack_2ndBehavior")) {

			behaviors_[PlayerBehaviorType::Attack_2nd]->ImGui();
		}

		if (ImGui::CollapsingHeader("Attack_3rdBehavior")) {

			behaviors_[PlayerBehaviorType::Attack_3rd]->ImGui();
		}
		ImGui::TreePop();
	}

	ImGui::PopItemWidth();
}

void PlayerRightHand::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck(baseBehaviorJsonFilePath_ + "playerRightHand.json", data)) {
		return;
	}

	BasePlayerParts::ApplyJson();

	// behaviors初期化
	InitBehaviors(data);
}

void PlayerRightHand::SaveJson() {

	parameter_.SaveJson();

	BasePlayerParts::SaveJson();

	Json data;

	for (const auto& behaviors : std::views::values(behaviors_)) {

		behaviors->SaveJson(data);
	}
	JsonAdapter::Save(baseBehaviorJsonFilePath_ + "playerRightHand.json", data);
}