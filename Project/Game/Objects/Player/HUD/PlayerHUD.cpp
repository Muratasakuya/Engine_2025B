#include "PlayerHUD.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Input/Input.h>
#include <Lib/Adapter/JsonAdapter.h>

//============================================================================
//	PlayerHUD classMethods
//============================================================================

void PlayerHUD::InitSprite() {

	// キーボード操作とパッド操作のtextureの名前を格納する
	std::unordered_map<InputType, std::string> dynamicTextures{};

	// 攻撃
	dynamicTextures[InputType::Keyboard] = "leftMouseClick";
	dynamicTextures[InputType::GamePad] = "XButton";
	attack_.Init(0, "attackIcon", dynamicTextures);

	// ダッシュ
	dynamicTextures[InputType::Keyboard] = "rightMouseClick";
	dynamicTextures[InputType::GamePad] = "AButton";
	dash_.Init(1, "dashIcon", dynamicTextures);

	// スキル
	dynamicTextures[InputType::Keyboard] = "EButton";
	dynamicTextures[InputType::GamePad] = "YButton";
	skil_.Init(2, "skilIcon", dynamicTextures);

	// 必殺
	dynamicTextures[InputType::Keyboard] = "QButton";
	dynamicTextures[InputType::GamePad] = "RTButton";
	special_.Init(3, "specialIcon", dynamicTextures);
}

void PlayerHUD::Init() {

	// sprite初期化
	InitSprite();

	// json適応
	ApplyJson();
}

void PlayerHUD::Update() {

	// input状態を取得
	inputType_ = Input::GetInstance()->GetType();

	// sprite更新
	UpdateSprite();
}

void PlayerHUD::UpdateSprite() {
}

void PlayerHUD::SetAllOperateTranslation() {

	attack_.SetTranslation(leftSpriteTranslation_,
		dynamicSpriteOffsetY_, operateSpriteSpancingX_);

	dash_.SetTranslation(leftSpriteTranslation_,
		dynamicSpriteOffsetY_, operateSpriteSpancingX_);

	skil_.SetTranslation(leftSpriteTranslation_,
		dynamicSpriteOffsetY_, operateSpriteSpancingX_);

	special_.SetTranslation(leftSpriteTranslation_,
		dynamicSpriteOffsetY_, operateSpriteSpancingX_);
}

void PlayerHUD::SetAllOperateSize() {

	attack_.SetSize(staticSpriteSize_, dynamicSpriteSize_);
	dash_.SetSize(staticSpriteSize_, dynamicSpriteSize_);
	skil_.SetSize(staticSpriteSize_, dynamicSpriteSize_);
	special_.SetSize(staticSpriteSize_, dynamicSpriteSize_);
}

void PlayerHUD::ImGui() {

	if (ImGui::Button("SaveJson...hudParameter.json")) {

		SaveJson();
	}

	bool edit = false;

	edit |= ImGui::DragFloat2("leftSpriteTranslation", &leftSpriteTranslation_.x, 1.0f);
	edit |= ImGui::DragFloat("dynamicSpriteOffsetY", &dynamicSpriteOffsetY_, 1.0f);
	edit |= ImGui::DragFloat("operateSpriteSpancingX", &operateSpriteSpancingX_, 1.0f);

	if (edit) {

		SetAllOperateTranslation();
	}

	edit |= ImGui::DragFloat2("staticSpriteSize", &staticSpriteSize_.x, 0.1f);
	edit |= ImGui::DragFloat2("dynamicSpriteSize", &dynamicSpriteSize_.x, 0.1f);

	if (edit) {

		SetAllOperateSize();
	}
}

void PlayerHUD::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck("Player/hudParameter.json", data)) {
		return;
	}

	leftSpriteTranslation_ = leftSpriteTranslation_.FromJson(data["leftSpriteTranslation"]);
	staticSpriteSize_ = leftSpriteTranslation_.FromJson(data["staticSpriteSize"]);
	dynamicSpriteSize_ = leftSpriteTranslation_.FromJson(data["dynamicSpriteSize"]);
	dynamicSpriteOffsetY_ = JsonAdapter::GetValue<float>(data, "dynamicSpriteOffsetY");
	operateSpriteSpancingX_ = JsonAdapter::GetValue<float>(data, "operateSpriteSpancingX");

	SetAllOperateTranslation();
	SetAllOperateSize();
}

void PlayerHUD::SaveJson() {

	Json data;

	data["leftSpriteTranslation"] = leftSpriteTranslation_.ToJson();
	data["staticSpriteSize"] = staticSpriteSize_.ToJson();
	data["dynamicSpriteSize"] = dynamicSpriteSize_.ToJson();
	data["dynamicSpriteOffsetY"] = dynamicSpriteOffsetY_;
	data["operateSpriteSpancingX"] = operateSpriteSpancingX_;

	JsonAdapter::Save("Player/hudParameter.json", data);
}

void PlayerHUD::InputStateSprite::Init(uint32_t spriteIndex, const std::string& staticSpriteTextureName,
	const std::unordered_map<InputType, std::string>& dynamicSpritesTextureName) {

	index = spriteIndex;

	// 変化しないspriteの初期化
	staticSprite = std::make_unique<GameEntity2D>();
	staticSprite->Init(staticSpriteTextureName, staticSpriteTextureName, groupName);

	// 変化するspriteをタイプごとに初期化
	for (auto& [type, texture] : dynamicSpritesTextureName) {

		dynamicSprites[type] = std::make_unique<GameEntity2D>();
		dynamicSprites[type]->Init(texture, texture, groupName);
	}
}

void PlayerHUD::InputStateSprite::SetTranslation(const Vector2& leftSpriteTranslation,
	float dynamicSpriteOffsetY, float operateSpriteSpancingX) {

	// X座標
	float translationX = leftSpriteTranslation.x + index * operateSpriteSpancingX;

	// 座標を設定
	staticSprite->SetTranslation(Vector2(translationX, leftSpriteTranslation.y));

	dynamicSprites[InputType::Keyboard]->SetTranslation(
		Vector2(translationX, leftSpriteTranslation.y + dynamicSpriteOffsetY));
	dynamicSprites[InputType::GamePad]->SetTranslation(
		Vector2(translationX, leftSpriteTranslation.y + dynamicSpriteOffsetY));
}

void PlayerHUD::InputStateSprite::SetSize(const Vector2& staticSpriteSize,
	const Vector2& dynamicSpriteSize_) {

	// サイズ設定
	staticSprite->SetSize(staticSpriteSize);
	dynamicSprites[InputType::Keyboard]->SetSize(dynamicSpriteSize_);
	dynamicSprites[InputType::GamePad]->SetSize(dynamicSpriteSize_);
}
