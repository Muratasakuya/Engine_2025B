#include "PlayerParryState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>
#include <Engine/Utility/GameTimer.h>
#include <Game/Camera/Follow/FollowCamera.h>
#include <Game/Objects/GameScene/Enemy/Boss/Entity/BossEnemy.h>
#include <Game/Objects/GameScene/Player/Entity/Player.h>

//============================================================================
//	PlayerParryState classMethods
//============================================================================

PlayerParryState::PlayerParryState() {

	// effect作成
	parryEffect_ = std::make_unique<GameEffect>();
	parryEffect_->CreateParticleSystem("Particle/parryParticle.json");
}

void PlayerParryState::Enter(Player& player) {

	player.SetNextAnimation("player_parry", false, nextAnimDuration_);

	// 座標、向きを計算
	Vector3 direction = SetLerpValue(startPos_, targetPos_,
		player, parryLerp_.moveDistance, true);

	// 敵の方向を向かせる
	player.SetRotation(Quaternion::LookRotation(direction, Vector3(0.0f, 1.0f, 0.0f)));
	// 左手の武器を反転
	player.SetReverseWeapon(true, PlayerWeaponType::Left);

	// deltaTimeをスケーリングしても元の値に戻らないようにする
	GameTimer::SetReturnScaleEnable(false);
	GameTimer::SetTimeScale(0.0f);

	// カメラをパリィ状態に遷移させる
	followCamera_->SetParry(true);

	canExit_ = false;
	request_ = std::nullopt;
	parryLerp_.isFinised = false;
	attackLerp_.isFinised = false;
}

void PlayerParryState::Update(Player& player) {

	// deltaTimeの管理時間を更新
	UpdateDeltaWaitTime(player);

	// 入力状態を確認
	CheckInput();

	// 座標補間を更新
	UpdateLerpTranslation(player);

	// animationの更新
	UpdateAnimation(player);
}

void PlayerParryState::UpdateDeltaWaitTime(const Player& player) {

	// 時間経過を進める
	deltaWaitTimer_ += GameTimer::GetDeltaTime();
	// 時間経過が過ぎたら元に戻させる
	if (deltaWaitTime_ < deltaWaitTimer_) {

		GameTimer::SetReturnScaleEnable(true);
		
		// コマンドに設定
		ParticleCommand command{};
		// 座標設定
		command.target = ParticleCommandTarget::Spawner;
		command.id = ParticleCommandID::SetTranslation;
		command.value = player.GetJointWorldPos("leftHand");
		parryEffect_->SendCommand(command);

		// 発生させる
		parryEffect_->Emit(true);
	}
}

void PlayerParryState::UpdateLerpTranslation(Player& player) {

	if (parryLerp_.isFinised) {
		return;
	}

	// 座標を補間
	Vector3 translation = GetLerpTranslation(parryLerp_);

	// 座標を設定
	player.SetTranslation(translation);
}

void PlayerParryState::CheckInput() {

	if (request_.has_value()) {
		return;
	}

	// deltaTimeが元に戻った後どうするかを入力確認
	// 攻撃を押していたらanimationが終了した後攻撃に移る
	if (inputMapper_->IsTriggered(PlayerInputAction::Attack)) {

		request_ = RequestState::PlayAnimation;
	}
}

void PlayerParryState::UpdateAnimation(Player& player) {

	// 座標補間が終了するまでなにもしない
	if (!parryLerp_.isFinised) {
		return;
	}

	// 攻撃ボタンが押されていなければ状態を終了する
	if (!request_.has_value()) {

		canExit_ = true;
		// 元に戻す
		player.SetReverseWeapon(false, PlayerWeaponType::Left);
		return;
	}

	switch (request_.value()) {
	case RequestState::PlayAnimation: {

		// 3段目の攻撃を再生させる
		player.SetNextAnimation("player_attack_3rd", false, nextAnimDuration_);

		// 補間先の座標を再設定する
		SetLerpValue(startPos_, targetPos_,
			player, attackLerp_.moveDistance, false);

		request_ = RequestState::AttackAnimation;

		// カメラをパリィ攻撃用に遷移
		followCamera_->SetParry(false);
		followCamera_->SetParryAttack(true);

		// 元に戻す
		player.SetReverseWeapon(false, PlayerWeaponType::Left);
		break;
	}
	case RequestState::AttackAnimation: {

		// 座標補間を行う
		Vector3 translation = GetLerpTranslation(attackLerp_);

		// 座標を設定
		player.SetTranslation(translation);

		// animationが再生し終わったら状態を終了させる
		if (player.IsAnimationFinished()) {

			request_ = std::nullopt;

			// 画面シェイクを行わせる
			followCamera_->SetScreenShake(true);
		}
		break;
	}
	}
}

