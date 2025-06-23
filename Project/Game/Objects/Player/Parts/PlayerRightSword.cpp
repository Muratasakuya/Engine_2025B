#include "PlayerRightSword.h"

//============================================================================
//	include
//============================================================================

// behaviors
#include <Game/Objects/Player/Behavior/Parts/RightSword/RightSwordWaitBehavior.h>
#include <Game/Objects/Player/Behavior/Parts/RightSword/RightSwordWalkBehavior.h>
#include <Game/Objects/Player/Behavior/Parts/RightSword/RightSwordFirstAttackBehavior.h>
#include <Game/Objects/Player/Behavior/Parts/RightSword/RightSwordThirdAttackBehavior.h>

//============================================================================
//	PlayerRightSword classMethods
//============================================================================

void PlayerRightSword::InitBehaviors(const Json& data) {

	// wait
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Wait,
		std::make_unique<RightSwordWaitBehavior>(data, transform_->rotation));
	// walk
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Walk,
		std::make_unique<RightSwordWalkBehavior>(data, transform_->rotation));
	// attack_1st
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Attack_1st,
		std::make_unique<RightSwordFirstAttackBehavior>(data));
	// attack_3rd
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Attack_3rd,
		std::make_unique<RightSwordThirdAttackBehavior>(data, parameter_.offsetTranslation));
}

void PlayerRightSword::Init() {

	BasePlayerParts::Init("playerSword", "playerRightSword");

	// json適応
	ApplyJson();
}

void PlayerRightSword::ImGui() {

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
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Attack")) {

		if (ImGui::CollapsingHeader("Attack_1stBehavior")) {

			behaviors_[PlayerBehaviorType::Attack_1st]->ImGui();
		}
		if (ImGui::CollapsingHeader("Attack_3rdBehavior")) {

			behaviors_[PlayerBehaviorType::Attack_3rd]->ImGui();
		}
		ImGui::TreePop();
	}

	ImGui::PopItemWidth();
}

void PlayerRightSword::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck(baseBehaviorJsonFilePath_ + "playerRightSword.json", data)) {
		return;
	}

	BasePlayerParts::ApplyJson();

	// behaviors初期化
	InitBehaviors(data);
}

void PlayerRightSword::SaveJson() {

	parameter_.SaveJson();

	BasePlayerParts::SaveJson();

	Json data;

	for (const auto& behaviors : std::views::values(behaviors_)) {

		behaviors->SaveJson(data);
	}
	JsonAdapter::Save(baseBehaviorJsonFilePath_ + "playerRightSword.json", data);
}

void PlayerRightSword::SetForwardDirection(const Vector3& direction) {

	BasePlayerParts::GetBehavior<RightSwordThirdAttackBehavior>
		(PlayerBehaviorType::Attack_3rd)->SetForwardDirection(direction);
}