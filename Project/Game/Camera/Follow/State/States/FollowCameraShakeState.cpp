#include "FollowCameraShakeState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/GameTimer.h>
#include <Game/Camera/Follow/FollowCamera.h>
#include <Lib/Adapter/RandomGenerator.h>
#include <Lib/Adapter/JsonAdapter.h>

//============================================================================
//	FollowCameraShakeState classMethods
//============================================================================

FollowCameraShakeState::FollowCameraShakeState() {

	canExit_ = false;
}

void FollowCameraShakeState::Enter() {
}

void FollowCameraShakeState::Update(FollowCamera& followCamera) {

	shakeTimer_ += GameTimer::GetScaledDeltaTime();
	const Transform3DComponent& transform = followCamera.GetTransform();

	// シェイクの残り時間を計算
	float remainingTime = shakeTime_ / shakeTimer_;
	if (remainingTime > 0.0f) {

		// 時間経過で減衰させる
		float lerpT = 1.0f - remainingTime / shakeTime_;
		lerpT = EasedValue(shakeEasingType_, lerpT);
		float intensity = std::lerp(shakeXZIntensity_, 0.0f, lerpT);
		float offsetYIntensity = std::lerp(shakeOffsetYIntensity_, 0.0f, lerpT);

		float offsetX = RandomGenerator::Generate(-1.0f, 1.0f) * intensity;
		float offsetY = RandomGenerator::Generate(-1.0f, 1.0f) * (intensity + offsetYIntensity);
		float offsetZ = RandomGenerator::Generate(-1.0f, 1.0f) * intensity;

		Vector3 forward = transform.GetForward();
		Vector3 right = transform.GetRight();

		Vector3 translation = transform.translation + (forward * offsetZ) + (right * offsetX);
		translation.y += offsetY;
	} else {

		// シェイク終了
		canExit_ = true;
	}
}

void FollowCameraShakeState::Exit() {

	// リセット
	shakeTimer_ = 0.0f;
	canExit_ = false;
}

void FollowCameraShakeState::ImGui([[maybe_unused]] const FollowCamera& followCamera) {

	ImGui::DragFloat("shakeXZIntensity", &shakeXZIntensity_, 0.01f);
	ImGui::DragFloat("shakeOffsetYIntensity", &shakeOffsetYIntensity_, 0.01f);
	ImGui::DragFloat("shakeTime", &shakeTime_, 0.01f);
	Easing::SelectEasingType(shakeEasingType_);
}

void FollowCameraShakeState::ApplyJson(const Json& data) {

	shakeXZIntensity_ = JsonAdapter::GetValue<float>(data, "shakeXZIntensity_");
	shakeOffsetYIntensity_ = JsonAdapter::GetValue<float>(data, "shakeOffsetYIntensity_");
	shakeTime_ = JsonAdapter::GetValue<float>(data, "shakeTime_");
	shakeEasingType_ = static_cast<EasingType>(JsonAdapter::GetValue<int>(data, "shakeEasingType_"));
}

void FollowCameraShakeState::SaveJson(Json& data) {

	data["shakeXZIntensity_"] = shakeXZIntensity_;
	data["shakeOffsetYIntensity_"] = shakeOffsetYIntensity_;
	data["shakeTime_"] = shakeTime_;
	data["shakeEasingType_"] = static_cast<int>(shakeEasingType_);
}