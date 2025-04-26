#include "BodyDashBehavior.h"

//============================================================================
//	include
//============================================================================
#include <Game/Camera/FollowCamera.h>
#include <Game/Object3D/Player/Parts/Base/BasePlayerParts.h>

//============================================================================
//	BodyDashBehavior classMethods
//============================================================================

BodyDashBehavior::BodyDashBehavior(const std::optional<Json>& data, FollowCamera* followCamera) {

	followCamera_ = nullptr;
	followCamera_ = followCamera;

	speedLerpValue_ = std::make_unique<SimpleAnimation<float>>();

	if (data.has_value()) {

		const Json& jsonData = data.value();
		const Json& animationData = jsonData.contains("DashSpeedLerpValue") ? jsonData["DashSpeedLerpValue"] : Json();
		speedLerpValue_->FromJson(animationData);
	}
}

void BodyDashBehavior::Execute(BasePlayerParts* parts) {

	// 補間処理はダッシュ中のみ
	if (!speedLerpValue_->IsStart()) {

		// 補間処理を開始
		speedLerpValue_->time_.elapsed = 0.0f;
		speed_ = speedLerpValue_->move_.start;
		speedLerpValue_->Start();
	}
	// 値を補間
	speedLerpValue_->LerpValue(speed_);

	Vector2 inputValue{};
	// inputの値を取得
	IPlayerBehavior::InputKey(inputValue);
	if (std::fabs(inputValue.x) > FLT_EPSILON || std::fabs(inputValue.y) > FLT_EPSILON) {

		// 入力がある場合のみ速度を計算する
		Vector3 inputDirection(inputValue.x, 0.0f, inputValue.y);
		inputDirection = Vector3::Normalize(inputDirection);

		Matrix4x4 rotateMatrix = Quaternion::MakeRotateMatrix(followCamera_->GetTransform().rotation);
		Vector3 rotatedDirection = Vector3::TransferNormal(inputDirection, rotateMatrix);
		rotatedDirection = Vector3::Normalize(rotatedDirection);

		move_ = rotatedDirection * speed_;
	}

	// 移動量を加算
	Vector3 translation = parts->GetTransform().translation;
	translation.x += move_.x;
	translation.z += move_.z;
	parts->SetTranslate(translation);
}

void BodyDashBehavior::Reset() {

	// 初期化する
	speedLerpValue_->Reset();
}

void BodyDashBehavior::ImGui() {

	speedLerpValue_->ImGui("PlayerBodyDashBehavior_speedLerpValue_");
}

void BodyDashBehavior::SaveJson(Json& data) {

	// 値の保存
	speedLerpValue_->ToJson(data["DashSpeedLerpValue"]);
}