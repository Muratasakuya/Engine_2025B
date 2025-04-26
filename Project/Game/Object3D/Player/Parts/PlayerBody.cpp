#include "PlayerBody.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Input/Input.h>
#include <Game/Camera/FollowCamera.h>
#include <Lib/Adapter/JsonAdapter.h>

// behaviors
#include <Game/Object3D/Player/Behavior/Parts/Body/BodyWaitBehavior.h>
#include <Game/Object3D/Player/Behavior/Parts/Body/BodyDashBehavior.h>

//============================================================================
//	PlayerBody classMethods
//============================================================================

void PlayerBody::InitBehaviors() {

	// wait
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Wait,
		std::make_unique<BodyWaitBehavior>(std::nullopt));
	// dash
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Dash,
		std::make_unique<BodyDashBehavior>(std::nullopt, followCamera_));
}

void PlayerBody::InitBehaviors(const Json& data) {

	// wait
	Json behaviorData = data.contains("PlayerBodyBehavior") ? data["PlayerBodyBehavior"] : Json();
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Wait,
		std::make_unique<BodyWaitBehavior>(behaviorData));
	// dash
	behaviorData = data.contains("PlayerBodyBehavior") ? data["PlayerBodyBehavior"] : Json();
	BasePlayerParts::RegisterBehavior(PlayerBehaviorType::Dash,
		std::make_unique<BodyDashBehavior>(behaviorData, followCamera_));
}

void PlayerBody::Init(FollowCamera* followCamera) {

	followCamera_ = nullptr;
	followCamera_ = followCamera;

	BasePlayerParts::Init("playerBody");

	// json適応
	ApplyJson();
}

void PlayerBody::UpdateWalk() {

	Vector2 inputValue{};
	// inputの値を取得
	InputKey(inputValue);

	if (std::fabs(inputValue.x) > FLT_EPSILON || std::fabs(inputValue.y) > FLT_EPSILON) {

		// 入力がある場合のみ速度を計算する
		Vector3 inputDirection(inputValue.x, 0.0f, inputValue.y);
		inputDirection = Vector3::Normalize(inputDirection);

		Matrix4x4 rotateMatrix = Quaternion::MakeRotateMatrix(followCamera_->GetTransform().rotation);
		Vector3 rotatedDirection = Vector3::TransferNormal(inputDirection, rotateMatrix);
		rotatedDirection = Vector3::Normalize(rotatedDirection);

		move_ = rotatedDirection * moveVelocity_;
	} else {

		// 入力がなければどんどん減速させる
		move_ *= moveDecay_;
		// 一定の速度以下で止まる
		if (move_.Length() < 0.001f) {
			move_.Init();
		}
	}

	// 移動量を加算
	transform_->translation.x += move_.x;
	transform_->translation.z += move_.z;
}

void PlayerBody::RotateToDirection() {

	Vector3 direction = Vector3(move_.x, 0.0f, move_.z).Normalize();

	if (direction.Length() <= 0.0f) {
		return;
	}

	Quaternion targetRotation = Quaternion::LookRotation(direction, Vector3(0.0f, 1.0f, 0.0f));
	transform_->rotation = Quaternion::Slerp(transform_->rotation, targetRotation, rotationLerpRate_);
}

void PlayerBody::ImGui() {

	ImGui::PushItemWidth(parameter_.itemWidth);

	if (ImGui::CollapsingHeader("Walk")) {

		ImGui::DragFloat3("moveVelocity##Walk", &moveVelocity_.x, 0.1f);
		ImGui::DragFloat("moveDecay##Walk", &moveDecay_, 0.1f, 0.0f, 1.0f);
		ImGui::DragFloat("rotationLerpRate##Walk", &rotationLerpRate_, 0.1f);
	}

	if (ImGui::CollapsingHeader("WaitBehavior")) {

		behaviors_[PlayerBehaviorType::Wait]->ImGui();
	}

	if (ImGui::CollapsingHeader("DashBehavior")) {

		behaviors_[PlayerBehaviorType::Dash]->ImGui();
	}

	ImGui::PopItemWidth();
}

void PlayerBody::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck(parameter_.baseFilePath + "PlayerBody.json", data)) {

		// behaviors初期化
		InitBehaviors();
		return;
	}

	moveVelocity_ = JsonAdapter::ToObject<Vector3>(data["moveVelocity_"]);
	moveDecay_ = JsonAdapter::GetValue<float>(data, "moveDecay_");
	rotationLerpRate_ = JsonAdapter::GetValue<float>(data, "rotationLerpRate_");

	// behaviors初期化
	InitBehaviors(data);
}

void PlayerBody::SaveJson() {

	Json data;

	data["moveVelocity_"] = JsonAdapter::FromObject<Vector3>(moveVelocity_);
	data["moveDecay_"] = moveDecay_;
	data["rotationLerpRate_"] = rotationLerpRate_;

	for (const auto& behaviors : std::views::values(behaviors_)) {

		behaviors->SaveJson(data["PlayerBodyBehavior"]);
	}

	JsonAdapter::Save(parameter_.baseFilePath + "PlayerBody.json", data);
}