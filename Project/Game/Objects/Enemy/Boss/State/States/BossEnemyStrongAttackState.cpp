#include "BossEnemyStrongAttackState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/GameTimer.h>
#include <Game/Objects/Player/Entity/Player.h>
#include <Game/Objects/Enemy/Boss/Entity/BossEnemy.h>

//============================================================================
//	BossEnemyStrongAttackState classMethods
//============================================================================

void BossEnemyStrongAttackState::Enter(BossEnemy& bossEnemy) {

	bossEnemy.SetNextAnimation("bossEnemy_idle", true, nextAnimDuration_);
}

void BossEnemyStrongAttackState::Update(BossEnemy& bossEnemy) {

	const float deltaTime = GameTimer::GetDeltaTime();

	// 前方ベクトルを取得
	Vector3 bossPos = bossEnemy.GetTranslation();
	Vector3 playerPos = player_->GetTransform().translation;

	// 回転を計算して設定
	Quaternion bossRotation = Quaternion::LookTarget(bossPos, playerPos,
		Vector3(0.0f, 1.0f, 0.0f), bossEnemy.GetRotation(), rotationLerpRate_ * deltaTime);
	bossEnemy.SetRotation(bossRotation);

	// 後ずさりさせる
	Vector3 backStepVelocity = bossEnemy.GetTransform().GetBack() * backStepSpeed_ * deltaTime;
	bossEnemy.SetTranslation(bossPos + backStepVelocity);
}

void BossEnemyStrongAttackState::Exit([[maybe_unused]] BossEnemy& bossEnemy) {
}

void BossEnemyStrongAttackState::ImGui([[maybe_unused]] const BossEnemy& bossEnemy) {

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.001f);
	ImGui::DragFloat("rotationLerpRate", &rotationLerpRate_, 0.001f);
	ImGui::DragFloat("backStepSpeed", &backStepSpeed_, 0.001f);
}

void BossEnemyStrongAttackState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
	rotationLerpRate_ = JsonAdapter::GetValue<float>(data, "rotationLerpRate_");
	backStepSpeed_ = JsonAdapter::GetValue<float>(data, "backStepSpeed_");
}

void BossEnemyStrongAttackState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
	data["rotationLerpRate_"] = rotationLerpRate_;
	data["backStepSpeed_"] = backStepSpeed_;
}