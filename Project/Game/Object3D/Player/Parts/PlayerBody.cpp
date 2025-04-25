#include "PlayerBody.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Input/Input.h>
#include <Game/Camera/FollowCamera.h>
#include <Lib/Adapter/JsonAdapter.h>

//============================================================================
//	PlayerBody classMethods
//============================================================================

void PlayerBody::InitParam() {

	// parameter初期化
	dashLerpValue_ = std::make_unique<SimpleAnimation<float>>();
}

void PlayerBody::Init() {

	BasePlayerParts::Init("playerBody");

	// parameter初期化
	InitParam();

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

void PlayerBody::UpdateDash() {

	// 補間処理はダッシュ中のみ
	if (!dashLerpValue_->IsStart()) {

		// 補間処理を開始
		dashLerpValue_->time_.elapsed = 0.0f;
		dashSpeed_ = dashLerpValue_->move_.start;
		dashLerpValue_->Start();
	}
	// 値を補完
	dashLerpValue_->LerpValue(dashSpeed_);

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

		move_ = rotatedDirection * moveVelocity_ * dashSpeed_;
	} else {

		// 入力がなくてもダッシュ速度を維持する
		move_ *= 1.0f;
	}

	// 移動量を加算
	transform_->translation.x += move_.x;
	transform_->translation.z += move_.z;
}

void PlayerBody::ImGui() {

	ImGui::PushItemWidth(parameter_.itemWidth);

	if (ImGui::CollapsingHeader("Walk")) {

		ImGui::DragFloat3("moveVelocity##Walk", &moveVelocity_.x, 0.1f);
		ImGui::DragFloat("moveDecay##Walk", &moveDecay_, 0.1f, 0.0f, 1.0f);
		ImGui::DragFloat("rotationLerpRate##Walk", &rotationLerpRate_, 0.1f);
	}

	if (ImGui::CollapsingHeader("Dash")) {

		dashLerpValue_->ImGui("dashLerpValue");
	}

	ImGui::PopItemWidth();
}

void PlayerBody::RotateToDirection() {

	Vector3 direction = Vector3(move_.x, 0.0f, move_.z).Normalize();

	if (direction.Length() <= 0.0f) {
		return;
	}

	Quaternion targetRotation = Quaternion::LookRotation(direction, Vector3(0.0f, 1.0f, 0.0f));
	transform_->rotation = Quaternion::Slerp(transform_->rotation, targetRotation, rotationLerpRate_);
}

void PlayerBody::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck(parameter_.baseFilePath + "PlayerBody.json", data)) {
		return;
	}

	moveVelocity_ = JsonAdapter::ToObject<Vector3>(data["moveVelocity_"]);
	moveDecay_ = JsonAdapter::GetValue<float>(data, "moveDecay_");
	rotationLerpRate_ = JsonAdapter::GetValue<float>(data, "rotationLerpRate_");

	dashLerpValue_->FromJson(data["dashLerpValue"]);
}

void PlayerBody::SaveJson() {

	Json data;

	data["moveVelocity_"] = JsonAdapter::FromObject<Vector3>(moveVelocity_);
	data["moveDecay_"] = moveDecay_;
	data["rotationLerpRate_"] = rotationLerpRate_;
	dashLerpValue_->ToJson(data["dashLerpValue"]);

	JsonAdapter::Save(parameter_.baseFilePath + "PlayerBody.json", data);
}

void PlayerBody::InputKey(Vector2& inputValue) {

	if (input_->PushKey(DIK_W)) {

		inputValue.y += 1.0f;
	} else if (input_->PushKey(DIK_S)) {

		inputValue.y -= 1.0f;
	}
	if (input_->PushKey(DIK_D)) {

		inputValue.x += 1.0f;
	} else if (input_->PushKey(DIK_A)) {

		inputValue.x -= 1.0f;
	}
}