Vector3 PlayerParryState::GetLerpTranslation(LerpParameter& lerp) {

	// 時間を進める
	lerp.timer += GameTimer::GetScaledDeltaTime();
	float lerpT = lerp.timer / lerp.time;
	lerpT = EasedValue(lerp.easingType, lerpT);

	// 座標を補間
	Vector3 translation = Vector3::Lerp(startPos_, targetPos_, lerpT);

	if (lerp.time < lerp.timer) {

		lerp.isFinised = true;
	}

	return translation;
}

Vector3 PlayerParryState::SetLerpValue(Vector3& start, Vector3& target,
	const Player& player, float moveDistance, bool isPlayerBase) {

	const Vector3 playerPos = player.GetTranslation();
	const Vector3 enemyPos = bossEnemy_->GetTranslation();
	// 向き
	Vector3 direction = (enemyPos - playerPos).Normalize();

	// 補間座標を設定する
	start = playerPos;
	if (isPlayerBase) {

		target = playerPos + direction * moveDistance;
	} else {

		target = enemyPos + direction * moveDistance;
	}

	return direction;
}

void PlayerParryState::Exit([[maybe_unused]] Player& player) {

	// リセット
	request_ = std::nullopt;
	deltaWaitTimer_ = 0.0f;
	parryLerp_.timer = 0.0f;
	attackLerp_.timer = 0.0f;
	parryLerp_.isFinised = false;
	attackLerp_.isFinised = false;
	canExit_ = false;
	parryEffect_->ResetEmitFlag();
}

void PlayerParryState::ImGui(const Player& player) {

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.001f);
	ImGui::DragFloat("deltaWaitTime", &deltaWaitTime_, 0.01f);

	LineRenderer* lineRenderer = LineRenderer::GetInstance();

	ImGui::SeparatorText("Parry: RED");

	ImGui::DragFloat("time##Parry", &parryLerp_.time, 0.01f);
	ImGui::DragFloat("moveDistance##Parry", &parryLerp_.moveDistance, 0.1f);
	Easing::SelectEasingType(parryLerp_.easingType, "Parry");

	{
		Vector3 start{};
		Vector3 target{};
		Vector3 translation = SetLerpValue(start, target,
			player, parryLerp_.moveDistance, true);
		start.y = 2.0f;
		target.y = 2.0f;

		lineRenderer->DrawLine3D(
			start, target, Color::Red());
	}

	ImGui::SeparatorText("Attack: YELLOW");

	ImGui::DragFloat("time##Attack", &attackLerp_.time, 0.01f);
	ImGui::DragFloat("moveDistance##Attack", &attackLerp_.moveDistance, 0.1f);
	Easing::SelectEasingType(attackLerp_.easingType, "Attack");

	{
		Vector3 start{};
		Vector3 target{};
		Vector3 translation = SetLerpValue(start, target,
			player, attackLerp_.moveDistance, false);
		start.y = 2.0f;
		target.y = 2.0f;

		lineRenderer->DrawLine3D(
			start, target, Color(0.0f, 1.0f, 1.0f, 1.0f));
	}
}

void PlayerParryState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
	deltaWaitTime_ = JsonAdapter::GetValue<float>(data, "deltaWaitTime_");

	parryLerp_.time = JsonAdapter::GetValue<float>(data, "parryLerp_.time");
	parryLerp_.moveDistance = JsonAdapter::GetValue<float>(data, "parryLerp_.moveDistance");
	parryLerp_.easingType = static_cast<EasingType>(
		JsonAdapter::GetValue<int>(data, "parryLerp_.easingType"));

	attackLerp_.time = JsonAdapter::GetValue<float>(data, "attackLerp_.time");
	attackLerp_.moveDistance = JsonAdapter::GetValue<float>(data, "attackLerp_.moveDistance");
	attackLerp_.easingType = static_cast<EasingType>(
		JsonAdapter::GetValue<int>(data, "attackLerp_.easingType"));
}

void PlayerParryState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
	data["deltaWaitTime_"] = deltaWaitTime_;

	data["parryLerp_.time"] = parryLerp_.time;
	data["parryLerp_.moveDistance"] = parryLerp_.moveDistance;
	data["parryLerp_.easingType"] = parryLerp_.easingType;

	data["attackLerp_.time"] = attackLerp_.time;
	data["attackLerp_.moveDistance"] = attackLerp_.moveDistance;
	data["attackLerp_.easingType"] = static_cast<int>(attackLerp_.easingType);
}