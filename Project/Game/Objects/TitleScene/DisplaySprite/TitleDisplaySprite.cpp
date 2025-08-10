#include "TitleDisplaySprite.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Config.h>

//============================================================================
//	TitleDisplaySprite classMethods
//============================================================================

void TitleDisplaySprite::InitSprites() {

	const std::string groupName = "TitleDisplaySprite";

	// 使用するスプライトの生成
	name_ = std::make_unique<GameObject2D>();
	name_->Init("title", "title", groupName);

	start_ = std::make_unique<GameObject2D>();
	start_->Init("gameStartText", "gameStartText", groupName);
}

void TitleDisplaySprite::SetSpritePos() {

	// 各スプライトの座標を設定する
	// 名前
	name_->SetCenterTranslation();
	// 開始文字
	start_->SetTranslation(Vector2(Config::kWindowWidthf / 2.0f,
		Config::kWindowHeightf - (start_->GetSize().y * 2.0f)));
}

void TitleDisplaySprite::Init() {

	const std::string groupName = "TitleDisplaySprite";

	// 各機能、スプライトの初期化
	InitSprites();

	finishUI_ = std::make_unique<GameFinishUI>();
	finishUI_->Init();

	// json適応
	ApplyJson();
}

void TitleDisplaySprite::Update() {

	finishUI_->Update();
}

void TitleDisplaySprite::ImGui() {

	ImGui::SetWindowFontScale(0.8f);
	ImGui::PushItemWidth(224.0f);

	if (ImGui::Button("Save")) {

		SaveJson();
	}
	if (ImGui::BeginTabBar("TitleDisplaySprite")) {
		if (ImGui::BeginTabItem("Base")) {

			bool edit = false;
			edit |= name_->ImGuiSize();
			edit |= start_->ImGuiSize();

			if (edit) {

				SetSpritePos();
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("FinishUI")) {

			finishUI_->ImGui();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::PopItemWidth();
	ImGui::SetWindowFontScale(1.0f);
}

void TitleDisplaySprite::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck("Title/titleDisplaySprite.json", data)) {
		return;
	}

	name_->SetSize(JsonAdapter::ToObject<Vector2>(data["name_Size"]));
	start_->SetSize(JsonAdapter::ToObject<Vector2>(data["start_Size"]));

	finishUI_->ApplyJson(data["FinishUI"]);

	// 値設定
	SetSpritePos();
}

void TitleDisplaySprite::SaveJson() {

	Json data;

	data["name_Size"] = name_->GetSize().ToJson();
	data["start_Size"] = start_->GetSize().ToJson();

	finishUI_->SaveJson(data["FinishUI"]);

	JsonAdapter::Save("Title/titleDisplaySprite.json", data);
}