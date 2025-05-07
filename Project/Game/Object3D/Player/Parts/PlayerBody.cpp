#include "PlayerBody.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Input/Input.h>
#include <Game/Camera/FollowCamera.h>
#include <Lib/Adapter/JsonAdapter.h>

// behaviors
#include <Game/Object3D/Player/Behavior/Parts/Body/BodyWaitBehavior.h>
#include <Game/Object3D/Player/Behavior/Parts/Body/BodyWalkBehavior.h>
#include <Game/Object3D/Player/Behavior/Parts/Body/BodyDashBehavior.h>
#include <Game/Object3D/Player/Behavior/Parts/Body/BodyFirstAttackBehavior.h>
#include <Game/Object3D/Player/Behavior/Parts/Body/BodySecondAttackBehavior.h>
#include <Game/Object3D/Player/Behavior/Parts/Body/BodyThirdAttackBehavior.h>

//============================================================================
//	PlayerBody classMethods
//============================================================================

void PlayerBody::InitBehaviors(const Json& data) {

	// wait
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Wait,
		std::make_unique<BodyWaitBehavior>(data));
	// walk
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Walk,
		std::make_unique<BodyWalkBehavior>(data, followCamera_));
	// dash
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Dash,
		std::make_unique<BodyDashBehavior>(data, followCamera_,
			GetBehavior<BodyWalkBehavior>(PlayerBehaviorType::Walk)->GetSpeed()));
	// attack_1st
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Attack_1st,
		std::make_unique<BodyFirstAttackBehavior>(data));
	// attack_2nd
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Attack_2nd,
		std::make_unique<BodySecondAttackBehavior>(data));
	// attack_3rd
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Attack_3rd,
		std::make_unique<BodyThirdAttackBehavior>(data, followCamera_));
}

void PlayerBody::Init(FollowCamera* followCamera) {

	followCamera_ = nullptr;
	followCamera_ = followCamera;

	BasePlayerParts::Init("playerBody");

	// json適応
	ApplyJson();
}

void PlayerBody::ImGui() {

	ImGui::PushItemWidth(parameter_.itemWidth);

	parameter_.ImGui();

	if (ImGui::CollapsingHeader("Material")) {

		BasePlayerParts::ImGuiMaterial();
	}

	if (ImGui::CollapsingHeader("Attack_3rd_EditCatmullRom")) {

		BasePlayerParts::GetBehavior<BodyThirdAttackBehavior>
			(PlayerBehaviorType::Attack_3rd)->EditCatmullRom(
				transform_->translation, transform_->GetForward());
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

void PlayerBody::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck(baseBehaviorJsonFilePath_ + "PlayerBody.json", data)) {
		return;
	}

	BasePlayerParts::ApplyJson();

	// behaviors初期化
	InitBehaviors(data);
}

void PlayerBody::SaveJson() {

	parameter_.SaveJson();

	BasePlayerParts::SaveJson();

	Json data;
	for (const auto& behaviors : std::views::values(behaviors_)) {

		behaviors->SaveJson(data);
	}
	JsonAdapter::Save(baseBehaviorJsonFilePath_ + "PlayerBody.json", data);
}

void PlayerBody::SetBackwardDirection() {

	Vector3 direction = transform_->GetBack();
	BasePlayerParts::GetBehavior<BodyThirdAttackBehavior>
		(PlayerBehaviorType::Attack_3rd)->SetBackwardDirection(direction);
}