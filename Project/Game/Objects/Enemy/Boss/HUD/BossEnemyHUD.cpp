#include "BossEnemyHUD.h"

//============================================================================
//	include
//============================================================================
#include <Lib/Adapter/JsonAdapter.h>

//============================================================================
//	BossEnemyHUD classMethods
//============================================================================

void BossEnemyHUD::InitSprite() {

	// HP背景
	hpBackground_ = std::make_unique<GameEntity2D>();
	hpBackground_->Init("enemyHPBackground", "hpBackground", "BossEnemyHUD");

	// HP残量
	hpBar_ = std::make_unique<GameHPBar>();
	hpBar_->Init("enemyHPBar", "whiteAlphaGradation", "hpBar", "BossEnemyHUD");

	// 名前文字表示
	nameText_ = std::make_unique<GameEntity2D>();
	nameText_->Init("bossName", "bossName", "BossEnemyHUD");
}

void BossEnemyHUD::SetInitParameter(GameEntity2D& sprite, InitParameter parameter) {

	sprite.SetTranslation(parameter.translation);
}

void BossEnemyHUD::Init() {

	// sprite初期化
	InitSprite();

	// json適応
	ApplyJson();
}

void BossEnemyHUD::Update() {

	// sprite更新
	UpdateSprite();
}

void BossEnemyHUD::UpdateSprite() {

	// HP残量を更新
	hpBar_->Update(stats_.currentHP, stats_.maxHP);
}

void BossEnemyHUD::ImGui() {

	if (ImGui::Button("SaveJson...hudParameter.json")) {

		SaveJson();
	}

	if (hpBackgroundParameter_.ImGui("HPBackground")) {

		hpBackground_->SetTranslation(hpBackgroundParameter_.translation);
	}

	if (hpBarParameter_.ImGui("HPBar")) {

		hpBar_->SetTranslation(hpBarParameter_.translation);
	}

	if (nameTextParameter_.ImGui("NameText")) {

		nameText_->SetTranslation(nameTextParameter_.translation);
	}
}

void BossEnemyHUD::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck("Enemy/Boss/hudParameter.json", data)) {
		return;
	}

	hpBackgroundParameter_.ApplyJson(data["hpBackground"]);
	SetInitParameter(*hpBackground_, hpBackgroundParameter_);

	hpBarParameter_.ApplyJson(data["hpBar"]);
	SetInitParameter(*hpBar_, hpBarParameter_);

	nameTextParameter_.ApplyJson(data["nameText"]);
	SetInitParameter(*nameText_, nameTextParameter_);
}

void BossEnemyHUD::SaveJson() {

	Json data;

	hpBackgroundParameter_.SaveJson(data["hpBackground"]);
	hpBarParameter_.SaveJson(data["hpBar"]);
	nameTextParameter_.SaveJson(data["nameText"]);

	JsonAdapter::Save("Enemy/Boss/hudParameter.json", data);
}

bool BossEnemyHUD::InitParameter::ImGui(const std::string& label) {

	bool edit = false;

	ImGui::SeparatorText(label.c_str());

	edit = ImGui::DragFloat2(("translation##" + label).c_str(), &translation.x, 1.0f);

	return edit;
}

void BossEnemyHUD::InitParameter::ApplyJson(const Json& data) {

	translation = translation.FromJson(data["translation"]);
}

void BossEnemyHUD::InitParameter::SaveJson(Json& data) {

	data["translation"] = translation.ToJson();
}