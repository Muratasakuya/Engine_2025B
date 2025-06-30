#include "BossEnemyHUD.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/GameTimer.h>
#include <Game/Camera/FollowCamera.h>
#include <Game/Objects/Enemy/Boss/Entity/BossEnemy.h>
#include <Lib/Adapter/JsonAdapter.h>
#include <Lib/Adapter/RandomGenerator.h>

//============================================================================
//	BossEnemyHUD classMethods
//============================================================================

void BossEnemyHUD::InitSprite() {

	// HP背景
	hpBackground_ = std::make_unique<GameEntity2D>();
	hpBackground_->Init("enemyHPBackground", "hpBackground", "BossEnemyHUD");

	// HP残量
	hpBar_ = std::make_unique<GameHPBar>();
	hpBar_->Init("enemyHPBar", "whiteAlphaGradation_0", "hpBar", "BossEnemyHUD");

	// 撃破靭性値
	destroyBar_ = std::make_unique<GameHPBar>();
	destroyBar_->Init("enemyDestroyBar", "whiteAlphaGradation_0", "destroyBar", "BossEnemyHUD");

	// 撃破靭性値の数字表示
	destroyNumDisplay_ = std::make_unique<GameDigitDisplay>();
	destroyNumDisplay_->Init(2, "toughnessNumber", "destroyNum", "BossEnemyHUD");

	// 名前文字表示
	nameText_ = std::make_unique<GameEntity2D>();
	nameText_->Init("bossName", "bossName", "BossEnemyHUD");

	// ダメージ表示
	damagePopups_.resize(damageDisplayMaxNum_);
	for (uint32_t index = 0; index < damageDisplayMaxNum_; ++index) {

		damagePopups_[index].digits = std::make_unique<GameDigitDisplay>();
		damagePopups_[index].digits->Init(damageDigitMaxNum_, "damageNumber",
			"damagePopup_" + std::to_string(index), "BossEnemyHUD");

		// postProcess適応
		damagePopups_[index].digits->SetPostProcessEnable(true);

		// 表示しない状態
		damagePopups_[index].active = false;
	}
}

void BossEnemyHUD::Init() {

	// sprite初期化
	InitSprite();

	// json適応
	ApplyJson();
}

void BossEnemyHUD::SetDamage(int damage) {

	// ダメージを設定
	receivedDamages_.push_back(std::clamp(damage, 0, 9999));
}

void BossEnemyHUD::Update(const BossEnemy& bossEnemy) {

	// sprite更新
	UpdateSprite(bossEnemy);
}

void BossEnemyHUD::UpdateSprite(const BossEnemy& bossEnemy) {

	// HP残量を更新
	hpBar_->Update(stats_.currentHP, stats_.maxHP, true);
	// 撃破靭性値を更新
	destroyBar_->Update(stats_.currentDestroyToughness, stats_.maxDestroyToughness, true);

	// 撃破靭性値の数字を更新
	destroyNumDisplay_->Update(2, stats_.currentDestroyToughness);

	// ダメージ表記の更新
	UpdateDamagePopups(bossEnemy);
}

