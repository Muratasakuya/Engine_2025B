#include "StageField.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/User/ComponentHelper.h>

//============================================================================
//	StageField classMethods
//============================================================================

StageField::~StageField() {

	GameObjectHelper::RemoveObject3D(objectId_);
}

void StageField::Init() {

	// field作成
	objectId_ = GameObjectHelper::CreateObject3D("stageField", "stageField", "Environment");

	// uvTransform初期化
	uvTransform_.scale = Vector3::AnyInit(1.0f);
}

void StageField::Update() {

	ImGui();

	Material* material = Component::GetComponent<Material>(objectId_);
	if (!material) {
		return;
	}

	material->uvTransform = Matrix4x4::MakeAffineMatrix(
		uvTransform_.scale, uvTransform_.rotate, uvTransform_.translate);
}

void StageField::ImGui() {

	std::function<void()> imgui = [&]() {

		ImGui::DragFloat3("uvTranslation", &uvTransform_.translate.x, 0.1f);
		ImGui::DragFloat3("uvScale", &uvTransform_.scale.x, 0.1f);
		};
	GameObjectHelper::SetImGuiFunc(objectId_, imgui);
}