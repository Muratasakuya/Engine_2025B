#include "RightHandThirdAttackBehavior.h"

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Player/Parts/Base/BasePlayerParts.h>

//============================================================================
//  RightHandThirdAttackBehavior classMethods
//============================================================================

RightHandThirdAttackBehavior::RightHandThirdAttackBehavior(const Json& data) {

	if (data.contains(attack3rdBehaviorJsonKey_)) {

		moveWaitTime_ = JsonAdapter::GetValue<float>(data[attack3rdBehaviorJsonKey_], "moveWaitTime_");
		initRotationAngle_ = JsonAdapter::ToObject<Vector3>(data[attack3rdBehaviorJsonKey_]["initRotationAngle_"]);
	}
}

void RightHandThirdAttackBehavior::Execute(BasePlayerParts* parts) {

	// 回転攻撃が始まるまでの時間を待つ
	WaitAttackTime();

	// 回転を開始地点に設定する
	SetStartRotation(parts);
}

void RightHandThirdAttackBehavior::SetStartRotation(BasePlayerParts* parts) {

	// 時間経過が終わったら
	if (enableMove_) {
		if (!setRotation_) {

			// 値を設定
			Quaternion rotation = IPlayerBehavior::CalRotationAxisAngle(initRotationAngle_);
			parts->SetRotate(rotation);

			// 設定完了
			setRotation_ = true;
		}
	} else {
		return;
	}
}
void RightHandThirdAttackBehavior::WaitAttackTime() {

	// 回転が始まるまでの時間
	if (!enableMove_) {

		// 時間経過を進める
		moveWaitTimer_ += GameTimer::GetDeltaTime();
		if (moveWaitTimer_ > moveWaitTime_) {

			// 動き出し開始
			enableMove_ = true;
		}
	}
}

void RightHandThirdAttackBehavior::Reset() {

	// 初期化する
	enableMove_ = false;
	setRotation_ = false;
	moveWaitTimer_ = 0.0f;
}

void RightHandThirdAttackBehavior::ImGui() {

	ImGui::PushItemWidth(itemWidth_);

	ImGui::DragFloat("moveWaitTime", &moveWaitTime_, 0.001f);
	ImGui::DragFloat3("initRotationAngle", &initRotationAngle_.x, 0.01f);

	ImGui::PopItemWidth();
}

void RightHandThirdAttackBehavior::SaveJson(Json& data) {

	data[attack3rdBehaviorJsonKey_]["moveWaitTime_"] = moveWaitTime_;
	data[attack3rdBehaviorJsonKey_]["initRotationAngle_"] = JsonAdapter::FromObject<Vector3>(initRotationAngle_);
}