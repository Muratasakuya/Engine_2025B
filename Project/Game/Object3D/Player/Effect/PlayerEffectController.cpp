#include "PlayerEffectController.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Input/Input.h>

//============================================================================
//	PlayerEffectController classMethods
//============================================================================

void PlayerEffectController::Init() {

	// 各effectの初期化
	circleHitEffect_ = std::make_unique<PlayerCircleHitEffect>();
	circleHitEffect_->Init();

	burstHitEffect_ = std::make_unique<PlayerBurstHitEffect>();
	burstHitEffect_->Init();

	lightningHitEffect0_ = std::make_unique<PlayerLightningHitEffect>();
	lightningHitEffect0_->Init();
	
	lightningHitEffect1_ = std::make_unique<PlayerLightningHitEffect>();
	lightningHitEffect1_->Init();

	lightningHitEffect2_ = std::make_unique<PlayerLightningHitEffect>();
	lightningHitEffect2_->Init();

	lightningHitEffect3_ = std::make_unique<PlayerLightningHitEffect>();
	lightningHitEffect3_->Init();

	isAttack_ = false;

	// json適応
	ApplyJson();
}

void PlayerEffectController::Update(const std::unordered_set<PlayerBehaviorType>& behaviors) {

	// 設定されたbehaviorで更新
	UpdateBehavior(behaviors);
}

void PlayerEffectController::UpdateBehavior(const std::unordered_set<PlayerBehaviorType>& behaviors) {

	// ダッシュ
	if (CheckCurrentBehaviors(behaviors, { PlayerBehaviorType::Dash })) {


	}

	// 攻撃受け流し
	if (CheckCurrentBehaviors(behaviors, { PlayerBehaviorType::Parry })) {


	}

	// animationをさせる
	if (!isAttack_) {
		if (Input::GetInstance()->TriggerKey(DIK_E)) {

			circleHitEffect_->StartAnimation();
			burstHitEffect_->StartAnimation();
			lightningHitEffect0_->StartAnimation();
			lightningHitEffect1_->StartAnimation();
			lightningHitEffect2_->StartAnimation();
			lightningHitEffect3_->StartAnimation();

			isAttack_ = true;
		}
	} else {

		timer_ += GameTimer::GetDeltaTime();
		if (timer_ > 1.0f) {

			timer_ = 0.0f;

			circleHitEffect_->Reset();
			burstHitEffect_->Reset();
			lightningHitEffect0_->Reset();
			lightningHitEffect1_->Reset();
			lightningHitEffect2_->Reset();
			lightningHitEffect3_->Reset();

			isAttack_ = false;
		}
	}

	// 更新は常に行う、triggerで発生
	circleHitEffect_->UpdateAnimation();
	burstHitEffect_->UpdateAnimation();
	lightningHitEffect0_->UpdateAnimation();
	lightningHitEffect1_->UpdateAnimation();
	lightningHitEffect2_->UpdateAnimation();
	lightningHitEffect3_->UpdateAnimation();
}

void PlayerEffectController::ImGui() {

	if (ImGui::Button("Save Json", ImVec2(itemWidth_, 32.0f))) {

		SaveJson();
	}

	if (ImGui::BeginTabBar("PlayerEffectControllerTabs")) {

		if (ImGui::BeginTabItem("circleHitEffect")) {

			circleHitEffect_->ImGui();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("burstHitEffect")) {

			burstHitEffect_->ImGui();
			ImGui::EndTabItem();
		}
		
		if (ImGui::BeginTabItem("lightningHitEffect")) {

			lightningHitEffect0_->ImGui();
			ImGui::EndTabItem();
		}


		ImGui::EndTabBar();
	}
}

void PlayerEffectController::ApplyJson() {


}

void PlayerEffectController::SaveJson() {

	// 値の保存
	circleHitEffect_->SaveJson();
	burstHitEffect_->SaveJson();
	lightningHitEffect0_->SaveJson();
}

bool PlayerEffectController::CheckCurrentBehaviors(
	const std::unordered_set<PlayerBehaviorType>& currentBehaviors,
	const std::initializer_list<PlayerBehaviorType> behaviours) {

	for (auto state : behaviours) {
		if (currentBehaviors.find(state) == currentBehaviors.end()) {
			return false;
		}
	}
	return true;
}