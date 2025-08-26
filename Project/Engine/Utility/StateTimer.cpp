#include "StateTimer.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/GameTimer.h>
#include <Engine/Utility/EnumAdapter.h>

// imgui
#include <imgui.h>

//============================================================================
//	StateTimer classMethods
//============================================================================

void StateTimer::Update() {

	current_ += GameTimer::GetDeltaTime();
	t_ = std::clamp(current_ / target_, 0.0f, 1.0f);
	easedT_ = EasedValue(easeingType_, t_);
}

void StateTimer::Reset() {

	current_ = 0.0f;
	t_ = 0.0f;
	easedT_ = 0.0f;
}

bool StateTimer::IsReached() const {

	// 現在の時間が目標時間に達したら
	return current_ > target_;
}

void StateTimer::ImGui(const std::string& name) {

	ImGui::SeparatorText(name.c_str());
	ImGui::PushID(name.c_str());

	ImGui::Text(std::format("currentT: {}", t_).c_str());
	ImGui::DragFloat("targetTime", &target_, 0.01f);
	Easing::SelectEasingType(easeingType_, name);

	ImGui::PopID();
}

void StateTimer::FromJson(const Json& data) {

	if (data.empty()) {
		return;
	}

	target_ = data.value("target_", 0.8f);

	const auto& easing = EnumAdapter<EasingType>::FromString(data.value("easeingType_", "EaseInSine"));
	easeingType_ = easing.value();
}

void StateTimer::ToJson(Json& data) {

	data["target_"] = target_;
	data["easeingType_"] = EnumAdapter<EasingType>::ToString(easeingType_);
}