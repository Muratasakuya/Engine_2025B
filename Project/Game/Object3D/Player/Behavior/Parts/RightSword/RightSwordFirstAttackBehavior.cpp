#include "RightSwordFirstAttackBehavior.h"

//============================================================================
//	include
//============================================================================
#include <Game/Object3D/Player/Parts/Base/BasePlayerParts.h>

//============================================================================
//  RightSwordFirstAttackBehavior classMethods
//============================================================================

RightSwordFirstAttackBehavior::RightSwordFirstAttackBehavior(const Json& data) {

	rotationLerpValue_ = std::make_unique<SimpleAnimation<Vector3>>();
	if (data.contains(attack1stBehaviorJsonKey_) && data[attack1stBehaviorJsonKey_].contains("RotationLerpValue")) {

		rotationLerpValue_->FromJson(data[attack1stBehaviorJsonKey_]["RotationLerpValue"]);
	}
}

void RightSwordFirstAttackBehavior::Execute([[maybe_unused]] BasePlayerParts* parts) {

	if (!rotationLerpValue_->IsStart()) {

		// animation開始
		rotationLerpValue_->Start();
		// 開始時の回転を記録
		startRotation_ = parts->GetTransform().rotation;
	}

	if (rotationLerpValue_->IsFinished()) {
		return;
	}

	// 値を補間
	Vector3 rotationAngle{};
	rotationLerpValue_->LerpValue(rotationAngle);
	// 回転を計算
	Quaternion deltaRotation = IPlayerBehavior::CalRotationAxisAngle(rotationAngle);
	Quaternion rotation = Quaternion::Normalize(deltaRotation);
	parts->SetRotate(rotation);
}

void RightSwordFirstAttackBehavior::Reset() {

	// 初期化する
	rotationLerpValue_->Reset();
}

void RightSwordFirstAttackBehavior::ImGui() {

	rotationLerpValue_->ImGui("RightSwordFirstAttackBehavior_rotationLerpValue_");
}

void RightSwordFirstAttackBehavior::SaveJson(Json& data) {

	// 値の保存
	rotationLerpValue_->ToJson(data[attack1stBehaviorJsonKey_]["RotationLerpValue"]);
}