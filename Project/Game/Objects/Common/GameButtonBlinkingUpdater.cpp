#include "GameButtonBlinkingUpdater.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/GameTimer.h>
#include <Engine/Utility/EnumAdapter.h>

//============================================================================
//	GameButtonBlinkingUpdater classMethods
//============================================================================

void GameButtonBlinkingUpdater::Begin(GameObject2D& object) {

	// 最初の値を保持
	smallScale_ = object.GetSize().x;
	beginColor_ = object.GetColor();
}

void GameButtonBlinkingUpdater::ActiveUpdate(GameObject2D& object) {

	// 時間に応じて補間させる
	loopTimer_ += GameTimer::GetDeltaTime();
	float t = loopTimer_ / loopSpacing_;
	// ループ
	t = animationLoop_.LoopedT(t);
	float easedT = EasedValue(loopEasing_, t);

	// サイズ補間
	object.SetSize(Vector2::Lerp(Vector2::AnyInit(smallScale_),
		Vector2::AnyInit(maxScale_), easedT));
	// 色補間
	object.SetColor(Color::Lerp(startColor_, targetColor_, easedT));
}

void GameButtonBlinkingUpdater::InactiveUpdate(GameObject2D& object) {

	endTimer_ += GameTimer::GetDeltaTime();
	float t = endTimer_ / endTime_;
	float easedT = EasedValue(endEasing_, t);

	// サイズ補間
	object.SetSize(Vector2::Lerp(Vector2::AnyInit(maxScale_),
		Vector2::AnyInit(smallScale_), easedT));
	// 色補間
	object.SetColor(Color::Lerp(targetColor_, startColor_, easedT));
}

void GameButtonBlinkingUpdater::End(GameObject2D& object) {

	// リセット
	loopTimer_ = 0.0f;
	endTimer_ = 0.0f;

	object.SetSize(Vector2::AnyInit(smallScale_));
	object.SetColor(beginColor_);
}

bool GameButtonBlinkingUpdater::IsInactiveFinished() const {

	// 時間経過後終了
	return endTime_ <= endTimer_;
}

void GameButtonBlinkingUpdater::IdleUpdate([[maybe_unused]] GameObject2D&) {
}

void GameButtonBlinkingUpdater::ImGui() {

	ImGui::DragFloat("loopSpacing", &loopSpacing_, 0.01f);
	Easing::SelectEasingType(loopEasing_, "loopEasing_");

	ImGui::DragFloat("endTime", &endTime_, 0.01f);
	Easing::SelectEasingType(endEasing_, "endEasing_");

	ImGui::DragFloat("smallScale", &smallScale_, 0.01f);
	ImGui::DragFloat("maxScale", &maxScale_, 0.01f);

	ImGui::ColorEdit4("startColor", &startColor_.r);
	ImGui::ColorEdit4("targetColor", &targetColor_.r);
}

void GameButtonBlinkingUpdater::FromJson(const Json& data) {

	loopSpacing_ = data["loopSpacing_"];
	endTime_ = data["endTime_"];
	smallScale_ = data["smallScale_"];
	maxScale_ = data["maxScale_"];
	startColor_ = startColor_.FromJson(data["startColor_"]);
	targetColor_ = targetColor_.FromJson(data["targetColor_"]);

	{
		const auto& easing = EnumAdapter<EasingType>::FromString(data["loopEasing_"]);
		loopEasing_ = easing.value();
	}
	{
		const auto& easing = EnumAdapter<EasingType>::FromString(data["endEasing_"]);
		endEasing_ = easing.value();
	}
}

void GameButtonBlinkingUpdater::ToJson(Json& data) {

	data["loopSpacing_"] = loopSpacing_;
	data["endTime_"] = endTime_;
	data["smallScale_"] = smallScale_;
	data["maxScale_"] = maxScale_;
	data["startColor_"] = startColor_.ToJson();
	data["targetColor_"] = targetColor_.ToJson();

	data["loopEasing_"] = EnumAdapter<EasingType>::ToString(loopEasing_);
	data["endEasing_"] = EnumAdapter<EasingType>::ToString(endEasing_);
}