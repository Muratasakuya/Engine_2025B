#include "RightHandWalkBehavior.h"

//============================================================================
//	include
//============================================================================
#include <Game/Object3D/Player/Parts/Base/BasePlayerParts.h>

//============================================================================
//  RightHandWalkBehavior classMethods
//============================================================================

RightHandWalkBehavior::RightHandWalkBehavior(const Json& data) {

	if (data.contains(walkBehaviorJsonKey_)) {

		rotationLerpRate_ = JsonAdapter::GetValue<float>(data[walkBehaviorJsonKey_], "rotationLerpRate_");
	}
}

void RightHandWalkBehavior::Execute(BasePlayerParts* parts) {

	// 現在の回転を取得
	Quaternion rotation = parts->GetTransform().rotation;
	Quaternion identity = Quaternion::IdentityQuaternion();

	// 現在回転が初期化値でなければ補間する
	if (rotation != identity) {

		// 初期化値に向かって補間
		rotation = Quaternion::Slerp(rotation, identity, rotationLerpRate_);
		Quaternion worldRotation = Quaternion::Normalize(rotation);
		parts->SetRotate(worldRotation);
	}
}

void RightHandWalkBehavior::Reset() {
}

void RightHandWalkBehavior::ImGui() {

	ImGui::PushItemWidth(itemWidth_);

	ImGui::DragFloat("rotationLerpRate##RightHandWalkBehavior", &rotationLerpRate_, 0.01f);

	ImGui::PopItemWidth();
}

void RightHandWalkBehavior::SaveJson(Json& data) {

	// 値の保存
	data[walkBehaviorJsonKey_]["rotationLerpRate_"] = rotationLerpRate_;
}