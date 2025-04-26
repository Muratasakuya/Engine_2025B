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

void BasePlayerParts::Init(const std::string& modelName) {

	input_ = nullptr;
	input_ = Input::GetInstance();

	const std::string groupName = "Player";

	// playerPartsを作成
	// modelName == objectName
	uint32_t entityId = GameObjectHelper::CreateObject3D(
		modelName, modelName, groupName);

	// 各componentを取得
	transform_ = Component::GetComponent<Transform3DComponent>(entityId);
	material_ = Component::GetComponent<MaterialComponent>(entityId);
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

void BasePlayerParts::PartsParameter::ImGui() {

	ImGui::PushItemWidth(itemWidth);

	ImGui::DragFloat3(("offsetTranslation##" + name).c_str(), &offsetTranslation.x, 0.01f);

	ImGui::PopItemWidth();
}

void BasePlayerParts::PartsParameter::ApplyJson() {

	Json data;
	if (!JsonAdapter::LoadCheck(baseFilePath + name + ".json", data)) {
		return;
	}

	offsetTranslation = JsonAdapter::ToObject<Vector3>(data["offsetTranslation"]);
}

void BasePlayerParts::PartsParameter::SaveJson() {

	Json data;

	data["offsetTranslation"] = JsonAdapter::FromObject<Vector3>(offsetTranslation);

	JsonAdapter::Save(baseFilePath + name + ".json", data);
}

void BasePlayerParts::SetParent(const Transform3DComponent& parent) {
	// 親を設定
	transform_->parent = &parent;
}

void BasePlayerParts::SetParam(const PartsParameter& param) {

	// 必要な値を設定
	parameter_.offsetTranslation = param.offsetTranslation;
	transform_->translation = parameter_.offsetTranslation;
}

const Transform3DComponent& BasePlayerParts::GetTransform() const {
	return *transform_;
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