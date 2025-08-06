#include "BossEnemyLightAttackState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>
#include <Engine/Utility/GameTimer.h>
#include <Game/Objects/GameScene/Player/Entity/Player.h>
#include <Game/Objects/GameScene/Enemy/Boss/Entity/BossEnemy.h>

//============================================================================
//	BossEnemyLightAttackState classMethods
//============================================================================

void BossEnemyLightAttackState::Enter(BossEnemy& bossEnemy) {

	// 座標を設定
	startPos_ = bossEnemy.GetTranslation();
	const Vector3 playerPos = player_->GetTranslation();
	Vector3 direction = (startPos_ - playerPos).Normalize();
	targetPos_ = playerPos - direction * attackOffsetTranslation_;

	// 開始時に一気playerの方を向かせる
	LookTarget(bossEnemy, targetPos_);

	canExit_ = false;
}

void BossEnemyLightAttackState::Update(BossEnemy& bossEnemy) {

	// 警告を出す
	if (bossEnemy.IsEventKey(0) || bossEnemy.IsEventKey(2)) {

		// 座標を設定して発生
		// システム変更で消えた

		++emitCount_;
	}

	lerpTimer_ += GameTimer::GetScaledDeltaTime();
	float lerpT = std::clamp(lerpTimer_ / lerpTime_, 0.0f, 1.0f);
	lerpT = EasedValue(easingType_, lerpT);

	// 座標補間
	bossEnemy.SetTranslation(Vector3::Lerp(startPos_, targetPos_, lerpT));

	// 時間経過が過ぎたらその座標にとどめる
	if (lerpTime_ < lerpTimer_ && !playAnimation_) {

		bossEnemy.SetTranslation(targetPos_);
		// ここでanimationを変更する
		bossEnemy.SetNextAnimation("bossEnemy_lightAttack", false, nextAnimDuration_);
		playAnimation_ = true;
	}

	// animationが終了したら経過時間を進める
	if (bossEnemy.IsAnimationFinished()) {

		exitTimer_ += GameTimer::GetDeltaTime();
		// 時間経過が過ぎたら遷移可能
		if (exitTime_ < exitTimer_) {

			canExit_ = true;
		}
	}
}

void BossEnemyLightAttackState::Exit([[maybe_unused]] BossEnemy& bossEnemy) {

	// リセット
	canExit_ = false;
	playAnimation_ = false;
	lerpTimer_ = 0.0f;
	exitTimer_ = 0.0f;
	emitCount_ = 0;
}

void BossEnemyLightAttackState::ImGui(const BossEnemy& bossEnemy) {

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.001f);
	ImGui::DragFloat("rotationLerpRate", &rotationLerpRate_, 0.001f);

	ImGui::DragFloat("attackOffsetTranslation", &attackOffsetTranslation_, 0.1f);
	ImGui::DragFloat("exitTime", &exitTime_, 0.01f);
	ImGui::DragFloat("lerpTime", &lerpTime_, 0.01f);

	ImGui::Text(std::format("canExit: {}", canExit_).c_str());
	ImGui::Text(std::format("emitCount: {}", emitCount_).c_str());
	ImGui::Text("exitTimer: %.3f", exitTimer_);
	ImGui::Text("lerpTime:  %.3f", lerpTime_);
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

void BossEnemyLightAttackState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
	rotationLerpRate_ = JsonAdapter::GetValue<float>(data, "rotationLerpRate_");

	attackOffsetTranslation_ = JsonAdapter::GetValue<float>(data, "attackOffsetTranslation_");
	exitTime_ = JsonAdapter::GetValue<float>(data, "exitTime_");
	lerpTime_ = JsonAdapter::GetValue<float>(data, "lerpTime_");
	easingType_ = static_cast<EasingType>(JsonAdapter::GetValue<int>(data, "easingType_"));
}

void BossEnemyLightAttackState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
	data["rotationLerpRate_"] = rotationLerpRate_;

	data["attackOffsetTranslation_"] = attackOffsetTranslation_;
	data["exitTime_"] = exitTime_;
	data["lerpTime_"] = lerpTime_;
	data["easingType_"] = static_cast<int>(easingType_);
}