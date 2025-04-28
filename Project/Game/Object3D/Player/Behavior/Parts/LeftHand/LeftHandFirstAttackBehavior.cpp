#include "LeftHandFirstAttackBehavior.h"

//============================================================================
//	include
//============================================================================
#include <Game/Object3D/Player/Parts/Base/BasePlayerParts.h>

//============================================================================
//  LeftHandFirstAttackBehavior classMethods
//============================================================================

LeftHandFirstAttackBehavior::LeftHandFirstAttackBehavior(const Json& data) {

	backLeftRotation_ = std::make_unique<SimpleAnimation<Vector3>>();
	forwardSlash_ = std::make_unique<SimpleAnimation<Vector3>>();
	if (data.contains(attack1stBehaviorJsonKey_) && data[attack1stBehaviorJsonKey_].contains("BackLeftRotation")) {

		backLeftRotation_->FromJson(data[attack1stBehaviorJsonKey_]["BackLeftRotation"]);
		forwardSlash_->FromJson(data[attack1stBehaviorJsonKey_]["ForwardSlash"]);
	}
}

void LeftHandFirstAttackBehavior::Execute([[maybe_unused]] BasePlayerParts* parts) {

	// 左後ろに回転
	UpdateBackLeftRotation(parts);
	// 左後ろに回転したあと前に斬りかかる
	UpdateForwardSlash(parts);
}

void LeftHandFirstAttackBehavior::UpdateBackLeftRotation(BasePlayerParts* parts) {

	if (!backLeftRotation_->IsStart()) {

		// animation開始
		backLeftRotation_->Start();
		// 開始時の回転を記録
		forwardSlash_->move_.start = rotationAngle_;
		startRotation_ = parts->GetTransform().rotation;
	}

	// 値を補間
	backLeftRotation_->LerpValue(rotationAngle_);
	// 回転を計算
	Quaternion deltaRotation = IPlayerBehavior::CalRotationAxisAngle(rotationAngle_);
	Quaternion rotation = Quaternion::Normalize(startRotation_ * deltaRotation);
	parts->SetRotate(rotation);
}

void LeftHandFirstAttackBehavior::UpdateForwardSlash(BasePlayerParts* parts) {

	// 左後ろに回転が終わったら
	if (backLeftRotation_->IsFinished()) {
		if (!forwardSlash_->IsStart()) {

			// animation開始
			forwardSlash_->move_.start = rotationAngle_;
			forwardSlash_->Start();
		}

		// 値を補間
		forwardSlash_->LerpValue(rotationAngle_);
		// 回転を計算
		Quaternion deltaRotation = IPlayerBehavior::CalRotationAxisAngle(rotationAngle_);
		Quaternion rotation = Quaternion::Normalize(startRotation_ * deltaRotation);
		parts->SetRotate(rotation);
	}
}

void LeftHandFirstAttackBehavior::Reset() {

	// 初期化する
	backLeftRotation_->Reset();
	forwardSlash_->Reset();
	rotationAngle_.Init();
}

void LeftHandFirstAttackBehavior::ImGui() {

	backLeftRotation_->ImGui("LeftHandFirstAttackBehavior_backLeftRotation_");
	forwardSlash_->ImGui("LeftHandFirstAttackBehavior_forwardSlash_");
}

void LeftHandFirstAttackBehavior::SaveJson(Json& data) {

	// 値の保存
	backLeftRotation_->ToJson(data[attack1stBehaviorJsonKey_]["BackLeftRotation"]);
	forwardSlash_->ToJson(data[attack1stBehaviorJsonKey_]["ForwardSlash"]);
}