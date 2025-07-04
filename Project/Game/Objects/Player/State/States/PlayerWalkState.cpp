#include "PlayerWalkState.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>
#include <Engine/Utility/GameTimer.h>
#include <Game/Objects/Player/Entity/Player.h>
#include <Game/Camera/Follow/FollowCamera.h>

//============================================================================
//	PlayerWalkState classMethods
//============================================================================

void PlayerWalkState::Enter(Player& player) {

	player.SetNextAnimation("player_walk", true, nextAnimDuration_);
}

void PlayerWalkState::Update(Player& player) {

	// 歩き更新
	UpdateWalk(player);
	// 回転、進行方向に向かせる
	SetRotateToDirection(player, move_);
}

void PlayerWalkState::UpdateWalk(Player& player) {

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
	} else {

		// 入力がなければどんどん減速させる
		move_ *= moveDecay_;
		// 一定の速度以下で止まる
		if (move_.Length() < epsilon_) {
			move_.Init();
		}
	}

	// 移動量を加算
	Vector3 translation = player.GetTranslation();
	translation.x += move_.x;
	translation.z += move_.z;
	// 座標を制限する
	float clampSize = moveClampSize_ / 2.0f;
	translation.x = std::clamp(translation.x, -clampSize, clampSize);
	translation.z = std::clamp(translation.z, -clampSize, clampSize);
	player.SetTranslation(translation);
}

void PlayerWalkState::Exit([[maybe_unused]] Player& player) {
}

void PlayerWalkState::ImGui([[maybe_unused]] const Player& player) {

	ImGui::DragFloat("nextAnimDuration", &nextAnimDuration_, 0.001f);
	ImGui::DragFloat("rotationLerpRate", &rotationLerpRate_, 0.001f);
	ImGui::DragFloat("moveSpeed", &moveSpeed_, 0.01f);
	ImGui::DragFloat("moveDecay", &moveDecay_, 0.01f);
	ImGui::DragFloat("moveClampSize", &moveClampSize_, 1.0f);

	LineRenderer::GetInstance()->DrawSquare(moveClampSize_,
		Vector3(0.0f, 4.0f, 0.0f), Color::Red());
}

void PlayerWalkState::ApplyJson(const Json& data) {

	nextAnimDuration_ = JsonAdapter::GetValue<float>(data, "nextAnimDuration_");
	rotationLerpRate_ = JsonAdapter::GetValue<float>(data, "rotationLerpRate_");
	moveSpeed_ = JsonAdapter::GetValue<float>(data, "moveSpeed_");
	moveDecay_ = JsonAdapter::GetValue<float>(data, "moveDecay_");
	moveClampSize_ = JsonAdapter::GetValue<float>(data, "moveClampSize_");
}

void PlayerWalkState::SaveJson(Json& data) {

	data["nextAnimDuration_"] = nextAnimDuration_;
	data["rotationLerpRate_"] = rotationLerpRate_;
	data["moveSpeed_"] = moveSpeed_;
	data["moveDecay_"] = moveDecay_;
	data["moveClampSize_"] = moveClampSize_;
}