#include "BodyWaitBehavior.h"

//============================================================================
//	include
//============================================================================
#include <Game/Object3D/Player/Parts/Base/BasePlayerParts.h>
#include <Game/Time/GameTimer.h>

//============================================================================
//	BodyWaitBehavior classMethods
//============================================================================

BodyWaitBehavior::BodyWaitBehavior(const Json& data) {

	undoPosY_ = std::make_unique<SimpleAnimation<float>>();
	if (data.contains(waitBehaviorJsonKey_)) {

		speed_ = JsonAdapter::GetValue<float>(data[waitBehaviorJsonKey_], "speed_");
		amplitude_ = JsonAdapter::GetValue<float>(data[waitBehaviorJsonKey_], "amplitude_");
		basePosY_ = JsonAdapter::GetValue<float>(data[waitBehaviorJsonKey_], "basePosY_");
		rotationLerpRate_ = JsonAdapter::GetValue<float>(data[waitBehaviorJsonKey_], "rotationLerpRate_");
		undoPosY_->FromJson(data[waitBehaviorJsonKey_]["UndoPosY"]);
	}
}

void BodyWaitBehavior::Execute(BasePlayerParts* parts) {

	if (!baseTranslate_.has_value()) {

		// animation開始
		undoPosY_->Start();
		// 初期座標を取得する
		undoPosY_->move_.start = parts->GetTransform().translation.y;
		baseTranslate_ = parts->GetTransform().translation;
	}

	// 現在の回転を取得
	Quaternion rotation = parts->GetTransform().rotation;
	// 正面向きの回転、Y軸は0.0f
	Vector3 direction = parts->GetTransform().GetForward();
	direction.y = 0.0f;
	direction = Vector3::Normalize(direction);
	Quaternion targetRotation = Quaternion::LookRotation(direction, Vector3(0.0f, 1.0f, 0.0f));

	// 現在回転が初期化値でなければ補間する
	if (rotation != targetRotation) {

		// 初期化値に向かって補間
		rotation = Quaternion::Slerp(rotation, targetRotation, rotationLerpRate_);
		Quaternion worldRotation = Quaternion::Normalize(rotation);
		parts->SetRotate(worldRotation);
	}

	// Y座標をまずbasePosYまで持って行ってからふわふわさせる
	if (baseTranslate_.value().y != basePosY_) {

		undoPosY_->LerpValue(baseTranslate_.value().y);
		parts->SetTranslate(baseTranslate_.value());
		return;
	}

	// 時間を加算
	elapsed_ += GameTimer::GetDeltaTime() * speed_;

	// Y座標を加算
	float offsetY = std::sin(elapsed_) * amplitude_;
	Vector3 translation = parts->GetTransform().translation;
	translation = baseTranslate_.value() + Vector3(0.0f, offsetY, 0.0f);
	parts->SetTranslate(translation);
}

void BodyWaitBehavior::Reset() {

	// 初期化する
	undoPosY_->Reset();
	baseTranslate_ = std::nullopt;
	elapsed_ = 0.0f;
}

void BodyWaitBehavior::ImGui() {

	ImGui::PushItemWidth(itemWidth_);

	ImGui::DragFloat("basePosY", &basePosY_, 0.01f);
	ImGui::DragFloat("rotationLerpRate", &rotationLerpRate_, 0.01f);
	ImGui::DragFloat("speed", &speed_, 0.01f);
	ImGui::DragFloat("amplitude", &amplitude_, 0.01f);

	undoPosY_->ImGui("BodyWaitBehavior_undoPosY_");

	ImGui::PopItemWidth();
}

void BodyWaitBehavior::SaveJson(Json& data) {

	// 値の保存
	data[waitBehaviorJsonKey_]["basePosY_"] = basePosY_;
	data[waitBehaviorJsonKey_]["rotationLerpRate_"] = rotationLerpRate_;
	data[waitBehaviorJsonKey_]["speed_"] = speed_;
	data[waitBehaviorJsonKey_]["amplitude_"] = amplitude_;
	undoPosY_->ToJson(data[waitBehaviorJsonKey_]["UndoPosY"]);
}