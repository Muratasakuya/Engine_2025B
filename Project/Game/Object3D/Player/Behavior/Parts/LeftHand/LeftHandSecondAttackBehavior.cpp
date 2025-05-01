#include "LeftHandSecondAttackBehavior.h"

//============================================================================
//	include
//============================================================================
#include <Game/Object3D/Player/Parts/Base/BasePlayerParts.h>

//============================================================================
//  LeftHandSecondAttackBehavior classMethods
//============================================================================

LeftHandSecondAttackBehavior::LeftHandSecondAttackBehavior(const Json& data) {

	frontLeftRotation_ = std::make_unique<SimpleAnimation<Vector3>>();
	frontRightRotation_ = std::make_unique<SimpleAnimation<Vector3>>();
	returnCenterRotation_ = std::make_unique<SimpleAnimation<Vector3>>();
	if (data.contains(attack2ndBehaviorJsonKey_) && data[attack2ndBehaviorJsonKey_].contains("FrontLeftRotation")) {

		frontLeftRotation_->FromJson(data[attack2ndBehaviorJsonKey_]["FrontLeftRotation"]);
		frontRightRotation_->FromJson(data[attack2ndBehaviorJsonKey_]["FrontRightRotation"]);
		returnCenterRotation_->FromJson(data[attack2ndBehaviorJsonKey_]["ReturnCenterRotation"]);
	}
}

void LeftHandSecondAttackBehavior::Execute(BasePlayerParts* parts) {

	// 左前に行く
	UpdateFrontLeftRotation(parts);

	// 右前に大きく動く
	UpdateFrontRightRotation(parts);

	// 最後、中心に戻るように左前に行く
	UpdateReturnCenterRotation(parts);
}

void LeftHandSecondAttackBehavior::UpdateFrontLeftRotation(BasePlayerParts* parts) {

	if (!frontLeftRotation_->IsStart()) {

		// animation開始
		frontLeftRotation_->Start();
		// 開始時の回転を記録
		startRotation_ = parts->GetTransform().rotation;
	}

	if (frontLeftRotation_->IsFinished()) {
		return;
	}

	// 値を補間
	frontLeftRotation_->LerpValue(rotationAngle_);
	// 回転を計算
	Quaternion deltaRotation = IPlayerBehavior::CalRotationAxisAngle(rotationAngle_);
	Quaternion rotation = Quaternion::Normalize(startRotation_ * deltaRotation);
	parts->SetRotate(rotation);
}

void LeftHandSecondAttackBehavior::UpdateFrontRightRotation(BasePlayerParts* parts) {

	// 右前に大きく動き終わったら
	if (frontLeftRotation_->IsFinished()) {
		if (!frontRightRotation_->IsStart()) {

			// animation開始
			frontRightRotation_->Start();
			// 初期値を設定
			frontRightRotation_->move_.start = rotationAngle_;
		}
	}

	if (frontRightRotation_->IsFinished()) {
		return;
	}

	// 値を補間
	frontRightRotation_->LerpValue(rotationAngle_);
	// 回転を計算
	Quaternion deltaRotation = IPlayerBehavior::CalRotationAxisAngle(rotationAngle_);
	Quaternion rotation = Quaternion::Normalize(startRotation_ * deltaRotation);
	parts->SetRotate(rotation);
}

void LeftHandSecondAttackBehavior::UpdateReturnCenterRotation(BasePlayerParts* parts) {

	// 左前に行き終わったら
	if (frontRightRotation_->IsFinished()) {
		if (!returnCenterRotation_->IsStart()) {

			// animation開始
			returnCenterRotation_->Start();
			// 初期値を設定
			returnCenterRotation_->move_.start = rotationAngle_;
		}
	}

	if (returnCenterRotation_->IsFinished()) {
		return;
	}

	// 値を補間
	returnCenterRotation_->LerpValue(rotationAngle_);
	// 回転を計算
	Quaternion deltaRotation = IPlayerBehavior::CalRotationAxisAngle(rotationAngle_);
	Quaternion rotation = Quaternion::Normalize(startRotation_ * deltaRotation);
	parts->SetRotate(rotation);
}

void LeftHandSecondAttackBehavior::Reset() {

	// 初期化する
	frontLeftRotation_->Reset();
	frontRightRotation_->Reset();
	returnCenterRotation_->Reset();
}

void LeftHandSecondAttackBehavior::ImGui() {

	ImGui::PushItemWidth(itemWidth_);

	if (ImGui::TreeNode("MoveFrontLeft")) {

		frontLeftRotation_->ImGui("BodySecondAttackBehavior_frontLeftRotation_");
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("MoveFrontRight")) {

		frontRightRotation_->ImGui("BodySecondAttackBehavior_frontRightRotation_");
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("MoveCenter")) {

		returnCenterRotation_->ImGui("BodySecondAttackBehavior_returnCenterRotation_");
		ImGui::TreePop();
	}

	ImGui::PopItemWidth();
}

void LeftHandSecondAttackBehavior::SaveJson(Json& data) {

	// 値の保存
	frontLeftRotation_->ToJson(data[attack2ndBehaviorJsonKey_]["FrontLeftRotation"]);
	frontRightRotation_->ToJson(data[attack2ndBehaviorJsonKey_]["FrontRightRotation"]);
	returnCenterRotation_->ToJson(data[attack2ndBehaviorJsonKey_]["ReturnCenterRotation"]);
}