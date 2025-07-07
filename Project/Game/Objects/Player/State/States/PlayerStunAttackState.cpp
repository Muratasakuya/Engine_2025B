#include "PlayerStunAttackState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/GameTimer.h>
#include <Game/Camera/Follow/FollowCamera.h>
#include <Game/Objects/Enemy/Boss/Entity/BossEnemy.h>
#include <Game/Objects/Player/Entity/Player.h>

//============================================================================
//	PlayerStunAttackState classMethods
//============================================================================

PlayerStunAttackState::PlayerStunAttackState() {

	// 味方を作成
	ally_ = std::make_unique<GameEntity3D>();
	ally_->Init("cube", "playerAlly", "Player");

	canExit_ = false;
}

void PlayerStunAttackState::Enter([[maybe_unused]] Player& player) {

	// 座標取得
	const Vector3 enemyPos = bossEnemy_->GetTranslation();

	// 敵との距離分離した位置を補間開始座標とする
	Vector3 toCamera = followCamera_->GetTransform().translation - enemyPos;
	toCamera.y = 0.0f;
	toCamera = toCamera.Normalize();
	rushStartAllyTranslation_ = enemyPos + toCamera * enemyDistance_;
	rushStartAllyTranslation_.y = targetTranslationY_;
	// 目標座標は敵の座標
	rushTargetAllyTranslation_ = enemyPos;
	rushTargetAllyTranslation_.y = targetTranslationY_;
}

void PlayerStunAttackState::Update(Player& player) {

	// 状態に応じて更新
	switch (currentState_) {
	case PlayerStunAttackState::State::AllyEntry: {

		UpdateAllyEntry(player);
		break;
	}
	case PlayerStunAttackState::State::AllyRushAttack: {

		UpdateAllyRushAttack(player);
		break;
	}
	case PlayerStunAttackState::State::PlayerAttack: {

		UpdatePlayerAttack(player);
		break;
	}
	}
}

void PlayerStunAttackState::UpdateAllyEntry(Player& player) {

	// 時間経過を進める
	entryTimer_ += GameTimer::GetDeltaTime();
	float lerpT = entryTimer_ / entryTime_;
	lerpT = EasedValue(entryEasingType_, lerpT);

	// 座標を設定
	Vector3 allyTranslation = ally_->GetTranslation();
	// Y座標を補間
	allyTranslation.y = std::lerp(0.0f, targetTranslationY_, lerpT);
	ally_->SetTranslation(allyTranslation);

	// playerの表示を消す(とりあえずα値を下げる)
	player.SetAlpha(std::clamp(1.0f - lerpT, 0.0f, 1.0f));

	if (1.0f <= lerpT) {

		// 次の状態に遷移させる
		currentState_ = State::AllyRushAttack;
		// このtimerは再利用するためリセット
		entryTimer_ = 0.0f;
	}
}

void PlayerStunAttackState::UpdateAllyRushAttack(Player& player) {

	// 時間経過を進める
	rushTimer_ += GameTimer::GetDeltaTime();
	float lerpT = rushTimer_ / rushTime_;
	lerpT = EasedValue(rushEasingType_, lerpT);

	// 座標を補間して設定
	Vector3 translation = Vector3::Lerp(rushStartAllyTranslation_,
		rushTargetAllyTranslation_, lerpT);
	ally_->SetTranslation(translation);

	if (1.0f <= lerpT) {

		// 次の状態に遷移させる
		currentState_ = State::PlayerAttack;

		// この瞬間に次のanimationに切り替える
		player.SetNextAnimation("player_stunAttack", false, nextAnimDuration_);

		// 一瞬スローモーションにするためにdeltaTimeをスケーリングさせる
		GameTimer::SetTimeScale(0.0f);
	}
}

void PlayerStunAttackState::UpdatePlayerAttack(Player& player) {

	// 時間経過を進める
	entryTimer_ += GameTimer::GetDeltaTime();
	float lerpT = entryTimer_ / entryTime_;
	lerpT = EasedValue(entryEasingType_, lerpT);

	// playerを出現させる
	player.SetAlpha(std::clamp(lerpT, 0.0f, 1.0f));

	// animationが終了次第遷移可能状態にする
	if (player.IsAnimationFinished()) {

		canExit_ = true;
	}
}

void PlayerStunAttackState::Exit([[maybe_unused]] Player& player) {

	// リセット
	entryTimer_ = 0.0f;
	rushTimer_ = 0.0f;
	canExit_ = false;
}

void PlayerStunAttackState::ImGui([[maybe_unused]] const Player& player) {

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.01f);
	ImGui::DragFloat("entryTime", &entryTime_, 0.01f);
	ImGui::DragFloat("targetTranslationY", &targetTranslationY_, 0.01f);
	ImGui::DragFloat("enemyDistance", &enemyDistance_, 0.01f);
	Easing::SelectEasingType(entryEasingType_, "entryEasingType");
	ImGui::DragFloat("rushTime", &rushTime_, 0.01f);
	Easing::SelectEasingType(rushEasingType_, "rushEasingType_");
}

void PlayerStunAttackState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
	entryTime_ = JsonAdapter::GetValue<float>(data, "entryTime_");
	targetTranslationY_ = JsonAdapter::GetValue<float>(data, "targetTranslationY_");
	enemyDistance_ = JsonAdapter::GetValue<float>(data, "enemyDistance_");
	rushTime_ = JsonAdapter::GetValue<float>(data, "rushTime_");
	entryEasingType_ = static_cast<EasingType>(
		JsonAdapter::GetValue<int>(data, "entryEasingType_"));
	rushEasingType_ = static_cast<EasingType>(
		JsonAdapter::GetValue<int>(data, "rushEasingType_"));
}

void PlayerStunAttackState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
	data["entryTime_"] = entryTime_;
	data["targetTranslationY_"] = targetTranslationY_;
	data["enemyDistance_"] = enemyDistance_;
	data["rushTime_"] = rushTime_;
	data["entryEasingType_"] = static_cast<int>(entryEasingType_);
	data["rushEasingType_"] = static_cast<int>(rushEasingType_);
}