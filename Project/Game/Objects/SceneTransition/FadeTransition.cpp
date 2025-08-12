#include "FadeTransition.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Config.h>
#include <Engine/Utility/JsonAdapter.h>

//============================================================================
//	FadeTransition classMethods
//============================================================================

void FadeTransition::Init() {

	sprite_ = std::make_unique<GameObject2D>();
	sprite_->Init("white", "transitionSprite", "Scene");
	// シーンが切り替わっても破棄しない
	sprite_->SetDestroyOnLoad(false);
	// サイズ、座標設定
	sprite_->SetCenterTranslation();
	sprite_->SetSize(Vector2(Config::kWindowWidthf, Config::kWindowHeightf));

	// fade初期設定
	sprite_->SetColor(Color::Convert(0x1F1F1FFF));
	sprite_->SetAlpha(0.0f);

	// json適応
	ApplyJson();
}

void FadeTransition::Update() {}

void FadeTransition::BeginUpdate() {

	beginTimer_.Update();
	sprite_->SetAlpha(std::lerp(0.0f, 1.0f, beginTimer_.easedT_));

	if (beginTimer_.IsReached()) {

		sprite_->SetAlpha(1.0f);

		// 次に進める
		state_ = TransitionState::Wait;
		beginTimer_.Reset();
	}
}

void FadeTransition::WaitUpdate() {

	waitTimer_.Update();
	if (waitTimer_.IsReached()) {

		// 次に進める
		state_ = TransitionState::End;
		waitTimer_.Reset();
	}
}

void FadeTransition::EndUpdate() {

	endTimer_.Update();
	sprite_->SetAlpha(std::lerp(0.0f, 1.0f, endTimer_.easedT_));

	if (endTimer_.IsReached()) {

		sprite_->SetAlpha(0.0f);

		// 遷移終了
		state_ = TransitionState::Begin;
		endTimer_.Reset();
	}
}

void FadeTransition::ImGui() {

	if (ImGui::Button("Save Json")) {

		SaveJson();
	}

	beginTimer_.ImGui("Begin");
	waitTimer_.ImGui("Wait");
	endTimer_.ImGui("End");
}

void FadeTransition::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck("Scene/Transition/fadeTransition.json", data)) {
		return;
	}

	beginTimer_.FromJson(data["beginTimer_"]);
	waitTimer_.FromJson(data["waitTimer_"]);
	endTimer_.FromJson(data["endTimer_"]);
}

void FadeTransition::SaveJson() {

	Json data;

	beginTimer_.ToJson(data["beginTimer_"]);
	waitTimer_.ToJson(data["waitTimer_"]);
	endTimer_.ToJson(data["endTimer_"]);

	JsonAdapter::Save("Scene/Transition/fadeTransition.json", data);
}