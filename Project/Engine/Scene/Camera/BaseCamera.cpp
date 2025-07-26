#include "BaseCamera.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Config.h>
#include <Engine/Core/Graphics/Renderer/LineRenderer.h>
#include <Lib/MathUtils/MathUtils.h>

//============================================================================
//	BaseCamera classMethods
//============================================================================

BaseCamera::BaseCamera() {

	// 初期値設定
	aspectRatio_ = Config::kWindowWidthf / Config::kWindowHeightf;
	frustumScale_ = 0.004f;

	fovY_ = 0.54f;
	nearClip_ = 0.1f;
	farClip_ = 3200.0f;

	// transformを一回初期化
	eulerRotation_ = Vector3(0.26f, 0.0f, 0.0f);
	transform_.scale = Vector3::AnyInit(1.0f);
	transform_.rotation = Quaternion::EulerToQuaternion(eulerRotation_);
	transform_.translation = Vector3(0.0f, 30.733f, -112.363f);
}

void BaseCamera::UpdateView() {

	transform_.UpdateMatrix();
	viewMatrix_ = Matrix4x4::Inverse(transform_.matrix.world);

	// アスペクト比
	float aspectRatio = Config::kWindowWidthf / Config::kWindowHeightf;
	projectionMatrix_ =
		Matrix4x4::MakePerspectiveFovMatrix(fovY_, aspectRatio, nearClip_, farClip_);

	viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;

	// billboardMatrixを計算
	CalBillboardMatrix();
}

void BaseCamera::ImGui() {

	ImGui::DragFloat3("translation##DebugCamera", &transform_.translation.x, 0.01f);
	if (ImGui::DragFloat3("rotation##DebugCamera", &eulerRotation_.x, 0.01f)) {

		transform_.rotation = Quaternion::EulerToQuaternion(eulerRotation_);
	}
	ImGui::Text("quaternion(%4.3f, %4.3f, %4.3f, %4.3f)",
		transform_.rotation.x, transform_.rotation.y, transform_.rotation.z, transform_.rotation.w);

	ImGui::DragFloat("fovY##DebugCamera", &fovY_, 0.01f);
	ImGui::DragFloat("farClip##DebugCamera", &farClip_, 1.0f);
}

void BaseCamera::EditFrustum() {

	ImGui::Checkbox("displayFrustum", &displayFrustum_);
	ImGui::DragFloat("frustumScale", &frustumScale_, 0.001f);
}

void BaseCamera::RenderFrustum() {

	if (!displayFrustum_) {
		return;
	}

	// カメラ空間でのコーナー計算
	float halfFovY = (fovY_ + 0.08f) * 0.5f;
	float heightNearHalf = std::tan(halfFovY) * nearClip_;
	float widthNearHalf = heightNearHalf * aspectRatio_;
	float heightFarHalf = std::tan(halfFovY) * farClip_;
	float widthFarHalf = heightFarHalf * aspectRatio_;

	Vector3 ncTL(-widthNearHalf, heightNearHalf, nearClip_);
	Vector3 ncTR(widthNearHalf, heightNearHalf, nearClip_);
	Vector3 ncBR(widthNearHalf, -heightNearHalf, nearClip_);
	Vector3 ncBL(-widthNearHalf, -heightNearHalf, nearClip_);

	Vector3 fcTL(-widthFarHalf, heightFarHalf, farClip_);
	Vector3 fcTR(widthFarHalf, heightFarHalf, farClip_);
	Vector3 fcBR(widthFarHalf, -heightFarHalf, farClip_);
	Vector3 fcBL(-widthFarHalf, -heightFarHalf, farClip_);

	ncTL *= frustumScale_;
	ncTR *= frustumScale_;
	ncBR *= frustumScale_;
	ncBL *= frustumScale_;

	fcTL *= frustumScale_;
	fcTR *= frustumScale_;
	fcBR *= frustumScale_;
	fcBL *= frustumScale_;

	Matrix4x4 cameraWorldMatrix = Matrix4x4::Inverse(viewMatrix_);

	// ワールド座標に変換
	Vector3 wncTL = Vector3::Transform(ncTL, cameraWorldMatrix);
	Vector3 wncTR = Vector3::Transform(ncTR, cameraWorldMatrix);
	Vector3 wncBR = Vector3::Transform(ncBR, cameraWorldMatrix);
	Vector3 wncBL = Vector3::Transform(ncBL, cameraWorldMatrix);

	Vector3 wfcTL = Vector3::Transform(fcTL, cameraWorldMatrix);
	Vector3 wfcTR = Vector3::Transform(fcTR, cameraWorldMatrix);
	Vector3 wfcBR = Vector3::Transform(fcBR, cameraWorldMatrix);
	Vector3 wfcBL = Vector3::Transform(fcBL, cameraWorldMatrix);

	Color color = Color::Yellow();
	LineRenderer* lineRenderer = LineRenderer::GetInstance();

	// 近クリップ
	lineRenderer->DrawLine3D(wncTL, wncTR, color);
	lineRenderer->DrawLine3D(wncTR, wncBR, color);
	lineRenderer->DrawLine3D(wncBR, wncBL, color);
	lineRenderer->DrawLine3D(wncBL, wncTL, color);
	// 遠クリップ
	lineRenderer->DrawLine3D(wfcTL, wfcTR, color);
	lineRenderer->DrawLine3D(wfcTR, wfcBR, color);
	lineRenderer->DrawLine3D(wfcBR, wfcBL, color);
	lineRenderer->DrawLine3D(wfcBL, wfcTL, color);
	// 近 → 遠
	lineRenderer->DrawLine3D(wncTL, wfcTL, color);
	lineRenderer->DrawLine3D(wncTR, wfcTR, color);
	lineRenderer->DrawLine3D(wncBR, wfcBR, color);
	lineRenderer->DrawLine3D(wncBL, wfcBL, color);
}

void BaseCamera::CalBillboardMatrix() {

	// billboardMatrixを計算する
	Matrix4x4 backToFrontMatrix = Matrix4x4::MakeYawMatrix(pi);

	billboardMatrix_ = Matrix4x4::Multiply(backToFrontMatrix, transform_.matrix.world);
	billboardMatrix_.m[3][0] = 0.0f;
	billboardMatrix_.m[3][1] = 0.0f;
	billboardMatrix_.m[3][2] = 0.0f;
}