void BossEnemyHUD::UpdateDamagePopups(const BossEnemy& bossEnemy) {

	const float deltaTime = GameTimer::GetDeltaTime();

	// ダメージを受けている間のみ更新
	while (!receivedDamages_.empty()) {

		int damage = receivedDamages_.front();
		receivedDamages_.pop_front();
		auto it = std::find_if(damagePopups_.begin(), damagePopups_.end(),
			[](const DamagePopup& p) { return !p.active; });
		// 上限異常は表示できないようにする
		if (it == damagePopups_.end()) {
			break;
		}

		// 値を設定、初期化
		it->value = damage;
		it->active = true;
		it->timer = 0.0f;
		it->outTimer = 0.0f;

		// スクリーン座標を取得
		Vector2 bossScreen = it->digits->ProjectToScreen(bossEnemy.GetTranslation(), *followCamera_);
		Vector2 randomOffset{
			RandomGenerator::Generate(-damageDisplayPosRandomRange_.x,damageDisplayPosRandomRange_.x),
			RandomGenerator::Generate(-damageDisplayPosRandomRange_.y,damageDisplayPosRandomRange_.y)
		};
		it->basePos = Vector2(bossScreen.x,
			bossScreen.y + bossScreenPosOffsetY_) + randomOffset;

		// 表示する桁の更新
		it->digits->Update(damageDigitMaxNum_, it->value);
	}
	for (auto& popup : damagePopups_) {

		// 表示しない
		if (!popup.active) {
			popup.digits->SetSize(Vector2::AnyInit(0.0f));
			continue;
		}

		totalAppearDuration_ = damageDisplayTime_ + digitDisplayInterval_ * (damageDigitMaxNum_ - 1);
		const float totalStayEnd = totalAppearDuration_ + damageStayTime_;
		float prevTimer = popup.timer;
		popup.timer += deltaTime;
		if (popup.timer < totalAppearDuration_) {

			for (uint32_t i = 0; i < damageDigitMaxNum_; ++i) {

				float localT = (popup.timer - i * digitDisplayInterval_) / damageDisplayTime_;
				localT = std::clamp(localT, 0.0f, 1.0f);
				float eased = EasedValue(damageDisplayEasingType_, localT);

				Vector2 size = Vector2::Lerp(damageDisplayMaxSize_,
					damageDisplaySize_, eased);
				popup.digits->SetDigitSize(i, size);
				
				float emissive = std::lerp(maxDamageEmissive_, 0.0f, eased);
				popup.digits->SetEmissive(i, emissive);
			}
		} else if (popup.timer < totalStayEnd) {

			// 全て表示した後damageStayTime_分だけ表示する
			for (uint32_t i = 0; i < damageDigitMaxNum_; ++i) {

				popup.digits->SetDigitSize(i, damageDisplaySize_);
			}
		} else {
			if (prevTimer < totalStayEnd) {

				popup.outTimer = 0.0f;
			}

			popup.outTimer += deltaTime;
			float outT = std::clamp(popup.outTimer / damageOutTime_, 0.0f, 1.0f);
			float eased = EasedValue(damageOutEasingType_, outT);
			Vector2 size = Vector2::Lerp(damageDisplaySize_, Vector2::AnyInit(0.0f), eased);
			for (uint32_t i = 0; i < damageDigitMaxNum_; ++i) {

				popup.digits->SetDigitSize(i, size);
				if (popup.outTimer >= damageOutTime_) {

					popup.active = false;
					continue;
				}
			}
		}

		// 座標設定
		popup.digits->SetTranslation(popup.basePos, Vector2(damageDisplaySize_.x + damageNumSpacing_, 0.0f));
	}
}

