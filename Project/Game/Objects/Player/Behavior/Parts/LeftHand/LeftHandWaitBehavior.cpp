#include "LeftHandWaitBehavior.h"

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Player/Parts/Base/BasePlayerParts.h>

//============================================================================
//  LeftHandWaitBehavior classMethods
//============================================================================

LeftHandWaitBehavior::LeftHandWaitBehavior(const Json& data) {

	if (data.contains(waitBehaviorJsonKey_)) {

		rotationLerpRate_ = JsonAdapter::GetValue<float>(data[waitBehaviorJsonKey_], "rotationLerpRate_");
	}
}

void LeftHandWaitBehavior::Execute(BasePlayerParts* parts) {

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

void LeftHandWaitBehavior::Reset() {
}

void LeftHandWaitBehavior::ImGui() {

	ImGui::PushItemWidth(itemWidth_);

	ImGui::DragFloat("rotationLerpRate##LeftHandWaitBehavior", &rotationLerpRate_, 0.01f);

	ImGui::PopItemWidth();
}

void LeftHandWaitBehavior::SaveJson(Json& data) {

	// 値の保存
	data[waitBehaviorJsonKey_]["rotationLerpRate_"] = rotationLerpRate_;
}