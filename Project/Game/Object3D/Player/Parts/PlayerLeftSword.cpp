#include "PlayerLeftSword.h"

//============================================================================
//	PlayerLeftSword classMethods
//============================================================================

void PlayerLeftSword::InitParam() {

	// 初期回転角
	transform_->rotation = Quaternion::Normalize(Quaternion::
		MakeRotateAxisAngleQuaternion(Vector3(1.0f, 0.0f, 0.0f), initRotationAngle_.x) *
		Quaternion::
		MakeRotateAxisAngleQuaternion(Vector3(0.0f, 1.0f, 0.0f), initRotationAngle_.y) *
		Quaternion::
		MakeRotateAxisAngleQuaternion(Vector3(0.0f, 0.0f, 1.0f), initRotationAngle_.z));
}

void PlayerLeftSword::Init() {

	BasePlayerParts::Init("playerSword");

	// json適応
	ApplyJson();
	// parameter設定
	InitParam();
}

void PlayerLeftSword::ImGui() {

	if (ImGui::DragFloat3("initRotationAngle##Init", &initRotationAngle_.x, 0.001f)) {

		transform_->rotation = Quaternion::Normalize(Quaternion::
			MakeRotateAxisAngleQuaternion(Vector3(1.0f, 0.0f, 0.0f), initRotationAngle_.x) *
			Quaternion::
			MakeRotateAxisAngleQuaternion(Vector3(0.0f, 1.0f, 0.0f), initRotationAngle_.y) *
			Quaternion::
			MakeRotateAxisAngleQuaternion(Vector3(0.0f, 0.0f, 1.0f), initRotationAngle_.z));
	}
}

void PlayerLeftSword::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck(parameter_.baseFilePath + "PlayerLeftSword.json", data)) {
		return;
	}

	initRotationAngle_ = JsonAdapter::ToObject<Vector3>(data["initRotationAngle_"]);
}

void PlayerLeftSword::SaveJson() {

	Json data;

	data["initRotationAngle_"] = JsonAdapter::FromObject<Vector3>(initRotationAngle_);

	JsonAdapter::Save(parameter_.baseFilePath + "PlayerLeftSword.json", data);
}