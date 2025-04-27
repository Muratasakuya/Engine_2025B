#include "RightHandDashBehavior.h"

//============================================================================
//	include
//============================================================================
#include <Game/Object3D/Player/Parts/Base/BasePlayerParts.h>

//============================================================================
//  RightHandDashBehavior classMethods
//============================================================================

RightHandDashBehavior::RightHandDashBehavior(const Json& data) {

	rotationLerpValue_ = std::make_unique<SimpleAnimation<Vector3>>();
	if (data.contains(dashBehaviorJsonKey_) && data[dashBehaviorJsonKey_].contains("RotationLerpValue")) {

		rotationLerpValue_->FromJson(data[dashBehaviorJsonKey_]["RotationLerpValue"]);
	}
}

void RightHandDashBehavior::Execute(BasePlayerParts* parts) {

	if (!startRotation_.has_value()) {

		// lerpの初期値を設定
		startRotation_ = parts->GetTransform().rotation;

		// 方針
		// startRotation_を取得、この値をMakeAxis関数で動かした値まで遷移させたものをpartsに設定する
		rotationLerpValue_->Start();
		rotationLerpValue_->move_.start = Quaternion::ToEulerAngles(*startRotation_);
	}

	// 値を補間
	rotationLerpValue_->LerpValue(rotationAngle_);
	// 回転を計算
	Quaternion rotation = IPlayerBehavior::CalRotationAxisAngle(rotationAngle_);
	parts->SetRotate(rotation);
}

void RightHandDashBehavior::Reset() {

	// 初期化する
	rotationLerpValue_->Reset();
}

void RightHandDashBehavior::ImGui() {

	rotationLerpValue_->ImGui("RightHandDashBehavior_rotationLerpValue_");
}

void RightHandDashBehavior::SaveJson(Json& data) {

	// 値の保存
	rotationLerpValue_->ToJson(data[dashBehaviorJsonKey_]["RotationLerpValue"]);
}