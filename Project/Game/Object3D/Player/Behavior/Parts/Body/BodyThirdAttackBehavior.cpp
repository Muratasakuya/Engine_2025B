#include "BodyThirdAttackBehavior.h"

//============================================================================
//	include
//============================================================================
#include <Game/Camera/FollowCamera.h>
#include <Game/Object3D/Player/Parts/Base/BasePlayerParts.h>

//============================================================================
//	BodyThirdAttackBehavior classMethods
//============================================================================

BodyThirdAttackBehavior::BodyThirdAttackBehavior(
	const Json& data, FollowCamera* followCamera) {

	followCamera_ = nullptr;
	followCamera_ = followCamera;

	moveBack_ = std::make_unique<SimpleAnimation<Vector3>>();
	moveFront_ = std::make_unique<SimpleAnimation<Vector3>>();

	backRotation_ = std::make_unique<SimpleAnimation<Vector3>>();
	frontRotation_ = std::make_unique<SimpleAnimation<Vector3>>();
	if (data.contains(attack3rdBehaviorJsonKey_) && data[attack3rdBehaviorJsonKey_].contains("MoveBack")) {

		moveBack_->FromJson(data[attack3rdBehaviorJsonKey_]["MoveBack"]);
		moveFront_->FromJson(data[attack3rdBehaviorJsonKey_]["MoveFront"]);
		backRotation_->FromJson(data[attack3rdBehaviorJsonKey_]["BackRotation"]);
		frontRotation_->FromJson(data[attack3rdBehaviorJsonKey_]["FrontRotation"]);

		moveWaitTime_ = JsonAdapter::GetValue<float>(data[attack3rdBehaviorJsonKey_], "moveWaitTime_");
		moveValue_ = JsonAdapter::GetValue<float>(data[attack3rdBehaviorJsonKey_], "moveValue_");
	}
}

void BodyThirdAttackBehavior::Execute(BasePlayerParts* parts) {

	// 後ろに引く
	UpdateMoveBack(parts);
	UpdateRotationBack(parts);

	// 時間経過を待つ
	WaitMoveTime();

	// 前に行く
	UpdateMoveFront(parts);
	UpdateRotationFront(parts);
}

void BodyThirdAttackBehavior::UpdateMoveBack(BasePlayerParts* parts) {

	if (!moveBack_->IsStart()) {

		backwardDirection_ = followCamera_->GetTransform().GetBack();
		backwardDirection_.y = 0.0f;
		backwardDirection_ = Vector3::Normalize(backwardDirection_);

		// 開始地点と終了地点を設定
		moveBack_->move_.start = parts->GetTransform().translation;
		moveBack_->move_.end = parts->GetTransform().translation +
			(backwardDirection_ * moveValue_);

		// animation開始
		moveBack_->Start();
	}

	if (moveBack_->IsFinished()) {
		return;
	}

	// 値を補間
	Vector3 translation = parts->GetTransform().translation;
	moveBack_->LerpValue(translation);
	parts->SetTranslate(translation);
}

void BodyThirdAttackBehavior::UpdateRotationBack(BasePlayerParts* parts) {

	if (!backRotation_->IsStart()) {

		// animation開始
		backRotation_->Start();
		// 開始時の回転を記録
		startRotation_ = parts->GetTransform().rotation;
	}

	if (backRotation_->IsFinished()) {
		return;
	}

	// 値を補間
	backRotation_->LerpValue(rotationAngle_);
	// 回転を計算
	Quaternion deltaRotation = IPlayerBehavior::CalRotationAxisAngle(rotationAngle_);
	Quaternion rotation = Quaternion::Normalize(startRotation_ * deltaRotation);
	parts->SetRotate(rotation);
}

void BodyThirdAttackBehavior::UpdateMoveFront(BasePlayerParts* parts) {

	// 時間経過しきったら
	if (enableMoveFront_) {
		if (!moveFront_->IsStart()) {

			// 開始地点と終了地点を設定
			moveFront_->move_.start = parts->GetTransform().translation;
			moveFront_->move_.end = moveBack_->move_.start;

			// animation開始
			moveFront_->Start();
		}
	}

	if (moveFront_->IsFinished()) {
		return;
	}

	// 値を補間
	Vector3 translation = parts->GetTransform().translation;
	moveFront_->LerpValue(translation);
	parts->SetTranslate(translation);
}

void BodyThirdAttackBehavior::UpdateRotationFront(BasePlayerParts* parts) {

	// 時間経過しきったら
	if (enableMoveFront_) {
		if (!frontRotation_->IsStart()) {

			// animation開始
			frontRotation_->Start();
			// 初期値を設定
			frontRotation_->move_.start = rotationAngle_;
		}
	}

	if (frontRotation_->IsFinished()) {
		return;
	}

	// 値を補間
	frontRotation_->LerpValue(rotationAngle_);
	// 回転を計算
	Quaternion deltaRotation = IPlayerBehavior::CalRotationAxisAngle(rotationAngle_);
	Quaternion rotation = Quaternion::Normalize(startRotation_ * deltaRotation);
	parts->SetRotate(rotation);
}

void BodyThirdAttackBehavior::WaitMoveTime() {

	// 後ろに下がりきったら
	if (moveBack_->IsFinished() && !enableMoveFront_) {

		// 時間経過を進める
		moveWaitTimer_ += GameTimer::GetDeltaTime();
		if (moveWaitTimer_ > moveWaitTime_) {

			// 次のanimation開始
			enableMoveFront_ = true;
		}
	}
}

void BodyThirdAttackBehavior::Reset() {

	// 初期化する
	moveBack_->Reset();
	moveFront_->Reset();
	backRotation_->Reset();
	frontRotation_->Reset();
	moveWaitTimer_ = 0.0f;
	enableMoveFront_ = false;
}

void BodyThirdAttackBehavior::ImGui() {

	ImGui::PushItemWidth(itemWidth_);

	ImGui::DragFloat("moveWaitTime", &moveWaitTime_, 0.01f);
	ImGui::DragFloat("moveValue", &moveValue_, 0.01f);

	if (ImGui::TreeNode("MoveBack")) {

		moveBack_->ImGui("BodyThirdAttackBehavior_moveBack_");
		backRotation_->ImGui("BodyThirdAttackBehavior_backRotation_");
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("MoveFront")) {

		moveFront_->ImGui("BodyThirdAttackBehavior_moveFront_");
		frontRotation_->ImGui("BodyThirdAttackBehavior_frontRotation_");
		ImGui::TreePop();
	}

	ImGui::PopItemWidth();
}

void BodyThirdAttackBehavior::SaveJson(Json& data) {

	// 値の保存
	moveBack_->ToJson(data[attack3rdBehaviorJsonKey_]["MoveBack"]);
	moveFront_->ToJson(data[attack3rdBehaviorJsonKey_]["MoveFront"]);
	backRotation_->ToJson(data[attack3rdBehaviorJsonKey_]["BackRotation"]);
	frontRotation_->ToJson(data[attack3rdBehaviorJsonKey_]["FrontRotation"]);

	data[attack3rdBehaviorJsonKey_]["moveValue_"] = moveValue_;
	data[attack3rdBehaviorJsonKey_]["moveWaitTime_"] = moveWaitTime_;
}