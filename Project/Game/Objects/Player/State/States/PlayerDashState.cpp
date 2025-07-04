#include "PlayerDashState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>
#include <Engine/Utility/GameTimer.h>
#include <Game/Objects/Player/Entity/Player.h>
#include <Game/Camera/Follow/FollowCamera.h>

//============================================================================
//	PlayerDashState classMethods
//============================================================================

void PlayerDashState::Enter(Player& player) {

	player.SetNextAnimation("player_dash", true, nextAnimDuration_);
}

void PlayerDashState::Update(Player& player) {

	// ダッシュ更新
	UpdateDash(player);
	// 回転、進行方向に向かせる
	SetRotateToDirection(player, move_);
}

void PlayerDashState::UpdateDash(Player& player) {

	// 補間処理はダッシュ中のみ
	if (!speedLerpValue_->IsStart()) {

		// 補間処理を開始
		moveSpeed_ = speedLerpValue_->move_.start;
		speedLerpValue_->Start();
	}
	// 値を補間
	speedLerpValue_->LerpValue(moveSpeed_);

	// 入力値取得
	Vector2 inputValue(inputMapper_->GetVector(PlayerAction::MoveX),
		inputMapper_->GetVector(PlayerAction::MoveZ));
	inputValue = Vector2::Normalize(inputValue);

	if (std::fabs(inputValue.x) > epsilon_ || std::fabs(inputValue.y) > epsilon_) {

		// 入力がある場合のみ速度を計算する
		Vector3 inputDirection(inputValue.x, 0.0f, inputValue.y);
		inputDirection = Vector3::Normalize(inputDirection);

		Matrix4x4 rotateMatrix = Quaternion::MakeRotateMatrix(followCamera_->GetTransform().rotation);
		Vector3 rotatedDirection = Vector3::TransferNormal(inputDirection, rotateMatrix);
		rotatedDirection = Vector3::Normalize(rotatedDirection);

		move_ = rotatedDirection * moveSpeed_;
	}

	// 移動量を加算
	Vector3 translation = player.GetTranslation();
	translation.x += move_.x;
	translation.z += move_.z;
	// 座標を制限する
	float clampSize = moveClampSize_ / 2.0f;
	translation.x = std::clamp(translation.x, -clampSize, clampSize);
	translation.z = std::clamp(translation.z, -clampSize, clampSize);.
	player.SetTranslation(translation);
}

void PlayerDashState::Exit([[maybe_unused]] Player& player) {

	// animationをリセット
	speedLerpValue_->Reset();
	move_.Init();
}

bool PlayerDashState::GetCanExit() const {

	if (inputMapper_->IsTriggered(PlayerAction::Attack)) {

		return true;
	}

	return !inputMapper_->IsPressed(PlayerAction::Dash);
}

void PlayerDashState::ImGui([[maybe_unused]] const Player& player) {

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.001f);
	ImGui::DragFloat("rotationLerpRate_", &rotationLerpRate_, 0.001f);
	ImGui::DragFloat("moveClampSize", &moveClampSize_, 1.0f);

	speedLerpValue_->ImGui("speedLerpValue");

	LineRenderer::GetInstance()->DrawSquare(moveClampSize_,
		Vector3(0.0f, 4.0f, 0.0f), Color::Red());
}

void PlayerDashState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
	rotationLerpRate_ = JsonAdapter::GetValue<float>(data, "rotationLerpRate_");
	moveClampSize_ = JsonAdapter::GetValue<float>(data, "moveClampSize_");

	speedLerpValue_ = std::make_unique<SimpleAnimation<float>>();
	if (data.contains("speedLerpValue_")) {
		speedLerpValue_->FromJson(data["speedLerpValue_"]);
	}
}

void PlayerDashState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
	data["rotationLerpRate_"] = rotationLerpRate_;
	data["moveClampSize_"] = moveClampSize_;
	speedLerpValue_->ToJson(data["speedLerpValue_"]);
}