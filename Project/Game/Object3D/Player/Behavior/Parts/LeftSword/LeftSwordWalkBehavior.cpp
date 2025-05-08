#include "LeftSwordWalkBehavior.h"

//============================================================================
//	include
//============================================================================
#include <Game/Object3D/Player/Parts/Base/BasePlayerParts.h>

//============================================================================
//  LeftSwordWalkBehavior classMethods
//============================================================================

LeftSwordWalkBehavior::LeftSwordWalkBehavior(
	const Json& data, const Quaternion& initRotation) {

	if (data.contains(walkBehaviorJsonKey_)) {

		rotationLerpRate_ = JsonAdapter::GetValue<float>(data[walkBehaviorJsonKey_], "rotationLerpRate_");
	}
	initRotation_ = initRotation;
}

void LeftSwordWalkBehavior::Execute(BasePlayerParts* parts) {

	// 現在の回転を取得
	Quaternion rotation = parts->GetTransform().rotation;

	// 現在回転が初期化値でなければ補間する
	if (rotation != initRotation_) {

		// 初期化値に向かって補間
		rotation = Quaternion::Slerp(rotation, initRotation_, rotationLerpRate_);
		Quaternion worldRotation = Quaternion::Normalize(rotation);
		parts->SetRotate(worldRotation);
	}
}

void LeftSwordWalkBehavior::Reset() {
}

void LeftSwordWalkBehavior::ImGui() {

	ImGui::PushItemWidth(itemWidth_);

	ImGui::DragFloat("rotationLerpRate##LeftSwordWalkBehavior", &rotationLerpRate_, 0.01f);

	ImGui::PopItemWidth();
}

void LeftSwordWalkBehavior::SaveJson(Json& data) {

	// 値の保存
	data[walkBehaviorJsonKey_]["rotationLerpRate_"] = rotationLerpRate_;
}