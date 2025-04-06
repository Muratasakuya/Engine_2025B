#include "GameCamera.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Window/WinApp.h>

//============================================================================
//	GameCamera classMethods
//============================================================================

void GameCamera::Init() {

	// 初期値設定
	fovY_ = 0.45f;
	nearClip_ = 0.1f;
	farClip_ = 512.0f;

	eulerRotation_ = Vector3(0.26f, 0.0f, 0.0f);
	transform_.scale = Vector3::AnyInit(1.0f);
	transform_.rotation = Quaternion::EulerToQuaternion(eulerRotation_);
	transform_.translation = Vector3(0.0f, 7.5f, -24.0f);

	// 行列更新
	transform_.UpdateMatrix();
	viewMatrix_ = Matrix4x4::Inverse(transform_.matrix.world);

	// アスペクト比
	float aspectRatio = static_cast<float>(WinApp::GetWindowWidth()) /
		static_cast<float>(WinApp::GetWindowHeight());
	projectionMatrix_ =
		Matrix4x4::MakePerspectiveFovMatrix(fovY_, aspectRatio, nearClip_, farClip_);

	viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;
}

void GameCamera::Update() {

	// 行列更新
	transform_.UpdateMatrix();
	viewMatrix_ = Matrix4x4::Inverse(transform_.matrix.world);

	// アスペクト比
	float aspectRatio = static_cast<float>(WinApp::GetWindowWidth()) /
		static_cast<float>(WinApp::GetWindowHeight());
	projectionMatrix_ =
		Matrix4x4::MakePerspectiveFovMatrix(fovY_, aspectRatio, nearClip_, farClip_);

	viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;
}

void GameCamera::ImGui() {

	ImGui::PushItemWidth(itemWidth_);

	ImGui::DragFloat3("translation##GameCamera", &transform_.translation.x, 0.01f);
	if (ImGui::DragFloat3("rotation##GameCamera", &eulerRotation_.x, 0.01f)) {

		transform_.rotation = Quaternion::EulerToQuaternion(eulerRotation_);
	}
	ImGui::Text("quaternion(%4.3f, %4.3f, %4.3f, %4.3f)",
		transform_.rotation.x, transform_.rotation.y, transform_.rotation.z, transform_.rotation.w);

	ImGui::DragFloat("fovY##GameCamera", &fovY_, 0.01f);
	ImGui::DragFloat("farClip##GameCamera", &farClip_, 1.0f);

	ImGui::PopItemWidth();
}