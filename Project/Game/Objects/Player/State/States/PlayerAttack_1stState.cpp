#include "PlayerAttack_1stState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/GameTimer.h>
#include <Game/Objects/Player/Entity/Player.h>

//============================================================================
//	PlayerAttack_1stState classMethods
//============================================================================

PlayerAttack_1stState::PlayerAttack_1stState() {

	// effect作成
	slashEffect_ = std::make_unique<GameEffect>();
	slashEffect_->CreateParticleSystem("Particle/playerSlash.json");
}

void PlayerAttack_1stState::Enter(Player& player) {

	player.SetNextAnimation("player_attack_1st", false, nextAnimDuration_);
	canExit_ = false;
}

void PlayerAttack_1stState::Update(Player& player) {

	// animationが終わったかチェック
	canExit_ = player.IsAnimationFinished();
	// animationが終わったら時間経過を進める
	if (canExit_) {

		exitTimer_ += GameTimer::GetScaledDeltaTime();
	}

	// 座標、回転補間
	AttackAssist(player);

	// オフセット計算して設定
	Matrix4x4 offsetMatrix = PlayerBaseAttackState::GetEffectOffsetMatrix(
		player, slashEffectRotation_, slashEffectTranslaton_);
	slashEffect_->SetTransform(offsetMatrix);
	// 発生させる
	slashEffect_->Emit(true);
}

void PlayerAttack_1stState::Exit([[maybe_unused]] Player& player) {

	// リセット
	attackPosLerpTimer_ = 0.0f;
	exitTimer_ = 0.0f;
	slashEffect_->ResetEmitFlag();
}

void PlayerAttack_1stState::ImGui(const Player& player) {

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.001f);
	ImGui::DragFloat("rotationLerpRate", &rotationLerpRate_, 0.001f);
	ImGui::DragFloat("exitTime", &exitTime_, 0.01f);

	PlayerBaseAttackState::ImGui(player);

	ImGui::SeparatorText("Effect");

	ImGui::DragFloat3("slashEffectRotation", &slashEffectRotation_.x, 0.01f);
	ImGui::DragFloat3("slashEffectTranslaton", &slashEffectTranslaton_.x, 0.01f);
}

void PlayerAttack_1stState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
	rotationLerpRate_ = JsonAdapter::GetValue<float>(data, "rotationLerpRate_");
	exitTime_ = JsonAdapter::GetValue<float>(data, "exitTime_");

	if (data.contains("slashEffectRotation_")) {

		slashEffectRotation_ = slashEffectRotation_.FromJson(data["slashEffectRotation_"]);
		slashEffectTranslaton_ = slashEffectTranslaton_.FromJson(data["slashEffectTranslaton_"]);
	}

	PlayerBaseAttackState::ApplyJson(data);
}

void PlayerAttack_1stState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
	data["rotationLerpRate_"] = rotationLerpRate_;
	data["exitTime_"] = exitTime_;

	data["slashEffectRotation_"] = slashEffectRotation_.ToJson();
	data["slashEffectTranslaton_"] = slashEffectTranslaton_.ToJson();

	PlayerBaseAttackState::SaveJson(data);
}

bool PlayerAttack_1stState::GetCanExit() const {

	// 経過時間が過ぎたら
	bool canExit = exitTimer_ > exitTime_;
	return canExit;
}