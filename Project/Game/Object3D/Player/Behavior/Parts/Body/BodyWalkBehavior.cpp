#include "BodyWalkBehavior.h"

//============================================================================
//	include
//============================================================================
#include <Game/Camera/FollowCamera.h>
#include <Game/Object3D/Player/Parts/Base/BasePlayerParts.h>
#include <Engine/Particle/ParticleSystem.h>

//============================================================================
//	BodyWalkBehavior classMethods
//============================================================================

BodyWalkBehavior::BodyWalkBehavior(const Json& data, FollowCamera* followCamera) {

	followCamera_ = nullptr;
	followCamera_ = followCamera;

	if (data.contains(walkBehaviorJsonKey_)) {

		speed_ = JsonAdapter::GetValue<float>(data[walkBehaviorJsonKey_], "speed_");
		moveDecay_ = JsonAdapter::GetValue<float>(data[walkBehaviorJsonKey_], "moveDecay_");
		rotationLerpRate_ = JsonAdapter::GetValue<float>(data[walkBehaviorJsonKey_], "rotationLerpRate_");
	}
}

void BodyWalkBehavior::Execute(BasePlayerParts* parts) {

	// 歩き更新
	UpdateWalk(parts);
	// 回転、向いている方向を計算
	RotateToDirection(parts);

	// particleを発生させる
	ParticleSystem::GetInstance()->FrequencyEmit("TestEmitter");

	const auto& transform = parts->GetTransform();
	ParticleSystem::GetInstance()->SetTranslate("TestEmitter", transform.translation);
	ParticleSystem::GetInstance()->SetRotate("TestEmitter", transform.rotation);

	ParticleSystem::GetInstance()->UpdateEmitter("TestEmitter");
}

void BodyWalkBehavior::UpdateWalk(BasePlayerParts* parts) {

	Vector2 inputValue{};
	// inputの値を取得
	InputKey(inputValue);
	inputValue = Vector2::Normalize(inputValue);
	if (std::fabs(inputValue.x) > FLT_EPSILON || std::fabs(inputValue.y) > FLT_EPSILON) {

		// 入力がある場合のみ速度を計算する
		Vector3 inputDirection(inputValue.x, 0.0f, inputValue.y);
		inputDirection = Vector3::Normalize(inputDirection);

		Matrix4x4 rotateMatrix = Quaternion::MakeRotateMatrix(followCamera_->GetTransform().rotation);
		Vector3 rotatedDirection = Vector3::TransferNormal(inputDirection, rotateMatrix);
		rotatedDirection = Vector3::Normalize(rotatedDirection);

		move_ = rotatedDirection * speed_;
	} else {

		// 入力がなければどんどん減速させる
		move_ *= moveDecay_;
		// 一定の速度以下で止まる
		if (move_.Length() < 0.001f) {
			move_.Init();
		}
	}

	// 移動量を加算
	Vector3 translation = parts->GetTransform().translation;
	translation.x += move_.x;
	translation.z += move_.z;
	parts->SetTranslate(translation);
}

void BodyWalkBehavior::RotateToDirection(BasePlayerParts* parts) {

	Vector3 direction = Vector3(move_.x, 0.0f, move_.z).Normalize();

	if (direction.Length() <= 0.0f) {
		return;
	}

	// 向きを計算
	Quaternion targetRotation = Quaternion::LookRotation(direction, Vector3(0.0f, 1.0f, 0.0f));
	Quaternion rotation = parts->GetTransform().rotation;
	rotation = Quaternion::Slerp(rotation, targetRotation, rotationLerpRate_);
	parts->SetRotate(rotation);
}

void BodyWalkBehavior::Reset() {
}

void BodyWalkBehavior::ImGui() {

	ImGui::Text(std::format("moveX: {}", move_.x).c_str());
	ImGui::Text(std::format("moveZ: {}", move_.z).c_str());

	ImGui::PushItemWidth(itemWidth_);

	ImGui::DragFloat("speed##BodyDashBehavior", &speed_, 0.01f);
	ImGui::DragFloat("moveDecay##BodyDashBehavior", &moveDecay_, 0.01f);
	ImGui::DragFloat("rotationLerpRate##BodyDashBehavior", &rotationLerpRate_, 0.01f);

	ImGui::PopItemWidth();
}

void BodyWalkBehavior::SaveJson(Json& data) {

	// 値の保存
	data[walkBehaviorJsonKey_]["speed_"] = speed_;
	data[walkBehaviorJsonKey_]["moveDecay_"] = moveDecay_;
	data[walkBehaviorJsonKey_]["rotationLerpRate_"] = rotationLerpRate_;
}