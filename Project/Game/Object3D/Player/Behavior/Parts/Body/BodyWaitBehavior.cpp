#include "BodyWaitBehavior.h"

//============================================================================
//	include
//============================================================================
#include <Game/Object3D/Player/Parts/Base/BasePlayerParts.h>
#include <Game/Time/GameTimer.h>

//============================================================================
//	BodyWaitBehavior classMethods
//============================================================================

BodyWaitBehavior::BodyWaitBehavior(const Json& data) {

	if (data.contains(waitBehaviorJsonKey_)) {

		speed_ = JsonAdapter::GetValue<float>(data[waitBehaviorJsonKey_], "speed_");
		amplitude_ = JsonAdapter::GetValue<float>(data[waitBehaviorJsonKey_], "amplitude_");
	}
}

void BodyWaitBehavior::Execute(BasePlayerParts* parts) {

	if (!baseTranslate_.has_value()) {

		// 初期座標を取得する
		baseTranslate_ = parts->GetTransform().translation;
	}

	// 時間を加算
	elapsed_ += GameTimer::GetDeltaTime() * speed_;

	// Y座標を加算
	float offsetY = std::sin(elapsed_) * amplitude_;
	Vector3 translation = parts->GetTransform().translation;
	translation = baseTranslate_.value() + Vector3(0.0f, offsetY, 0.0f);
	parts->SetTranslate(translation);
}

void BodyWaitBehavior::Reset() {

	// 初期化する
	baseTranslate_ = std::nullopt;
	elapsed_ = 0.0f;
}

void BodyWaitBehavior::ImGui() {

	ImGui::PushItemWidth(itemWidth_);

	ImGui::DragFloat("speed", &speed_, 0.01f);
	ImGui::DragFloat("amplitude", &amplitude_, 0.01f);

	ImGui::PopItemWidth();
}

void BodyWaitBehavior::SaveJson(Json& data) {

	// 値の保存
	data[waitBehaviorJsonKey_]["speed_"] = speed_;
	data[waitBehaviorJsonKey_]["amplitude_"] = amplitude_;
}