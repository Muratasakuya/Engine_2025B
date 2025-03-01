#include "DebugCamera.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Window/WinApp.h>
#include <Engine/Input/Input.h>

//============================================================================
//	DebugCamera classMethods
//============================================================================

void DebugCamera::Init() {

	zoomRate_ = 1.0f;

	// 初期値設定
	fovY_ = 0.45f;
	nearClip_ = 0.1f;
	farClip_ = 1024.0f;

	eulerRotation_ = Vector3(0.2f, 0.0f, 0.0f);
	transform_.scale = Vector3::AnyInit(1.0f);
	transform_.rotation = Quaternion::EulerToQuaternion(eulerRotation_);
	transform_.translation = Vector3(0.0f, 6.0f, -36.0f);

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

void DebugCamera::Update() {

	// 移動処理
	Move();

	// 行列更新
	rotateMatrix_ = Matrix4x4::MakeRotateMatrix(eulerRotation_);
	transform_.matrix.world = Matrix4x4::MakeIdentity4x4();
	Vector3 offset = { 0.0f, 2.0f, -12.0f };
	offset = Vector3::TransferNormal(offset, rotateMatrix_);

	Matrix4x4 translateMatrix = Matrix4x4::MakeTranslateMatrix(transform_.translation);
	Matrix4x4 scaleMatrix = Matrix4x4::MakeScaleMatrix(Vector3::AnyInit(1.0f));
	transform_.matrix.world = Matrix4x4::Multiply(scaleMatrix, rotateMatrix_);
	transform_.matrix.world = Matrix4x4::Multiply(transform_.matrix.world, translateMatrix);
	viewMatrix_ = Matrix4x4::Inverse(transform_.matrix.world);

	// アスペクト比
	float aspectRatio = static_cast<float>(WinApp::GetWindowWidth()) /
		static_cast<float>(WinApp::GetWindowHeight());
	projectionMatrix_ =
		Matrix4x4::MakePerspectiveFovMatrix(fovY_, aspectRatio, nearClip_, farClip_);

	viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;
}

void DebugCamera::Move() {

	float deltaX = Input::GetInstance()->GetMouseMoveValue().x;
	float deltaY = Input::GetInstance()->GetMouseMoveValue().y;

	const float rotateSpeed = 0.01f;
	const float panSpeed = 0.1f;

	// 右クリック
	if (Input::GetInstance()->PushMouseRight()) {

		eulerRotation_.x += deltaY * rotateSpeed;
		eulerRotation_.y += deltaX * rotateSpeed;
	}

	// 中クリック + Shift
	if (Input::GetInstance()->PushMouseCenter() && Input::GetInstance()->PushKey(DIK_LSHIFT)) {

		Vector3 right = { panSpeed * deltaX, 0.0f, 0.0f };
		Vector3 up = { 0.0f, -panSpeed * deltaY, 0.0f };

		// 平行移動ベクトルを変換
		right = Vector3::TransferNormal(right, transform_.matrix.world);
		up = Vector3::TransferNormal(up, transform_.matrix.world);

		transform_.translation += right + up;
	}

	Vector3 forward = { 0.0f, 0.0f, Input::GetInstance()->GetMouseWheel() * zoomRate_ };
	forward = Vector3::TransferNormal(forward, rotateMatrix_);

	if (Input::GetInstance()->GetMouseWheel() != 0) {

		transform_.translation += forward;
	}
}