#include "LeftSwordFirstAttackBehavior.h"

//============================================================================
//	include
//============================================================================
#include <Game/Object3D/Player/Parts/Base/BasePlayerParts.h>

//============================================================================
//  LeftSwordFirstAttackBehavior classMethods
//============================================================================

LeftSwordFirstAttackBehavior::LeftSwordFirstAttackBehavior(const Json& data) {

	rotationLerpValue_ = std::make_unique<SimpleAnimation<Vector3>>();
	if (data.contains(attack1stBehaviorJsonKey_) && data[attack1stBehaviorJsonKey_].contains("RotationLerpValue")) {

		rotationLerpValue_->FromJson(data[attack1stBehaviorJsonKey_]["RotationLerpValue"]);
	}
}

void LeftSwordFirstAttackBehavior::Execute(BasePlayerParts* parts) {

	if (!rotationLerpValue_->IsStart()) {

		// animation開始
		rotationLerpValue_->Start();
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

void LeftSwordFirstAttackBehavior::Reset() {

	// 初期化する
	rotationLerpValue_->Reset();
}

void LeftSwordFirstAttackBehavior::ImGui() {

	rotationLerpValue_->ImGui("LeftSwordFirstAttackBehavior_rotationLerpValue_");
}

void LeftSwordFirstAttackBehavior::SaveJson(Json& data) {

	// 値の保存
	rotationLerpValue_->ToJson(data[attack1stBehaviorJsonKey_]["RotationLerpValue"]);
}