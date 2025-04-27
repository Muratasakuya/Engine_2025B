#include "BodyFirstAttackBehavior.h"

//============================================================================
//	include
//============================================================================
#include <Game/Object3D/Player/Parts/Base/BasePlayerParts.h>

//============================================================================
//	BodyFirstAttackBehavior classMethods
//============================================================================

BodyFirstAttackBehavior::BodyFirstAttackBehavior(const Json& data) {

	moveStraight_ = std::make_unique<SimpleAnimation<Vector3>>();
	if (data.contains(attack1stBehaviorJsonKey_) && data[attack1stBehaviorJsonKey_].contains("MoveStraight")) {

		moveStraight_->FromJson(data[attack1stBehaviorJsonKey_]["MoveStraight"]);
		moveValue_ = JsonAdapter::GetValue<float>(data[attack1stBehaviorJsonKey_], "moveValue_");
	}
}

void BodyFirstAttackBehavior::Execute(BasePlayerParts* parts) {

	if (!setStart_) {
		
		// 開始時の座標、回転を基にどれくらい進ませるか計算する
		// 初期座標を設定
		moveStraight_->move_.start = parts->GetTransform().translation;
		// 終了座標を設定
		Vector3 direction = parts->GetTransform().GetForward();
		direction.y = 0.0f;
		direction = direction.Normalize();
		moveStraight_->move_.end = parts->GetTransform().translation + (direction * moveValue_);

		moveStraight_->Start();
		setStart_ = true;
	}

	// 値を補間
	Vector3 translation = parts->GetTransform().translation;
	moveStraight_->LerpValue(translation);
	parts->SetTranslate(translation);
}

void BodyFirstAttackBehavior::Reset() {

	// 初期化する
	setStart_ = false;
	moveStraight_->Reset();
}

void BodyFirstAttackBehavior::ImGui() {

	ImGui::PushItemWidth(itemWidth_);

	ImGui::DragFloat("moveValue##BodyFirstAttackBehavior", &moveValue_, 0.01f);

	ImGui::PopItemWidth();

	moveStraight_->ImGui("BodyFirstAttackBehavior_moveStraight_");
}

void BodyFirstAttackBehavior::SaveJson(Json& data) {

	// 値の保存
	moveStraight_->ToJson(data[attack1stBehaviorJsonKey_]["MoveStraight"]);
	data[attack1stBehaviorJsonKey_]["moveValue_"] = moveValue_;
}