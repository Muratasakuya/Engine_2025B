#include "PlayerLeftSword.h"

//============================================================================
//	include
//============================================================================

// behaviors
#include <Game/Object3D/Player/Behavior/Parts/LeftSword/LeftSwordWaitBehavior.h>
#include <Game/Object3D/Player/Behavior/Parts/LeftSword/LeftSwordWalkBehavior.h>
#include <Game/Object3D/Player/Behavior/Parts/LeftSword/LeftSwordFirstAttackBehavior.h>
#include <Game/Object3D/Player/Behavior/Parts/LeftSword/LeftSwordThirdAttackBehavior.h>

//============================================================================
//	PlayerLeftSword classMethods
//============================================================================

void PlayerLeftSword::InitBehaviors(const Json& data) {

	// wait
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Wait,
		std::make_unique<LeftSwordWaitBehavior>(data, transform_->rotation));
	// walk
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Walk,
		std::make_unique<LeftSwordWalkBehavior>(data, transform_->rotation));
	// attack_1st
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Attack_1st,
		std::make_unique<LeftSwordFirstAttackBehavior>(data));
	// attack_3rd
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Attack_3rd,
		std::make_unique<LeftSwordThirdAttackBehavior>(data, parameter_.offsetTranslation));
}

void PlayerLeftSword::Init() {

	BasePlayerParts::Init("playerSword", "playerLeftSword");

	// json適応
	ApplyJson();
}

void PlayerLeftSword::ImGui() {

	ImGui::PushItemWidth(parameter_.itemWidth);

	parameter_.ImGui();

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

void PlayerLeftSword::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck(baseBehaviorJsonFilePath_ + "playerLeftSword.json", data)) {
		return;
	}

	// behaviors初期化
	InitBehaviors(data);
}

void PlayerLeftSword::SaveJson() {

	parameter_.SaveJson();

	Json data;

	for (const auto& behaviors : std::views::values(behaviors_)) {

		behaviors->SaveJson(data);
	}
	JsonAdapter::Save(baseBehaviorJsonFilePath_ + "playerLeftSword.json", data);
}

void PlayerLeftSword::SetForwardDirection(const Vector3& direction) {

	BasePlayerParts::GetBehavior<LeftSwordThirdAttackBehavior>
		(PlayerBehaviorType::Attack_3rd)->SetForwardDirection(direction);
}