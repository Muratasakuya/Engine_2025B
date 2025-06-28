#include "PlayerBaseAttackState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>
#include <Game/Objects/Enemy/Boss/Entity/BossEnemy.h>
#include <Game/Objects/Player/Entity/Player.h>
#include <Engine/Utility/GameTimer.h>

//============================================================================
//	PlayerBaseAttackState classMethods
//============================================================================

void PlayerBaseAttackState::AttackAssist(Player& player) {

	// 時間経過
	attackPosLerpTimer_ += GameTimer::GetScaledDeltaTime();
	float lerpT = std::clamp(attackPosLerpTimer_ / attackPosLerpTime_, 0.0f, 1.0f);
	lerpT = EasedValue(attackPosEaseType_, lerpT);

	// 座標、距離を取得
	const Vector3 playerPos = player.GetTranslation();
	const Vector3 enemyPos = bossEnemy_->GetTranslation();
	const float distance = (enemyPos - playerPos).Length();

	// 指定円の中に敵がいれば座標を補完する
	Vector3 direction = (enemyPos - playerPos).Normalize();
	if (attackPosLerpCircleRange_ > epsilon_ && distance <= attackPosLerpCircleRange_) {

		// 補間先
		Vector3 target = enemyPos - direction * attackOffsetTranslation_;
		Vector3 translation = Vector3::Lerp(playerPos, target, std::clamp(lerpT, 0.0f, 1.0f));
		player.SetTranslation(translation);
	}

	// 指定円の中に敵がいれば敵の方向に向かせる
	if (attackLookAtCircleRange_ > epsilon_ && distance <= attackLookAtCircleRange_) {

		Quaternion currentRotation = player.GetRotation();
		Quaternion targetRotation = Quaternion::LookRotation(direction, Vector3(0.0f, 1.0f, 0.0f));
		player.SetRotation(Quaternion::Slerp(currentRotation, targetRotation, std::clamp(rotationLerpRate_, 0.0f, 1.0f)));
	}
}

void PlayerBaseAttackState::DrawAttackOffset(const Player& player) {

	LineRenderer* lineRenderer = LineRenderer::GetInstance();

	// 座標、距離を取得
	Vector3 playerPos = player.GetTranslation();
	playerPos.y = 2.0f;
	Vector3 enemyPos = bossEnemy_->GetTranslation();
	enemyPos.y = 2.0f;
	Vector3 direction = (enemyPos - playerPos).Normalize();
	Vector3 target = enemyPos - direction * attackOffsetTranslation_;

	lineRenderer->DrawLine3D(playerPos, target, Color::Red());
	lineRenderer->DrawSphere(8, 2.0f, target, Color::Red());
}

void PlayerBaseAttackState::DrawAttackRangeCircle(const Player& player, float range) {

	LineRenderer* lineRenderer = LineRenderer::GetInstance();
	const Vector3 playerPos = player.GetTranslation();

	const int segments = 32;
	for (int i = 0; i < segments; ++i) {

		float a0 = 2.0f * std::numbers::pi_v<float> *i / segments;
		float a1 = 2.0f * std::numbers::pi_v<float> *(i + 1) / segments;

		Vector3 p0(playerPos.x + range * std::cos(a0), 2.0f, playerPos.z + range * std::sin(a0));
		Vector3 p1(playerPos.x + range * std::cos(a1), 2.0f, playerPos.z + range * std::sin(a1));

		lineRenderer->DrawLine3D(p0, p1, Color::Red());
	}
}

void PlayerBaseAttackState::ImGui(const Player& player) {

	ImGui::DragFloat("attackPosLerpCircleRange", &attackPosLerpCircleRange_, 0.1f);
	ImGui::DragFloat("attackLookAtCircleRange", &attackLookAtCircleRange_, 0.1f);
	ImGui::DragFloat("attackOffsetTranslation", &attackOffsetTranslation_, 0.1f);
	ImGui::DragFloat("attackPosLerpTime", &attackPosLerpTime_, 0.01f);
	Easing::SelectEasingType(attackPosEaseType_);

	DrawAttackOffset(player);
	DrawAttackRangeCircle(player, attackPosLerpCircleRange_);
	DrawAttackRangeCircle(player, attackLookAtCircleRange_);
}

void PlayerBaseAttackState::ApplyJson(const Json& data) {

	attackPosLerpCircleRange_ = JsonAdapter::GetValue<float>(data, "attackPosLerpCircleRange_");
	attackLookAtCircleRange_ = JsonAdapter::GetValue<float>(data, "attackLookAtCircleRange_");
	attackOffsetTranslation_ = JsonAdapter::GetValue<float>(data, "attackOffsetTranslation_");
	attackPosLerpTime_ = JsonAdapter::GetValue<float>(data, "attackPosLerpTime_");
	attackPosEaseType_ = static_cast<EasingType>(JsonAdapter::GetValue<int>(data, "attackPosEaseType_"));
}

void PlayerBaseAttackState::SaveJson(Json& data) {

	data["attackPosLerpCircleRange_"] = attackPosLerpCircleRange_;
	data["attackLookAtCircleRange_"] = attackLookAtCircleRange_;
	data["attackOffsetTranslation_"] = attackOffsetTranslation_;
	data["attackPosLerpTime_"] = attackPosLerpTime_;
	data["attackPosEaseType_"] = static_cast<int>(attackPosEaseType_);
}