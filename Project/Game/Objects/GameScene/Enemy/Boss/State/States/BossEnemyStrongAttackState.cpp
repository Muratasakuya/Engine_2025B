#include "BossEnemyStrongAttackState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>
#include <Engine/Utility/GameTimer.h>
#include <Game/Objects/GameScene/Player/Entity/Player.h>
#include <Game/Objects/GameScene/Enemy/Boss/Entity/BossEnemy.h>

//============================================================================
//	BossEnemyStrongAttackState classMethods
//============================================================================

void BossEnemyStrongAttackState::Enter(BossEnemy& bossEnemy) {

	// 攻撃予兆アニメーションを設定
	bossEnemy.SetNextAnimation("bossEnemy_strongAttackParrySign", false, nextAnimDuration_);

	// 座標を設定
	startPos_ = bossEnemy.GetTranslation();
	canExit_ = false;
}

void BossEnemyStrongAttackState::Update(BossEnemy& bossEnemy) {

	// 状態に応じて更新
	switch (currentState_) {
	case BossEnemyStrongAttackState::State::ParrySign: {

		// プレイヤーの方を向きながら補間
		UpdateParrySign(bossEnemy);
		break;
	}
	case BossEnemyStrongAttackState::State::Attack: {

		// 攻撃、終了後状態を終了
		UpdateAttack(bossEnemy);
		break;
	}
	}
}

void BossEnemyStrongAttackState::UpdateParrySign(BossEnemy& bossEnemy) {

	// アニメーション終了時間で補間
	lerpTimer_ += GameTimer::GetScaledDeltaTime();
	float lerpT = std::clamp(lerpTimer_ / bossEnemy.GetAnimationDuration(
		"bossEnemy_strongAttackParrySign"), 0.0f, 1.0f);
	lerpT = EasedValue(easingType_, lerpT);

	// 目標座標を常に更新する
	const Vector3 playerPos = player_->GetTranslation();
	Vector3 direction = (bossEnemy.GetTranslation() - playerPos).Normalize();
	Vector3 target = playerPos - direction * attackOffsetTranslation_;
	LookTarget(bossEnemy, target);

	// 座標補間
	bossEnemy.SetTranslation(Vector3::Lerp(startPos_, target, lerpT));

	// アニメーションが終了次第攻撃する
	if (bossEnemy.IsAnimationFinished()) {

		bossEnemy.SetTranslation(target);
		// ここでanimationを変更する
		bossEnemy.SetNextAnimation("bossEnemy_strongAttack", false, nextAnimDuration_);

		// 状態を進める
		currentState_ = State::Attack;
	}
}

void BossEnemyStrongAttackState::UpdateAttack(BossEnemy& bossEnemy) {

	// animationが終了したら経過時間を進める
	if (bossEnemy.IsAnimationFinished()) {

		exitTimer_ += GameTimer::GetDeltaTime();
		// 時間経過が過ぎたら遷移可能
		if (exitTime_ < exitTimer_) {

			canExit_ = true;
		}
	}
}

void BossEnemyStrongAttackState::Exit([[maybe_unused]] BossEnemy& bossEnemy) {

	// リセット
	canExit_ = false;
	lerpTimer_ = 0.0f;
	exitTimer_ = 0.0f;
	currentState_ = State::ParrySign;
}

void BossEnemyStrongAttackState::ImGui([[maybe_unused]] const BossEnemy& bossEnemy) {

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.001f);
	ImGui::DragFloat("rotationLerpRate", &rotationLerpRate_, 0.001f);

	ImGui::DragFloat("attackOffsetTranslation", &attackOffsetTranslation_, 0.1f);
	ImGui::DragFloat("exitTime", &exitTime_, 0.01f);

	ImGui::Text(std::format("canExit: {}", canExit_).c_str());
	ImGui::Text("exitTimer: %.3f", exitTimer_);
	Easing::SelectEasingType(easingType_);

	// 座標を設定
	// 座標を設定
	Vector3 start = bossEnemy.GetTranslation();
	const Vector3 playerPos = player_->GetTranslation();
	Vector3 direction = (start - playerPos).Normalize();
	Vector3 target = playerPos - direction * attackOffsetTranslation_;
	target.y = 2.0f;
	LineRenderer::GetInstance()->DrawSphere(8, 2.0f, target, Color::Red());
}

void BossEnemyStrongAttackState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
	rotationLerpRate_ = JsonAdapter::GetValue<float>(data, "rotationLerpRate_");

	attackOffsetTranslation_ = JsonAdapter::GetValue<float>(data, "attackOffsetTranslation_");
	exitTime_ = JsonAdapter::GetValue<float>(data, "exitTime_");
	easingType_ = static_cast<EasingType>(JsonAdapter::GetValue<int>(data, "easingType_"));
}

void BossEnemyStrongAttackState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
	data["rotationLerpRate_"] = rotationLerpRate_;

	data["attackOffsetTranslation_"] = attackOffsetTranslation_;
	data["exitTime_"] = exitTime_;
	data["easingType_"] = static_cast<int>(easingType_);
}