void BossEnemyHUD::ImGui() {

	if (ImGui::Button("SaveJson...hudParameter.json")) {

		SaveJson();
	}

	if (ImGui::BeginTabBar("BossEnemyHUD")) {
		if (ImGui::BeginTabItem("Init##HUD")) {
			if (hpBackgroundParameter_.ImGui("HPBackground")) {

				hpBackground_->SetTranslation(hpBackgroundParameter_.translation);
			}

			if (hpBarParameter_.ImGui("HPBar")) {

				hpBar_->SetTranslation(hpBarParameter_.translation);
			}

			if (destroyBarParameter_.ImGui("DestroyBar")) {

				destroyBar_->SetTranslation(destroyBarParameter_.translation);
			}

			if (destroyNumParameter_.ImGui("DestroyNum") ||
				ImGui::DragFloat2("destroyNumOffset", &destroyNumOffset_.x, 1.0f) ||
				ImGui::DragFloat2("destroyNumSize", &destroyNumSize_.x, 1.0f)) {

				destroyNumDisplay_->SetTranslation(destroyNumParameter_.translation, destroyNumOffset_);
				destroyNumDisplay_->SetSize(destroyNumSize_);
			}

			if (nameTextParameter_.ImGui("NameText")) {

				nameText_->SetTranslation(nameTextParameter_.translation);
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Damage##HUD")) {

			ImGui::DragFloat("damageDisplayTime", &damageDisplayTime_, 0.01f);
			ImGui::DragFloat("damageOutTime", &damageOutTime_, 0.01f);
			ImGui::DragFloat("damageStayTime", &damageStayTime_, 0.01f);
			ImGui::DragFloat("damageNumSpacing", &damageNumSpacing_, 0.1f);
			ImGui::DragFloat("digitDisplayInterval", &digitDisplayInterval_, 0.01f);
			ImGui::DragFloat("bossScreenPosOffsetY", &bossScreenPosOffsetY_, 1.0f);
			ImGui::DragFloat("maxDamageEmissive", &maxDamageEmissive_, 1.0f);
			ImGui::DragFloat2("damageDisplayPosRandomRange", &damageDisplayPosRandomRange_.x, 1.0f);
			ImGui::DragFloat2("damageDisplayMaxSize", &damageDisplayMaxSize_.x, 1.0f);
			ImGui::DragFloat2("damageDisplaySize", &damageDisplaySize_.x, 1.0f);

			Easing::SelectEasingType(damageDisplayEasingType_, "damageDisplaySize");
			Easing::SelectEasingType(damageOutEasingType_, "damageOutEasingType");
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

void BossEnemyHUD::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck("Enemy/Boss/hudParameter.json", data)) {
		return;
	}

	hpBackgroundParameter_.ApplyJson(data["hpBackground"]);
	GameCommon::SetInitParameter(*hpBackground_, hpBackgroundParameter_);

	hpBarParameter_.ApplyJson(data["hpBar"]);
	GameCommon::SetInitParameter(*hpBar_, hpBarParameter_);

	destroyBarParameter_.ApplyJson(data["destroyBar"]);
	GameCommon::SetInitParameter(*destroyBar_, destroyBarParameter_);

	destroyNumParameter_.ApplyJson(data["destroyNum"]);
	destroyNumOffset_ = destroyNumOffset_.FromJson(data["destroyNum"]["numOffset"]);
	destroyNumSize_ = destroyNumSize_.FromJson(data["destroyNum"]["numSize"]);
	destroyNumDisplay_->SetTranslation(destroyNumParameter_.translation, destroyNumOffset_);
	destroyNumDisplay_->SetSize(destroyNumSize_);

	nameTextParameter_.ApplyJson(data["nameText"]);
	GameCommon::SetInitParameter(*nameText_, nameTextParameter_);

	damageDisplayTime_ = JsonAdapter::GetValue<float>(data, "damageDisplayTime_");
	damageOutTime_ = JsonAdapter::GetValue<float>(data, "damageOutTime_");
	damageNumSpacing_ = JsonAdapter::GetValue<float>(data, "damageNumSpacing_");
	digitDisplayInterval_ = JsonAdapter::GetValue<float>(data, "digitDisplayInterval_");
	bossScreenPosOffsetY_ = JsonAdapter::GetValue<float>(data, "bossScreenPosOffsetY_");
	maxDamageEmissive_ = JsonAdapter::GetValue<float>(data, "maxDamageEmissive_");
	damageStayTime_ = JsonAdapter::GetValue<float>(data, "damageStayTime_");
	damageDisplayPosRandomRange_ = JsonAdapter::ToObject<Vector2>(data["damageDisplayPosRandomRange_"]);
	damageDisplayMaxSize_ = JsonAdapter::ToObject<Vector2>(data["damageDisplayMaxSize_"]);
	damageDisplaySize_ = JsonAdapter::ToObject<Vector2>(data["damageDisplaySize_"]);
	damageDisplayEasingType_ = static_cast<EasingType>(
		JsonAdapter::GetValue<int>(data, "damageDisplayEasingType_"));
	damageOutEasingType_ = static_cast<EasingType>(
		JsonAdapter::GetValue<int>(data, "damageOutEasingType_"));

	totalAppearDuration_ = damageDisplayTime_ + digitDisplayInterval_ * (damageDigitMaxNum_ - 1);
}

void BossEnemyHUD::SaveJson() {

	Json data;

	hpBackgroundParameter_.SaveJson(data["hpBackground"]);
	hpBarParameter_.SaveJson(data["hpBar"]);
	destroyBarParameter_.SaveJson(data["destroyBar"]);
	destroyNumParameter_.SaveJson(data["destroyNum"]);
	data["destroyNum"]["numOffset"] = destroyNumOffset_.ToJson();
	data["destroyNum"]["numSize"] = destroyNumSize_.ToJson();
	nameTextParameter_.SaveJson(data["nameText"]);

	data["damageDisplayTime_"] = damageDisplayTime_;
	data["damageOutTime_"] = damageOutTime_;
	data["damageNumSpacing_"] = damageNumSpacing_;
	data["digitDisplayInterval_"] = digitDisplayInterval_;
	data["bossScreenPosOffsetY_"] = bossScreenPosOffsetY_;
	data["maxDamageEmissive_"] = maxDamageEmissive_;
	data["damageStayTime_"] = damageStayTime_;
	data["damageDisplayPosRandomRange_"] = JsonAdapter::FromObject<Vector2>(damageDisplayPosRandomRange_);
	data["damageDisplayMaxSize_"] = JsonAdapter::FromObject<Vector2>(damageDisplayMaxSize_);
	data["damageDisplaySize_"] = JsonAdapter::FromObject<Vector2>(damageDisplaySize_);
	data["damageDisplayEasingType_"] = static_cast<int>(damageDisplayEasingType_);
	data["damageOutEasingType_"] = static_cast<int>(damageOutEasingType_);

	JsonAdapter::Save("Enemy/Boss/hudParameter.json", data);
}