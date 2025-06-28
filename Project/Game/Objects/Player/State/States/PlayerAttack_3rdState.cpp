#include "PlayerAttack_3rdState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/GameTimer.h>
#include <Game/Objects/Player/Entity/Player.h>

//============================================================================
//	PlayerAttack_3rdState classMethods
//============================================================================

void PlayerAttack_3rdState::Enter(Player& player) {

	player.SetNextAnimation("player_attack_3rd", false, nextAnimDuration_);
	animationFinished_ = false;
}

void PlayerAttack_3rdState::Update(Player& player) {

	// animationが終わったかチェック
	animationFinished_ = player.IsAnimationFinished();

	// 座標、回転補間
	AttackAssist(player);
}

void PlayerAttack_3rdState::Exit([[maybe_unused]] Player& player) {

	// timerをリセット
	attackPosLerpTimer_ = 0.0f;
}

void PlayerAttack_3rdState::ImGui(const Player& player) {

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.001f);
	ImGui::DragFloat("rotationLerpRate", &rotationLerpRate_, 0.001f);

	ImGui::DragFloat("attackPosLerpCircleRange", &attackPosLerpCircleRange_, 0.1f);
	ImGui::DragFloat("attackLookAtCircleRange", &attackLookAtCircleRange_, 0.1f);
	ImGui::DragFloat("attackOffsetTranslation", &attackOffsetTranslation_, 0.1f);
	ImGui::DragFloat("attackPosLerpTime", &attackPosLerpTime_, 0.01f);
	Easing::SelectEasingType(attackPosEaseType_);

	DrawAttackOffset(player);
	DrawAttackRangeCircle(player, attackPosLerpCircleRange_);
	DrawAttackRangeCircle(player, attackLookAtCircleRange_);
}

void PlayerAttack_3rdState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
	rotationLerpRate_ = JsonAdapter::GetValue<float>(data, "rotationLerpRate_");
	attackPosLerpCircleRange_ = JsonAdapter::GetValue<float>(data, "attackPosLerpCircleRange_");
	attackLookAtCircleRange_ = JsonAdapter::GetValue<float>(data, "attackLookAtCircleRange_");
	attackOffsetTranslation_ = JsonAdapter::GetValue<float>(data, "attackOffsetTranslation_");
	attackPosLerpTime_ = JsonAdapter::GetValue<float>(data, "attackPosLerpTime_");
	attackPosEaseType_ = static_cast<EasingType>(JsonAdapter::GetValue<int>(data, "attackPosEaseType_"));
}

void PlayerAttack_3rdState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
	data["rotationLerpRate_"] = rotationLerpRate_;
	data["attackPosLerpCircleRange_"] = attackPosLerpCircleRange_;
	data["attackLookAtCircleRange_"] = attackLookAtCircleRange_;
	data["attackOffsetTranslation_"] = attackOffsetTranslation_;
	data["attackPosLerpTime_"] = attackPosLerpTime_;
	data["attackPosEaseType_"] = static_cast<int>(attackPosEaseType_);
}