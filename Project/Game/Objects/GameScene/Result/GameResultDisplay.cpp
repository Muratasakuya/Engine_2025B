#include "GameResultDisplay.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/JsonAdapter.h>
#include <Engine/Utility/EnumAdapter.h>
#include <Lib/Adapter/RandomGenerator.h>

//============================================================================
//	GameResultDisplay classMethods
//============================================================================

void GameResultDisplay::Init() {

	// スプライト初期化
	// 時間表示
	resultTime_ = std::make_unique<GameTimerDisplay>();
	resultTime_->Init("dd:dd:dd", "toughnessNumber",
		"toughnessTimeSymbol", "resultTime", "GameResultDisplay");
	// 最初は表示しない
	resultTime_->SetAlpha(0.0f);

	// json適応
	ApplyJson();
}

void GameResultDisplay::Measurement() {

	// 時間計測中...
	resultTimer_.Update();
}

void GameResultDisplay::StartDisplay() {

	// 表示する
	currentState_ = State::BeginTime;
	resultTime_->SetAlpha(1.0f);
	randomDisplayTimer_.Reset();
	randomSwitchIndex_ = 0;

	// 最初の乱数値
	lastRandomTime_ = RandomGenerator::Generate<float>(0.0f, randomTimeMax_);

	// 次の切り替え閾値
	auto calcThreshold = [&](int index) {
		float u = float(index + 1) / float(randomSwitchCount_ + 1);
		return std::pow(u, randomSwitchBias_); };
	nextSwitchT_ = (randomSwitchCount_ > 0) ? calcThreshold(0) : 1.0f;
}

void GameResultDisplay::Update() {

	// 各状態の更新
	switch (currentState_) {
	case GameResultDisplay::State::BeginTime: {

		UpdateBeginTime();
		break;
	}
	case GameResultDisplay::State::Result: {

		UpdateResult();
		break;
	}
	}
}

void GameResultDisplay::UpdateBeginTime() {

	// 時間を進める
	randomDisplayTimer_.Update();

	// 最終的に表示する値
	const float resultTime = resultTimer_.current_ / 100.0f;

	// 収束値を設定
	const float maxSpanAtStart = std::max(resultTime, randomTimeMax_ - resultTime);
	const float span = maxSpanAtStart * (1.0f - randomDisplayTimer_.easedT_);

	float min = std::max(0.0f, resultTime - span);
	float max = std::min(randomTimeMax_, resultTime + span);

	// 切り替え予約
	auto calcThreshold = [&](int index) {
		float u = float(index + 1) / float(randomSwitchCount_ + 1);
		return std::pow(u, randomSwitchBias_); };

	float progress = randomDisplayTimer_.easedT_;
	while (randomSwitchIndex_ < randomSwitchCount_ && progress >= nextSwitchT_) {

		// 区間内で新しい乱数をセット
		lastRandomTime_ = RandomGenerator::Generate<float>(min, max);
		++randomSwitchIndex_;
		nextSwitchT_ = (randomSwitchIndex_ < randomSwitchCount_) ?
			calcThreshold(randomSwitchIndex_) : 1.0f;
	}

	// 表示値する値を設定
	const float display = randomDisplayTimer_.IsReached() ?
		resultTime : lastRandomTime_ / 100.0f;

	resultTime_->Update(display);

	// タイマーが終わったら状態遷移
	if (randomDisplayTimer_.IsReached()) {

		currentState_ = State::Result;
	}
}

void GameResultDisplay::UpdateResult() {


}

void GameResultDisplay::ImGui() {

	if (ImGui::Button("Save Json")) {

		SaveJson();
	}

	ImGui::DragFloat("resultTimer_.current", &resultTimer_.current_, 0.1f);
	ImGui::DragFloat("randomTimeMax", &randomTimeMax_, 0.1f);
	ImGui::DragInt("randomSwitchCount", &randomSwitchCount_, 1);
	ImGui::DragFloat("randomSwitchBias", &randomSwitchBias_, 0.05f);
	ImGui::Text("switch %d / %d", randomSwitchIndex_, randomSwitchCount_);

	EnumAdapter<State>::Combo("currentState", &currentState_);

	ImGui::SeparatorText("BeginTime");
	{
		if (ImGui::Button("Reset")) {

			randomDisplayTimer_.Reset();
			randomSwitchIndex_ = 0;
			// 最初の乱数値
			lastRandomTime_ = RandomGenerator::Generate<float>(0.0f, randomTimeMax_);

			// 次の切り替え閾値
			auto calcThreshold = [&](int index) {
				float u = float(index + 1) / float(randomSwitchCount_ + 1);
				return std::pow(u, randomSwitchBias_); };
			nextSwitchT_ = (randomSwitchCount_ > 0) ? calcThreshold(0) : 1.0f;
		}

		if (ImGui::DragFloat2("timerTranslation", &timerTranslation_.x, 1.0f)) {

			resultTime_->SetTranslation(timerTranslation_);
		}
		if (ImGui::DragFloat("timerOffsetX", &timerOffsetX_, 0.1f)) {

			resultTime_->SetOffset(Vector2(timerOffsetX_, 0.0f));
		}
		if (ImGui::DragFloat2("timerSize", &timerSize_.x, 0.1f)) {

			resultTime_->SetTimerSize(timerSize_);
		}
		if (ImGui::DragFloat2("timerSymbolSize", &timerSymbolSize_.x, 0.1f)) {

			resultTime_->SetSymbolSize(timerSymbolSize_);
		}
	}

	randomDisplayTimer_.ImGui("RandomDisplayTime");
}

void GameResultDisplay::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck("Result/resultDisplay.json", data)) {
		return;
	}

	// BeginTime
	timerTranslation_ = Vector2::FromJson(data.value("timerTranslation_", Json()));
	timerOffsetX_ = data.value("timerOffsetX_", 4.0f);
	randomTimeMax_ = data.value("randomTimeMax_", 4.0f);
	randomSwitchCount_ = data.value("randomSwitchCount_", 10);
	randomSwitchBias_ = data.value("randomSwitchBias_", 1.5f);
	timerSize_ = Vector2::FromJson(data.value("timerSize_", Json()));
	timerSymbolSize_ = Vector2::FromJson(data.value("timerSymbolSize_", Json()));
	randomDisplayTimer_.FromJson(data["RandomDisplayTime"]);

	// 値を適応
	resultTime_->SetOffset(Vector2(timerOffsetX_, 0.0f));
	resultTime_->SetTranslation(timerTranslation_);
	resultTime_->SetTimerSize(timerSize_);
	resultTime_->SetSymbolSize(timerSymbolSize_);

	// Result
}

void GameResultDisplay::SaveJson() {

	Json data;

	// BeginTime
	data["timerTranslation_"] = timerTranslation_.ToJson();
	data["timerOffsetX_"] = timerOffsetX_;
	data["randomTimeMax_"] = randomTimeMax_;
	data["randomSwitchCount_"] = randomSwitchCount_;
	data["randomSwitchBias_"] = randomSwitchBias_;
	data["timerSize_"] = timerSize_.ToJson();
	data["timerSymbolSize_"] = timerSymbolSize_.ToJson();
	randomDisplayTimer_.ToJson(data["RandomDisplayTime"]);

	// Result

	JsonAdapter::Save("Result/resultDisplay.json", data);
}