#include "PlayerLeftHand.h"

//============================================================================
//	include
//============================================================================

// behaviors
#include <Game/Object3D/Player/Behavior/Parts/LeftHand/LeftHandWaitBehavior.h>
#include <Game/Object3D/Player/Behavior/Parts/LeftHand/LeftHandWalkBehavior.h>
#include <Game/Object3D/Player/Behavior/Parts/LeftHand/LeftHandDashBehavior.h>
#include <Game/Object3D/Player/Behavior/Parts/LeftHand/LeftHandFirstAttackBehavior.h>
#include <Game/Object3D/Player/Behavior/Parts/LeftHand/LeftHandSecondAttackBehavior.h>
#include <Game/Object3D/Player/Behavior/Parts/LeftHand/LeftHandThirdAttackBehavior.h>

//============================================================================
//	PlayerLeftHand classMethods
//============================================================================

void PlayerLeftHand::InitBehaviors(const Json& data) {

	// wait
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Wait,
		std::make_unique<LeftHandWaitBehavior>(data));
	// walk
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Walk,
		std::make_unique<LeftHandWalkBehavior>(data));
	// dash
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Dash,
		std::make_unique<LeftHandDashBehavior>(data));
	// attack_1st
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Attack_1st,
		std::make_unique<LeftHandFirstAttackBehavior>(data));
	// attack_2nd
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Attack_2nd,
		std::make_unique<LeftHandSecondAttackBehavior>(data));
	// attack_3rd
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Attack_3rd,
		std::make_unique<LeftHandThirdAttackBehavior>(data));
}

void PlayerLeftHand::Init() {

	BasePlayerParts::Init("playerLeftHand");

	// json適応
	ApplyJson();
}

void PlayerLeftHand::ImGui() {

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

void PlayerLeftHand::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck(baseBehaviorJsonFilePath_ + "playerLeftHand.json", data)) {
		return;
	}

	BasePlayerParts::ApplyJson();

	// behaviors初期化
	InitBehaviors(data);
}

void PlayerLeftHand::SaveJson() {

	parameter_.SaveJson();

	BasePlayerParts::SaveJson();

	Json data;

	for (const auto& behaviors : std::views::values(behaviors_)) {

		behaviors->SaveJson(data);
	}
	JsonAdapter::Save(baseBehaviorJsonFilePath_ + "playerLeftHand.json", data);
}