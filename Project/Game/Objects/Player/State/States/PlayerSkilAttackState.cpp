#include "PlayerSkilAttackState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>
#include <Engine/Utility/GameTimer.h>
#include <Game/Objects/Player/Entity/Player.h>
#include <Game/Objects/Enemy/Boss/Entity/BossEnemy.h>

//============================================================================
//	PlayerSkilAttackState classMethods
//============================================================================

void PlayerSkilAttackState::Enter(Player& player) {

	player.SetNextAnimation("player_skilAttack", false, nextAnimDuration_);

	const Vector3 playerPos = player.GetTranslation();
	const Vector3 enemyPos = bossEnemy_->GetTranslation();
	// 向き
	Vector3 direction = (playerPos - enemyPos).Normalize();

	// 補間座標を設定する
	startPos_ = playerPos;
	targetPos_ = enemyPos + direction * moveDistance_;

	// 敵の方向を向かせる
	player.SetRotation(Quaternion::LookRotation((enemyPos - playerPos).Normalize(), Vector3(0.0f, 1.0f, 0.0f)));

	canExit_ = false;
	currentState_ = State::Approach;
}

void PlayerSkilAttackState::Update(Player& player) {

	// 状態の更新
	UpdateState(player);
}

void PlayerSkilAttackState::UpdateState(Player& player) {

	switch (currentState_) {
	case PlayerSkilAttackState::State::Approach: {

		// 座標を補間
		Vector3 translation = approach_.Update(startPos_, targetPos_);
		player.SetTranslation(translation);

		// 時間が経過しきったら次の状態に遷移させる
		if (approach_.time < approach_.timer) {

			currentState_ = State::Leave;
			player.SetTranslation(targetPos_);
		}
		break;
	}
	case PlayerSkilAttackState::State::Leave: {

		// 座標を補間
		Vector3 translation = leave_.Update(targetPos_, startPos_);
		player.SetTranslation(translation);

		// 時間が経過しきったら状態を終了させる
		if (leave_.time < leave_.timer) {

			canExit_ = true;
			player.SetTranslation(startPos_);
		}
		break;
	}
	}
}

Vector3 PlayerSkilAttackState::LerpParameter::Update(
	const Vector3& start, const Vector3& target) {

	// 時間を進める
	timer += GameTimer::GetScaledDeltaTime();
	float lerpT = timer / time;
	lerpT = EasedValue(easingType, lerpT);

	return Vector3::Lerp(start, target, lerpT);
}

void PlayerSkilAttackState::Exit(Player& player) {

	// リセット
	canExit_ = false;
	approach_.timer = 0.0f;
	leave_.timer = 0.0f;
	player.ResetAnimation();
}

void PlayerSkilAttackState::LerpParameter::ImGui(const std::string& label) {

	ImGui::SeparatorText(label.c_str());

	ImGui::DragFloat(("time##" + label).c_str(), &time, 0.01f);
	Easing::SelectEasingType(easingType, label);
}

void PlayerSkilAttackState::ImGui(const Player& player) {

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.001f);
	ImGui::DragFloat("rotationLerpRate", &rotationLerpRate_, 0.001f);
	ImGui::DragFloat("moveDistance", &moveDistance_, 0.1f);

	approach_.ImGui("approach");
	leave_.ImGui("leave");

	const Vector3 playerPos = player.GetTranslation();
	const Vector3 enemyPos = bossEnemy_->GetTranslation();
	// 向き
	Vector3 direction = (playerPos - enemyPos).Normalize();

	// 補間座標を設定する
	Vector3 startPos = playerPos;
	Vector3 targetPos = enemyPos + direction * moveDistance_;
	startPos.y = 4.0f;
	targetPos.y = 4.0f;

	LineRenderer::GetInstance()->DrawLine3D(
		startPos, targetPos, Color::Red());
}

void PlayerSkilAttackState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
	rotationLerpRate_ = JsonAdapter::GetValue<float>(data, "rotationLerpRate_");
	moveDistance_ = JsonAdapter::GetValue<float>(data, "moveDistance_");

	approach_.time = JsonAdapter::GetValue<float>(data, "approach_.time");
	approach_.easingType = static_cast<EasingType>(
		JsonAdapter::GetValue<int>(data, "approach_.easingType"));
	leave_.time = JsonAdapter::GetValue<float>(data, "leave_.time");
	leave_.easingType = static_cast<EasingType>(
		JsonAdapter::GetValue<int>(data, "leave_.easingType"));
}

void PlayerSkilAttackState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
	data["rotationLerpRate_"] = rotationLerpRate_;
	data["moveDistance_"] = moveDistance_;

	data["approach_.time"] = approach_.time;
	data["approach_.easingType"] = static_cast<int>(approach_.easingType);
	data["leave_.time"] = leave_.time;
	data["leave_.easingType"] = static_cast<int>(leave_.easingType);
}