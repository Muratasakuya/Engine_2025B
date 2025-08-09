#include "GameFinishUI.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Config.h>
#include <Engine/Utility/EnumAdapter.h>
#include <Game/Objects/Common/GameButtonBlinkingUpdater.h>

// imgui
#include <imgui.h>

//============================================================================
//	GameFinishUI classMethods
//============================================================================

void GameFinishUI::InitSprites() {

	const std::string groupName = "GameFinishUI";

	// 使用するスプライトの生成
	// 電源
	powerIcon_ = std::make_unique<GameButton>();
	powerIcon_->Init("powerIcon", groupName);
	powerIcon_->RegisterUpdater(GameButtonResponseType::AnyMouseClick,
		std::make_unique<GameButtonBlinkingUpdater>());

	// 終了しますか表示の背景
	askFinishBackground_ = std::make_unique<GameObject2D>();
	askFinishBackground_->Init("gameFinishTextBackground", "gameFinishTextBackground", groupName);

	// 終了しますか表示
	askFinish_ = std::make_unique<GameObject2D>();
	askFinish_->Init("gameFinishText", "gameFinishText", groupName);

	// キャンセル
	selectCancel_ = std::make_unique<GameObject2D>();
	selectCancel_->Init("cancelText", "cancelText", groupName);

	// OK -> Finish
	selectOK_ = std::make_unique<GameObject2D>();
	selectOK_->Init("okText", "okText", groupName);
}

void GameFinishUI::SetSpritePos() {

	// 各スプライトの座標を設定する
	// 電源
	Vector2 powerPos = powerIcon_->GetSize() + (powerIcon_->GetSize() / 2.0f);
	powerPos.y = Config::kWindowHeightf - powerPos.y;
	powerIcon_->SetTranslation(powerPos);
	// 終了しますか表示の背景
	askFinishBackground_->SetCenterTranslation();
	// 終了しますか表示
	askFinish_->SetCenterTranslation();

	const Vector2 backgroundPos = askFinishBackground_->GetTranslation();
	const float selectPosY = backgroundPos.y + (askFinishBackground_->GetSize().y / 2.0f);

	// キャンセル
	selectCancel_->SetTranslation(Vector2(backgroundPos.x - selectButtonSpacing_, selectPosY));
	// OK -> Finish
	selectOK_->SetTranslation(Vector2(backgroundPos.x + selectButtonSpacing_, selectPosY));
}

void GameFinishUI::Init() {

	// スプライトの初期化
	InitSprites();

	// 初期化値設定
	currentState_ = State::Power;
}

void GameFinishUI::Update() {

	// 状態別の更新処理
	UpdateState();
}

void GameFinishUI::UpdateState() {

	switch (currentState_) {
	case GameFinishUI::State::Power: {

		powerIcon_->Update();
		break;
	}
	case GameFinishUI::State::Select: {

		break;
	}
	case GameFinishUI::State::Finish:{

		break;
	}
	}
}

void GameFinishUI::ImGui() {

	ImGui::Text("currentState: %s", EnumAdapter<State>::ToString(currentState_));

	if (ImGui::DragFloat("selectButtonSpacing", &selectButtonSpacing_, 0.1f)) {

		SetSpritePos();
	}

	bool edit = false;
	edit |= powerIcon_->ImGuiSize();
	edit |= askFinishBackground_->ImGuiSize();
	edit |= askFinish_->ImGuiSize();
	edit |= selectCancel_->ImGuiSize();
	edit |= selectOK_->ImGuiSize();

	if (edit) {

		SetSpritePos();
	}

	ImGui::SeparatorText("Other Settings");

	powerIcon_->ImGui();
}

void GameFinishUI::ApplyJson(const Json& data) {

	selectButtonSpacing_ = data.value("selectButtonSpacing_", 4.0f);

	powerIcon_->SetSize(JsonAdapter::ToObject<Vector2>(data["powerIcon_Size"]));
	askFinishBackground_->SetSize(JsonAdapter::ToObject<Vector2>(data["askFinishBackground_Size"]));
	askFinish_->SetSize(JsonAdapter::ToObject<Vector2>(data["askFinish_Size"]));
	selectCancel_->SetSize(JsonAdapter::ToObject<Vector2>(data["selectCancel_Size"]));
	selectOK_->SetSize(JsonAdapter::ToObject<Vector2>(data["selectOK_Size"]));

	// 値設定
	SetSpritePos();

	if (data.contains("powerIcon_")) {

		powerIcon_->FromJson(data["powerIcon_"]);
	}
}

void GameFinishUI::SaveJson(Json& data) {

	data["selectButtonSpacing_"] = selectButtonSpacing_;

	data["powerIcon_Size"] = powerIcon_->GetSize().ToJson();
	data["askFinishBackground_Size"] = askFinishBackground_->GetSize().ToJson();
	data["askFinish_Size"] = askFinish_->GetSize().ToJson();
	data["selectCancel_Size"] = selectCancel_->GetSize().ToJson();
	data["selectOK_Size"] = selectOK_->GetSize().ToJson();

	powerIcon_->ToJson(data["powerIcon_"]);
}

bool GameFinishUI::IsSelectFinish() const {

	bool result = currentState_ == State::Finish;
	return result;
}