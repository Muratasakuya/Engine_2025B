#include "PlayerRightSword.h"

//============================================================================
//	include
//============================================================================

// behaviors
#include <Game/Object3D/Player/Behavior/Parts/RightSword/RightSwordWaitBehavior.h>
#include <Game/Object3D/Player/Behavior/Parts/RightSword/RightSwordWalkBehavior.h>
#include <Game/Object3D/Player/Behavior/Parts/RightSword/RightSwordFirstAttackBehavior.h>

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
}

void PlayerRightSword::Init() {

	BasePlayerParts::Init("playerSword", "playerRightSword");

	// json適応
	ApplyJson();
}

void PlayerRightSword::ImGui() {

	ImGui::PushItemWidth(parameter_.itemWidth);

	parameter_.ImGui();

	if (ImGui::CollapsingHeader("WaitBehavior")) {

		behaviors_[PlayerBehaviorType::Wait]->ImGui();
	}

	if (ImGui::CollapsingHeader("WalkBehavior")) {

		behaviors_[PlayerBehaviorType::Walk]->ImGui();
	}

	if (ImGui::CollapsingHeader("Attack_1stBehavior")) {

		behaviors_[PlayerBehaviorType::Attack_1st]->ImGui();
	}

	ImGui::PopItemWidth();
}

void PlayerRightSword::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck(baseBehaviorJsonFilePath_ + "playerRightSword.json", data)) {
		return;
	}

	// behaviors初期化
	InitBehaviors(data);
}

void PlayerRightSword::SaveJson() {

	parameter_.SaveJson();

	Json data;

	for (const auto& behaviors : std::views::values(behaviors_)) {

		behaviors->SaveJson(data);
	}
	JsonAdapter::Save(baseBehaviorJsonFilePath_ + "playerRightSword.json", data);
}