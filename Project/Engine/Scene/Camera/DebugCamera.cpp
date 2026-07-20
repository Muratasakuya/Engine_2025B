#include "DebugCamera.h"

using namespace SakuEngine;

//============================================================================
//	include
//============================================================================
#include <Engine/Config.h>
#include <Engine/Input/Input.h>

//============================================================================
//	DebugCamera classMethods
//============================================================================

void DebugCamera::Init() {

	zoomRate_ = 2.0f;
	panSpeed_ = 0.64f;

	// 初期値設定
	fovY_ = 0.54f;
	nearClip_ = 0.1f;
	farClip_ = 3200.0f;

	transform_.SetEulerRotation(Vector3(0.26f, 0.0f, 0.0f));
	transform_.SetScale(SakuEngine::Vector3::AnyInit(1.0f));
	transform_.SetRotation(Quaternion::EulerToQuaternion(transform_.GetEulerRotation()));
	transform_.SetTranslation(Vector3(0.0f, 30.733f, -112.363f));

	// 行列更新
	transform_.UpdateMatrix();
	viewMatrix_ = Matrix4x4::Inverse(transform_.GetMatrix().world);

	// アスペクト比
	float aspectRatio = Config::kWindowWidthf / Config::kWindowHeightf;
	projectionMatrix_ =
		Matrix4x4::MakePerspectiveFovMatrix(fovY_, aspectRatio, nearClip_, farClip_);

	viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;
}

void DebugCamera::Update() {

	// 移動処理
	Move();

	// 自動フォーカス処理
	BaseCamera::UpdateAutoFocus();

	// 行列更新
	transform_.SetRotation(Quaternion::EulerToQuaternion(transform_.GetEulerRotation()));
	rotateMatrix_ = SakuEngine::Matrix4x4::MakeRotateMatrix(transform_.GetEulerRotation());
	transform_.EditMatrix().world = Matrix4x4::MakeIdentity4x4();

	Matrix4x4 translateMatrix = Matrix4x4::MakeTranslateMatrix(transform_.GetTranslation());
	Matrix4x4 scaleMatrix = Matrix4x4::MakeScaleMatrix(SakuEngine::Vector3::AnyInit(1.0f));
	transform_.EditMatrix().world = Matrix4x4::Multiply(scaleMatrix, rotateMatrix_);
	transform_.EditMatrix().world = Matrix4x4::Multiply(transform_.GetMatrix().world, translateMatrix);
	viewMatrix_ = Matrix4x4::Inverse(transform_.GetMatrix().world);

	// アスペクト比
	float aspectRatio = Config::kWindowWidthf / Config::kWindowHeightf;
	projectionMatrix_ =
		Matrix4x4::MakePerspectiveFovMatrix(fovY_, aspectRatio, nearClip_, farClip_);

	viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;

	// billboardMatrixを計算
	BaseCamera::CalBillboardMatrix();
}

void DebugCamera::ImGui() {

	ImGui::PushItemWidth(itemWidth_);

	ImGui::DragFloat3("translation##DebugCamera", &transform_.EditTranslation().x, 0.01f);
	if (ImGui::DragFloat3("rotation##DebugCamera", &transform_.EditEulerRotation().x, 0.01f)) {

		transform_.SetRotation(Quaternion::EulerToQuaternion(transform_.GetEulerRotation()));
	}
	ImGui::Text("quaternion(%4.3f, %4.3f, %4.3f, %4.3f)",
		transform_.GetRotation().x, transform_.GetRotation().y, transform_.GetRotation().z, transform_.GetRotation().w);

	ImGui::DragFloat("zoomRate##DebugCamera", &zoomRate_, 0.01f);
	ImGui::DragFloat("panSpeed##DebugCamera", &panSpeed_, 0.01f);
	ImGui::DragFloat("fovY##DebugCamera", &fovY_, 0.01f);
	ImGui::DragFloat("farClip##DebugCamera", &farClip_, 1.0f);

	ImGui::PopItemWidth();
}

void DebugCamera::Move() {

	ImGui::Begin("Scene", nullptr,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoMove);

	bool isActive = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

	ImGui::End();

	// ウィンドウを触っていない時は操作不可
	if (!isActive) {
		return;
	}

	float deltaX = Input::GetInstance()->GetMouseMoveValue().x;
	float deltaY = Input::GetInstance()->GetMouseMoveValue().y;

	const float rotateSpeed = 0.01f;

	// 右クリック
	if (Input::GetInstance()->PushMouseRight()) {

		SakuEngine::Vector3 eulerRotate = transform_.GetEulerRotation();
		eulerRotate.x += deltaY * rotateSpeed;
		eulerRotate.y += deltaX * rotateSpeed;
		transform_.SetEulerRotation(eulerRotate);
	}

	// 中クリック
	if (Input::GetInstance()->PushMouseCenter()) {

		Vector3 right = { panSpeed_ * deltaX, 0.0f, 0.0f };
		Vector3 up = { 0.0f, -panSpeed_ * deltaY, 0.0f };

		// 平行移動ベクトルを変換
		right = Vector3::TransferNormal(right, transform_.GetMatrix().world);
		up = Vector3::TransferNormal(up, transform_.GetMatrix().world);

		transform_.AddTranslation(right + up);
	}

	Vector3 forward = { 0.0f, 0.0f, Input::GetInstance()->GetMouseWheel() * zoomRate_ };
	forward = Vector3::TransferNormal(forward, rotateMatrix_);

	if (Input::GetInstance()->GetMouseWheel() != 0) {

		transform_.AddTranslation(forward);
	}
}
