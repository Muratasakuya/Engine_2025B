#include "PlayerEffectController.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Input/Input.h>

//============================================================================
//	PlayerEffectController classMethods
//============================================================================

void PlayerEffectController::Init() {

	lightningHitEffect_ = std::make_unique<PlayerLightningHitEffect>();
	lightningHitEffect_->Init();

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

			isAttack_ = true;

			lightningHitEffect_->StartAnimation(Vector3());
		}
	} else {

		timer_ += GameTimer::GetDeltaTime();
		if (timer_ > 1.0f) {

			timer_ = 0.0f;
			isAttack_ = false;

			lightningHitEffect_->Reset();
		}
	}

	// 更新は常に行う、triggerで発生
	lightningHitEffect_->UpdateAnimation();
}

void PlayerEffectController::ImGui() {

	if (ImGui::Button("Save Json", ImVec2(itemWidth_, 32.0f))) {

		SaveJson();
	}

	if (ImGui::BeginTabBar("PlayerEffectControllerTabs")) {

		if (ImGui::BeginTabItem("lightningHitEffect")) {

			lightningHitEffect_->ImGui();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

void PlayerEffectController::ApplyJson() {
}

void PlayerEffectController::SaveJson() {

	// 値の保存
	lightningHitEffect_->SaveJson();
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