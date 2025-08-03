#include "PlayerAttack_3rdState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>
#include <Engine/Utility/GameTimer.h>
#include <Game/Camera/Follow/FollowCamera.h>
#include <Game/Objects/Player/Entity/Player.h>

//============================================================================
//	PlayerAttack_3rdState classMethods
//============================================================================

PlayerAttack_3rdState::PlayerAttack_3rdState() {

	// effect作成
	groungEffect_ = std::make_unique<GameEffect>();
	groungEffect_->CreateParticleSystem("Particle/player3rdAttackGround.json");
}

void PlayerAttack_3rdState::Enter(Player& player) {

	player.SetNextAnimation("player_attack_3rd", false, nextAnimDuration_);
	canExit_ = false;
}

void PlayerAttack_3rdState::Update(Player& player) {

	// animationが終わったかチェック
	canExit_ = player.IsAnimationFinished();
	// animationが終わったら時間経過を進める
	if (canExit_) {

		exitTimer_ += GameTimer::GetScaledDeltaTime();
		if (!emitEffect_) {

			// playerの前方
			Vector3 forward = player.GetTransform().GetForward() * groundEffectDistance_;
			Matrix4x4 transMatrix = Matrix4x4::MakeIdentity4x4();
			Vector3 basePos = player.GetTranslation();
			// Y固定
			basePos.y = groundEffectPosY_;
			transMatrix = Matrix4x4::MakeTranslateMatrix(basePos + forward);

			// 発生させる
			groungEffect_->SetTransform(transMatrix);
			groungEffect_->Emit();

			// システム変更で消えた
			emitEffect_ = true;
		}

		// 画面シェイクを行わせる
		followCamera_->SetScreenShake(true);
	}

	// 座標、回転補間
	AttackAssist(player);
}

void PlayerAttack_3rdState::Exit([[maybe_unused]] Player& player) {

	// timerをリセット
	attackPosLerpTimer_ = 0.0f;
	exitTimer_ = 0.0f;
	emitEffect_ = false;
}

void PlayerAttack_3rdState::ImGui(const Player& player) {

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.001f);
	ImGui::DragFloat("rotationLerpRate", &rotationLerpRate_, 0.001f);
	ImGui::DragFloat("exitTime", &exitTime_, 0.01f);

	PlayerBaseAttackState::ImGui(player);

	ImGui::SeparatorText("Effect");

	ImGui::DragFloat("groundEffectDistance", &groundEffectDistance_, 0.01f);
	ImGui::DragFloat("groundEffectPosY", &groundEffectPosY_, 0.01f);

	LineRenderer::GetInstance()->DrawLine3D(player.GetTranslation(),
		player.GetTranslation() + player.GetTransform().GetForward() * groundEffectDistance_, Color::Red());
}

void PlayerAttack_3rdState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
	rotationLerpRate_ = JsonAdapter::GetValue<float>(data, "rotationLerpRate_");
	exitTime_ = JsonAdapter::GetValue<float>(data, "exitTime_");

	// この書き方でいい、GetValue関数はいらない
	groundEffectDistance_ = data.value("groundEffectDistance_", 2.0f);
	groundEffectPosY_ = data.value("groundEffectPosY_", 2.0f);

	PlayerBaseAttackState::ApplyJson(data);
}

void PlayerAttack_3rdState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
	data["rotationLerpRate_"] = rotationLerpRate_;
	data["exitTime_"] = exitTime_;
	data["groundEffectDistance_"] = groundEffectDistance_;
	data["groundEffectPosY_"] = groundEffectPosY_;

	PlayerBaseAttackState::SaveJson(data);
}

bool PlayerAttack_3rdState::GetCanExit() const {

	// 経過時間が過ぎたら
	bool canExit = exitTimer_ > exitTime_;
	return canExit;
}