#include "GameResultDisplay.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/JsonAdapter.h>
#include <Engine/Utility/EnumAdapter.h>

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

	float result = resultTimer_.current_;
	// 表示時間の設定と更新
	resultTime_->Update(result);
}

void GameResultDisplay::UpdateResult() {


}

void GameResultDisplay::ImGui() {

	if (ImGui::Button("Save Json")) {

		SaveJson();
	}

	EnumAdapter<State>::Combo("currentState", &currentState_);

	ImGui::SeparatorText("BeginTime");
	{
		if (ImGui::Button("Reset")) {

			randomDisplayTimer_.Reset();
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
	data["timerSize_"] = timerSize_.ToJson();
	data["timerSymbolSize_"] = timerSymbolSize_.ToJson();
	randomDisplayTimer_.ToJson(data["RandomDisplayTime"]);

	// Result

	JsonAdapter::Save("Result/resultDisplay.json", data);
}
