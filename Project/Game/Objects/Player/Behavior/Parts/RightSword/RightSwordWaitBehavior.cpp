#include "RightSwordWaitBehavior.h"

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Player/Parts/Base/BasePlayerParts.h>

//============================================================================
//  RightSwordWaitBehavior classMethods
//============================================================================

RightSwordWaitBehavior::RightSwordWaitBehavior(
	const Json& data, const Quaternion& initRotation) {

	if (data.contains(waitBehaviorJsonKey_)) {

		rotationLerpRate_ = JsonAdapter::GetValue<float>(data[waitBehaviorJsonKey_], "rotationLerpRate_");
	}
	initRotation_ = initRotation;
}

void RightSwordWaitBehavior::Execute(BasePlayerParts* parts) {

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

void RightSwordWaitBehavior::Reset() {
}

void RightSwordWaitBehavior::ImGui() {

	ImGui::PushItemWidth(itemWidth_);

	ImGui::DragFloat("rotationLerpRate##RightSwordWaitBehavior", &rotationLerpRate_, 0.01f);

	ImGui::PopItemWidth();
}

void RightSwordWaitBehavior::SaveJson(Json& data) {

	// 値の保存
	data[waitBehaviorJsonKey_]["rotationLerpRate_"] = rotationLerpRate_;
}