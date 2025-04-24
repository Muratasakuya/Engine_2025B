#include "PlayerEffectController.h"

//============================================================================
//	PlayerEffectController classMethods
//============================================================================

void PlayerEffectController::Init() {

	// 各effectの初期化
	circleHitEffect_ = std::make_unique<PlayerCircleHitEffect>();
	circleHitEffect_->Init();

	burstHitEffect_ = std::make_unique<PlayerBurstHitEffect>();
	burstHitEffect_->Init();

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

	// TEST
	circleHitEffect_->UpdateAnimation();
	burstHitEffect_->UpdateAnimation();
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


		ImGui::EndTabBar();
	}
}

void PlayerEffectController::ApplyJson() {


}

void PlayerEffectController::SaveJson() {

	// 値の保存
	circleHitEffect_->SaveJson();
	burstHitEffect_->SaveJson();
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