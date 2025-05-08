#include "BasePlayerParts.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/ComponentHelper.h>
#include <Engine/Input/Input.h>
#include <Lib/Adapter/JsonAdapter.h>

//============================================================================
//	BasePlayerParts classMethods
//============================================================================

void BasePlayerParts::Init(const std::string& modelName,
	const std::optional<std::string>& objectName) {

	input_ = nullptr;
	input_ = Input::GetInstance();

	const std::string groupName = "Player";

	// playerPartsを作成
	uint32_t entityId = GameObjectHelper::CreateObject3D(
		modelName, objectName.value_or(modelName), groupName);

	// 各componentを取得
	transform_ = Component::GetComponent<Transform3DComponent>(entityId);
	material_ = Component::GetComponent<MaterialComponent>(entityId);

	// json適応
	parameter_.owner = this;
	parameter_.name = objectName.value_or(modelName) + "Param";
	parameter_.ApplyJson();
	// 値を設定
	SetParam();
}

void BasePlayerParts::RegisterBehavior(PlayerBehaviorType type,
	std::unique_ptr<IPlayerBehavior> behavior) {

	behaviors_[type] = std::move(behavior);
}

void BasePlayerParts::ExecuteBehavior(PlayerBehaviorType type) {

	if (behaviors_.count(type)) {

		behaviors_[type]->Execute(this);
	}
}

void BasePlayerParts::ResetBehavior(PlayerBehaviorType type) {

	if (behaviors_.count(type)) {

		behaviors_[type]->Reset();
	}
}

void BasePlayerParts::ImGuiMaterial() {

	material_->ImGui(parameter_.itemWidth);
}

void BasePlayerParts::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck(parameter_.owner->baseInitJsonFilePath_ +
		parameter_.name + "Material.json", data)) {
		return;
	}

	material_->FromJson(data["Material"]);
}

void BasePlayerParts::SaveJson() {

	Json data;

	material_->ToJson(data["Material"]);

	JsonAdapter::Save(parameter_.owner->baseInitJsonFilePath_ +
		parameter_.name + "Material.json", data);
}

void BasePlayerParts::PartsParameter::ImGui() {

	ImGui::PushItemWidth(itemWidth);

	bool isChange = false;

	isChange |= ImGui::DragFloat3(("offsetTranslation##" + name).c_str(), &offsetTranslation.x, 0.01f);
	isChange |= ImGui::DragFloat3(("initRotationAngle##" + name).c_str(), &initRotationAngle.x, 0.01f);

	// 変更があったときのみ更新
	if (isChange) {

		owner->SetParam();
	}

	ImGui::PopItemWidth();
}

void BasePlayerParts::PartsParameter::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck(owner->baseInitJsonFilePath_ + name + ".json", data)) {
		return;
	}

	offsetTranslation = JsonAdapter::ToObject<Vector3>(data["offsetTranslation"]);
	initRotationAngle = JsonAdapter::ToObject<Vector3>(data["initRotationAngle"]);
}

void BasePlayerParts::PartsParameter::SaveJson() {

	Json data;

	data["offsetTranslation"] = JsonAdapter::FromObject<Vector3>(offsetTranslation);
	data["initRotationAngle"] = JsonAdapter::FromObject<Vector3>(initRotationAngle);

	JsonAdapter::Save(owner->baseInitJsonFilePath_ + name + ".json", data);
}

void BasePlayerParts::SetParent(const Transform3DComponent& parent) {
	// 親を設定
	transform_->parent = &parent;
}

const Transform3DComponent& BasePlayerParts::GetTransform() const {
	return *transform_;
}

void BasePlayerParts::SetParam() {

	// 必要な値を設定
	transform_->translation = parameter_.offsetTranslation;
	transform_->rotation = CalRotationAxisAngle(parameter_.initRotationAngle);
}

Quaternion BasePlayerParts::CalRotationAxisAngle(const Vector3& rotationAngle) {

	Quaternion rotation = Quaternion::Normalize(Quaternion::
		MakeRotateAxisAngleQuaternion(Vector3(1.0f, 0.0f, 0.0f), rotationAngle.x) *
		Quaternion::
		MakeRotateAxisAngleQuaternion(Vector3(0.0f, 1.0f, 0.0f), rotationAngle.y) *
		Quaternion::
		MakeRotateAxisAngleQuaternion(Vector3(0.0f, 0.0f, 1.0f), rotationAngle.z));
	return  rotation;
}

void BasePlayerParts::InputKey(Vector2& inputValue) {

	if (input_->PushKey(DIK_W)) {

		inputValue.y += 1.0f;
	} else if (input_->PushKey(DIK_S)) {

		inputValue.y -= 1.0f;
	}
	if (input_->PushKey(DIK_D)) {

		inputValue.x += 1.0f;
	} else if (input_->PushKey(DIK_A)) {

		inputValue.x -= 1.0f;
	}
}