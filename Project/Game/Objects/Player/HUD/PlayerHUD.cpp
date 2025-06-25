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

	// HP背景
	hpBackground_ = std::make_unique<GameEntity2D>();
	hpBackground_->Init("playerHPBackground", "hpBackground", "PlayerHUD");

	// HP残量
	hpBar_ = std::make_unique<GameHPBar>();
	hpBar_->Init("playerHPBar", "whiteAlphaGradation_1", "hpBar", "PlayerHUD");

	// スキル値
	skilBar_ = std::make_unique<GameHPBar>();
	skilBar_->Init("playerSkilBar", "whiteAlphaGradation_1", "destroyBar", "PlayerHUD");

	// 名前文字表示
	nameText_ = std::make_unique<GameEntity2D>();
	nameText_->Init("playerName", "playerName", "PlayerHUD");

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

	// HP残量を更新
	hpBar_->Update(stats_.currentHP, stats_.maxHP, true);
	// スキル値を更新
	skilBar_->Update(stats_.currentSkilPoint, stats_.maxSkilPoint, true);

	// 入力状態に応じて表示を切り替える
	ChangeAllOperateSprite();
}

void PlayerHUD::ChangeAllOperateSprite() {

	attack_.ChangeDynamicSprite(inputType_);
	dash_.ChangeDynamicSprite(inputType_);
	skil_.ChangeDynamicSprite(inputType_);
	special_.ChangeDynamicSprite(inputType_);
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

	if (hpBackgroundParameter_.ImGui("HPBackground")) {

		hpBackground_->SetTranslation(hpBackgroundParameter_.translation);
	}

	if (hpBarParameter_.ImGui("HPBar")) {

		hpBar_->SetTranslation(hpBarParameter_.translation);
	}

	if (skilBarParameter_.ImGui("SkilBar")) {

		skilBar_->SetTranslation(skilBarParameter_.translation);
	}


	if (nameTextParameter_.ImGui("NameText")) {

		nameText_->SetTranslation(nameTextParameter_.translation);
	}

	ImGui::Separator();

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

	hpBackgroundParameter_.ApplyJson(data["hpBackground"]);
	GameCommon::SetInitParameter(*hpBackground_, hpBackgroundParameter_);

	hpBarParameter_.ApplyJson(data["hpBar"]);
	GameCommon::SetInitParameter(*hpBar_, hpBarParameter_);

	skilBarParameter_.ApplyJson(data["skilBar"]);
	GameCommon::SetInitParameter(*skilBar_, skilBarParameter_);

	nameTextParameter_.ApplyJson(data["nameText"]);
	GameCommon::SetInitParameter(*nameText_, nameTextParameter_);

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

	hpBackgroundParameter_.SaveJson(data["hpBackground"]);
	hpBarParameter_.SaveJson(data["hpBar"]);
	skilBarParameter_.SaveJson(data["skilBar"]);
	nameTextParameter_.SaveJson(data["nameText"]);

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

void PlayerHUD::InputStateSprite::ChangeDynamicSprite(InputType type) {

	// 表示の切り替え
	for (auto& [key, sprite] : dynamicSprites) {
		if (key == type) {

			sprite->SetAlpha(1.0f);
		} else {

			sprite->SetAlpha(0.0f);
		}
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
