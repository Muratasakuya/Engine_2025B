#include "BossEnemyTeleportationState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/GameTimer.h>
#include <Game/Camera/FollowCamera.h>
#include <Game/Objects/Player/Entity/Player.h>
#include <Game/Objects/Enemy/Boss/Entity/BossEnemy.h>

//============================================================================
//	BossEnemyTeleportationState classMethods
//============================================================================

void BossEnemyTeleportationState::Enter(BossEnemy& bossEnemy) {

	bossEnemy.SetNextAnimation("bossEnemy_teleport", true, nextAnimDuration_);

	// 座標を設定
	const Vector3 center = player_->GetTranslation();
	const Vector3 forward = followCamera_->GetTransform().GetForward();
	startPos_ = bossEnemy.GetTranslation();
	// 弧上の座標を取得
	if (type_ == BossEnemyTeleportType::Far) {

		targetPos_ = Math::RandomPointOnArc(center, forward, farRadius_, halfAngle_);
	} else if (type_ == BossEnemyTeleportType::Near) {

		targetPos_ = Math::RandomPointOnArc(center, forward, nearRadius_, halfAngle_);
	}

	canExit_ = false;
}

void BossEnemyTeleportationState::Update(BossEnemy& bossEnemy) {

	lerpTimer_ += GameTimer::GetScaledDeltaTime();
	float lerpT = std::clamp(lerpTimer_ / lerpTime_, 0.0f, 1.0f);
	lerpT = EasedValue(easingType_, lerpT);

	// 座標補間
	bossEnemy.SetTranslation(Vector3::Lerp(startPos_, targetPos_, lerpT));

	// playerの方を向かせる
	LookTarget(bossEnemy, player_->GetTranslation());

	// 時間経過が過ぎたら状態遷移可能にする
	if (lerpTime_ < lerpTimer_) {

		bossEnemy.SetTranslation(targetPos_);
		canExit_ = true;
	}
}

void BossEnemyTeleportationState::Exit([[maybe_unused]] BossEnemy& bossEnemy) {

	// リセット
	canExit_ = false;
	lerpTimer_ = 0.0f;
}

void BossEnemyTeleportationState::ImGui([[maybe_unused]] const BossEnemy& bossEnemy) {

	// テレポートの種類の名前
	const char* teleportNames[] = { "Far","Near" };
	ImGui::Text("currentTeleportType: %s", teleportNames[static_cast<int>(type_)]);

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.001f);
	ImGui::DragFloat("rotationLerpRate", &rotationLerpRate_, 0.001f);

	ImGui::DragFloat("farRadius:Red", &farRadius_, 0.1f);
	ImGui::DragFloat("nearRadius:Blue", &nearRadius_, 0.1f);
	ImGui::DragFloat("halfAngle", &halfAngle_, 0.1f);
	ImGui::DragFloat("lerpTime", &lerpTime_, 0.01f);
	Easing::SelectEasingType(easingType_);

	DrawArc(player_->GetTranslation(), followCamera_->GetTransform().GetForward(),
		farRadius_, halfAngle_, Color::Red());
	DrawArc(player_->GetTranslation(), followCamera_->GetTransform().GetForward(),
		nearRadius_, halfAngle_, Color::Blue());
}

void BossEnemyTeleportationState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
	rotationLerpRate_ = JsonAdapter::GetValue<float>(data, "rotationLerpRate_");
	farRadius_ = JsonAdapter::GetValue<float>(data, "farRadius_");
	nearRadius_ = JsonAdapter::GetValue<float>(data, "nearRadius_");
	halfAngle_ = JsonAdapter::GetValue<float>(data, "halfAngle_");
	lerpTime_ = JsonAdapter::GetValue<float>(data, "lerpTime_");
	easingType_ = static_cast<EasingType>(JsonAdapter::GetValue<int>(data, "easingType_"));
}

void BossEnemyTeleportationState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
	data["rotationLerpRate_"] = rotationLerpRate_;
	data["farRadius_"] = farRadius_;
	data["nearRadius_"] = nearRadius_;
	data["halfAngle_"] = halfAngle_;
	data["lerpTime_"] = lerpTime_;
	data["easingType_"] = static_cast<int>(easingType_);
}