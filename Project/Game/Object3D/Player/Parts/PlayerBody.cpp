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

	if (ImGui::CollapsingHeader("WaitBehavior")) {

		behaviors_[PlayerBehaviorType::Wait]->ImGui();
	}

	if (ImGui::CollapsingHeader("Walk")) {

		behaviors_[PlayerBehaviorType::Walk]->ImGui();
	}

	if (ImGui::CollapsingHeader("DashBehavior")) {

		behaviors_[PlayerBehaviorType::Dash]->ImGui();
	}

	if (ImGui::CollapsingHeader("Attack_1stBehavior")) {

		behaviors_[PlayerBehaviorType::Attack_1st]->ImGui();
	}

	ImGui::PopItemWidth();
}

void PlayerBody::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck(baseBehaviorJsonFilePath_ + "PlayerBody.json", data)) {
		return;
	}

	// behaviors初期化
	InitBehaviors(data);
}

void PlayerBody::SaveJson() {

	parameter_.SaveJson();

	Json data;
	for (const auto& behaviors : std::views::values(behaviors_)) {

		behaviors->SaveJson(data);
	}
	JsonAdapter::Save(baseBehaviorJsonFilePath_ + "PlayerBody.json", data);
}