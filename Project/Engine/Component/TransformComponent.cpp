#include "TransformComponent.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Window/WinApp.h>

// imgui
#include <imgui.h>

//============================================================================
//	TransformComponent classMethods
//============================================================================

//============================================================================
// 3D
//============================================================================

void Transform3DComponent::Init() {

	scale = Vector3::AnyInit(1.0f);
	rotation.Init();
	translation.Init();
}

void Transform3DComponent::UpdateMatrix() {

	// 行列を更新
	matrix.Update(parent, scale, rotation, translation);
}

void Transform3DComponent::ImGui(float itemSize) {

	ImGui::PushItemWidth(itemSize);
	if (ImGui::Button("Reset")) {

		scale = Vector3::AnyInit(1.0f);
		rotation.Init();
		translation.Init();
	}
	ImGui::DragFloat3("translate", &translation.x, 0.01f);
	ImGui::DragFloat3("scale", &scale.x, 0.01f);
	ImGui::PopItemWidth();
}

Vector3 Transform3DComponent::GetWorldPos() const {

	Vector3 worldPos{};
	worldPos.x = matrix.world.m[3][0];
	worldPos.y = matrix.world.m[3][1];
	worldPos.z = matrix.world.m[3][2];

	return worldPos;
}

Vector3 Transform3DComponent::GetForward() const {
	return Vector3(matrix.world.m[2][0], matrix.world.m[2][1], matrix.world.m[2][2]).Normalize();
}

Vector3 Transform3DComponent::GetBack() const {
	return Vector3(-GetForward().x, -GetForward().y, -GetForward().z);
}

Vector3 Transform3DComponent::GetRight() const {
	return Vector3(matrix.world.m[0][0], matrix.world.m[0][1], matrix.world.m[0][2]).Normalize();
}

Vector3 Transform3DComponent::GetLeft() const {
	return Vector3(-GetRight().x, -GetRight().y, -GetRight().z);
}

Vector3 Transform3DComponent::GetUp() const {
	return Vector3(matrix.world.m[1][0], matrix.world.m[1][1], matrix.world.m[1][2]).Normalize();
}

Vector3 Transform3DComponent::GetDown() const {
	return Vector3(-GetUp().x, -GetUp().y, -GetUp().z);
}

//============================================================================
// 2D
//============================================================================

void Transform2DComponent::Init() {

	translation = Vector2::AnyInit(0.0f);
	rotation = 0.0f;

	size = Vector2::AnyInit(0.0f);
	// 中心で設定
	anchorPoint = Vector2::AnyInit(0.5f);

	// 左上設定
	textureLeftTop = Vector2::AnyInit(0.0f);
	textureSize = Vector2::AnyInit(0.0f);
}

void Transform2DComponent::UpdateMatrix() {

	Vector3 scale = Vector3(size.x, size.y, 1.0f);
	Vector3 rotate = Vector3(0.0f, 0.0f, rotation);
	Vector3 translate = Vector3(translation.x, translation.y, 0.0f);

	matrix = Matrix4x4::MakeAffineMatrix(scale, rotate, translate);

	if (parent) {
		matrix = parent->matrix * matrix;
	}
}

void Transform2DComponent::ImGui(float itemSize) {

	ImGui::SeparatorText("Config");

	if (ImGui::Button("Set CenterPos", ImVec2(itemSize, 22.0f))) {

		translation.x = static_cast<float>(WinApp::GetWindowWidth()) / 2.0f;
		translation.y = static_cast<float>(WinApp::GetWindowHeight()) / 2.0f;
	}

	if (ImGui::Button("Set CenterAnchor", ImVec2(itemSize, 22.0f))) {

		anchorPoint = Vector2::AnyInit(0.5f);
	}

	if (ImGui::Button("Set LeftAnchor", ImVec2(itemSize, 22.0f))) {

		anchorPoint = Vector2::AnyInit(0.0f);
	}

	if (ImGui::Button("Set RightAnchor", ImVec2(itemSize, 22.0f))) {

		anchorPoint = Vector2::AnyInit(1.0f);
	}

	ImGui::SeparatorText("Parameter");

	ImGui::PushItemWidth(itemSize);
	ImGui::DragFloat2("translate", &translation.x, 1.0f);
	ImGui::SliderAngle("rotation", &rotation);

	ImGui::DragFloat2("size", &size.x, 1.0f);
	ImGui::DragFloat2("anchorPoint", &anchorPoint.x, 0.01f, 0.0f, 1.0f);

	ImGui::DragFloat2("textureLeftTop", &textureLeftTop.x, 1.0f);
	ImGui::DragFloat2("textureSize", &textureSize.x, 1.0f);
	ImGui::PopItemWidth();
}