#include "BossEnemyChargeAttackState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/GameTimer.h>
#include <Game/Objects/Player/Entity/Player.h>
#include <Game/Objects/Enemy/Boss/Entity/BossEnemy.h>

//============================================================================
//	BossEnemyChargeAttackState classMethods
//============================================================================

void BossEnemyChargeAttackState::Enter(BossEnemy& bossEnemy) {

	bossEnemy.SetNextAnimation("bossEnemy_chargeAttack", false, nextAnimDuration_);

	Vector3 startPos = bossEnemy.GetTranslation();
	Vector3 targetPos = player_->GetTranslation();

	// 開始時に一気playerの方を向かせる
	LookTarget(bossEnemy, targetPos);

	canExit_ = false;
}

void BossEnemyChargeAttackState::Update(BossEnemy& bossEnemy) {

	// 警告を出す
	if (bossEnemy.IsEventKey(0)) {

		// 座標を設定して発生
		// システム変更で消えた
	}

	if (bossEnemy.IsAnimationFinished()) {

		exitTimer_ += GameTimer::GetDeltaTime();
		// 時間経過が過ぎたら遷移可能
		if (exitTime_ < exitTimer_) {

			canExit_ = true;
		}
	}
}

void BossEnemyChargeAttackState::Exit([[maybe_unused]] BossEnemy& bossEnemy) {

	// リセット
	exitTimer_ = 0.0f;
	canExit_ = false;
}

void BossEnemyChargeAttackState::ImGui([[maybe_unused]] const BossEnemy& bossEnemy) {

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.001f);
	ImGui::DragFloat("rotationLerpRate", &rotationLerpRate_, 0.001f);

	ImGui::DragFloat("exitTime", &exitTime_, 0.01f);
}

void BossEnemyChargeAttackState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
	rotationLerpRate_ = JsonAdapter::GetValue<float>(data, "rotationLerpRate_");

	exitTime_ = JsonAdapter::GetValue<float>(data, "exitTime_");
}

void BossEnemyChargeAttackState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
	data["rotationLerpRate_"] = rotationLerpRate_;

	data["exitTime_"] = exitTime_;
}