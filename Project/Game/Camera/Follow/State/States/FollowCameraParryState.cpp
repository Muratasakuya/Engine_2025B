#include "FollowCameraParryState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/GameTimer.h>
#include <Game/Camera/Follow/FollowCamera.h>
#include <Lib/Adapter/JsonAdapter.h>

//============================================================================
//	FollowCameraParryState classMethods
//============================================================================

void FollowCameraParryState::Enter(FollowCamera& followCamera) {

	// 補間開始値を設定
	startRotateX_ = followCamera.GetTransform().eulerRotate.x;

	lerpTimer_ = 0.0f;
	waitTimer_ = 0.0f;
	canExit_ = false;
}

void FollowCameraParryState::Update(FollowCamera& followCamera) {

	// 時間経過を進める
	lerpTimer_ += GameTimer::GetDeltaTime();
	float lerpT = lerpTimer_ / lerpTime_;
	lerpT = EasedValue(easingType_, lerpT);

	// 補間が終了したら待機状態に遷移させる
	if (1.0f <= lerpT) {

		// 時間経過を進める
		waitTimer_ += GameTimer::GetDeltaTime();

		// 時間経過が終了したら状態を終了させる
		if (waitTime_ < waitTimer_) {

			canExit_ = true;
		}
		lerpT = 1.0f;
	}

	// 距離
	Vector3 offsetTranslation = Vector3::Lerp(
		startOffsetTranslation_, targetOffsetTranslation_, lerpT);
	// X軸回転
	float rotationX = std::lerp(startRotateX_, targetRotateX_, lerpT);
	// 画角
	float fovY = std::lerp(startFovY_, targetFovY_, lerpT);

	// 補間値を設定
	// 画角
	followCamera.SetFovY(fovY);

	// 回転
	Vector3 rotation = followCamera.GetTransform().eulerRotate;
	rotation.x = rotationX;
	followCamera.SetEulerRotation(rotation);

	// 座標
	// 補間先の座標を補間割合に応じて補間する
	interTarget_ = Vector3::Lerp(interTarget_, targets_[FollowCameraTargetType::Player]->GetWorldPos(), lerpRate_);

	Matrix4x4 rotateMatrix = Matrix4x4::MakeRotateMatrix(rotation);
	Vector3 offset = Vector3::TransferNormal(offsetTranslation, rotateMatrix);

	// offset分座標をずらす
	Vector3 translation = interTarget_ + offset;
	followCamera.SetTranslation(translation);
}

void FollowCameraParryState::Exit() {

	// リセット
	lerpTimer_ = 0.0f;
	waitTimer_ = 0.0f;
	interTarget_.Init();
	canExit_ = false;
}

void FollowCameraParryState::ImGui([[maybe_unused]] const FollowCamera& followCamera) {

	ImGui::Text(std::format("canExit: {}", canExit_).c_str());
	ImGui::Text(std::format("lerpTimer: {}", lerpTimer_).c_str());
	ImGui::Text(std::format("waitTimer: {}", waitTimer_).c_str());

	ImGui::DragFloat3("targetOffsetTranslation", &targetOffsetTranslation_.x, 0.1f);

	ImGui::DragFloat("lerpTime", &lerpTime_, 0.01f);
	ImGui::DragFloat("waitTime", &waitTime_, 0.01f);
	ImGui::DragFloat("lerpRate##FollowCameraParryState", &lerpRate_, 0.1f);
	ImGui::DragFloat("targetRotateX", &targetRotateX_, 0.01f);
	ImGui::DragFloat("targetFovY", &targetFovY_, 0.01f);

	Easing::SelectEasingType(easingType_);
}

void FollowCameraParryState::ApplyJson(const Json& data) {

	targetOffsetTranslation_ = JsonAdapter::ToObject<Vector3>(data["targetOffsetTranslation_"]);
	lerpTime_ = JsonAdapter::GetValue<float>(data, "lerpTime_");
	waitTime_ = JsonAdapter::GetValue<float>(data, "waitTime_");
	lerpRate_ = JsonAdapter::GetValue<float>(data, "lerpRate_");
	targetRotateX_ = JsonAdapter::GetValue<float>(data, "targetRotateX_");
	targetFovY_ = JsonAdapter::GetValue<float>(data, "targetFovY_");
	easingType_ = JsonAdapter::GetValue<EasingType>(data, "easingType_");
}

void FollowCameraParryState::SaveJson(Json& data) {

	data["targetOffsetTranslation_"] = JsonAdapter::FromObject<Vector3>(targetOffsetTranslation_);
	data["lerpTime_"] = lerpTime_;
	data["waitTime_"] = waitTime_;
	data["lerpRate_"] = lerpRate_;
	data["targetRotateX_"] = targetRotateX_;
	data["targetFovY_"] = targetFovY_;
	data["easingType_"] = static_cast<int>(easingType_);
}

void FollowCameraParryState::SetStartOffsetTranslation(const Vector3& startOffsetTranslation) {

	startOffsetTranslation_ = startOffsetTranslation;
}