#include "CameraManager.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>
#include <Engine/Core/Window/WinApp.h>
#include <Engine/Renderer/LineRenderer.h>

//============================================================================
//	CameraManager classMethods
//============================================================================

void CameraManager::Init() {

#ifdef _DEBUG
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Init();
#endif // _DEBUG

	// 影生成用
	lightViewCamera_ = std::make_unique<LightViewCamera>();
	lightViewCamera_->Init();
}

void CameraManager::Update() {

#ifdef _DEBUG
	debugCamera_->Update();
#endif // _DEBUG

	lightViewCamera_->Update();

	// gameCameraの視錐台描画
	RenderCameraFrame();
}

void CameraManager::SetCamera(BaseCamera* gameCamera) {

	// カメラのセット
	gameCamera_ = std::nullopt;
	gameCamera_ = gameCamera;
}

BaseCamera* CameraManager::GetCamera() const {

	if (!gameCamera_.has_value()) {
		ASSERT(FALSE, "un setting gameCamera");
		return nullptr;
	}
	return *gameCamera_;
}

void CameraManager::RenderCameraFrame() {

	if (!gameCamera_.has_value()) {
		ASSERT(FALSE, "un setting gameCamera");
	}

	// 1. カメラパラメータ (FOV, Aspect, nearClip_, farClip_) 取得
	float fovY = gameCamera_.value()->GetFovY();
	float n = gameCamera_.value()->GetNearClip();
	float f = gameCamera_.value()->GetFarClip();
	float aspect = static_cast<float>(WinApp::GetWindowWidth()) /
		static_cast<float>(WinApp::GetWindowHeight());

	// カメラ空間でのコーナー計算
	float halfFovY = fovY * 0.5f;
	float heightNearHalf = std::tan(halfFovY) * n;
	float widthNearHalf = heightNearHalf * aspect;
	float heightFarHalf = std::tan(halfFovY) * f;
	float widthFarHalf = heightFarHalf * aspect;

	Vector3 ncTL(-widthNearHalf, heightNearHalf, n);
	Vector3 ncTR(widthNearHalf, heightNearHalf, n);
	Vector3 ncBR(widthNearHalf, -heightNearHalf, n);
	Vector3 ncBL(-widthNearHalf, -heightNearHalf, n);

	Vector3 fcTL(-widthFarHalf, heightFarHalf, f);
	Vector3 fcTR(widthFarHalf, heightFarHalf, f);
	Vector3 fcBR(widthFarHalf, -heightFarHalf, f);
	Vector3 fcBL(-widthFarHalf, -heightFarHalf, f);

	// 視錐台のサイズ
	float scale = 0.08f;

	ncTL *= scale;
	ncTR *= scale;
	ncBR *= scale;
	ncBL *= scale;

	fcTL *= scale;
	fcTR *= scale;
	fcBR *= scale;
	fcBL *= scale;

	Matrix4x4 cameraWorldMatrix = Matrix4x4::Inverse(gameCamera_.value()->GetViewMatrix());

	// ワールド座標に変換
	Vector3 wncTL = Vector3::Transform(ncTL, cameraWorldMatrix);
	Vector3 wncTR = Vector3::Transform(ncTR, cameraWorldMatrix);
	Vector3 wncBR = Vector3::Transform(ncBR, cameraWorldMatrix);
	Vector3 wncBL = Vector3::Transform(ncBL, cameraWorldMatrix);

	Vector3 wfcTL = Vector3::Transform(fcTL, cameraWorldMatrix);
	Vector3 wfcTR = Vector3::Transform(fcTR, cameraWorldMatrix);
	Vector3 wfcBR = Vector3::Transform(fcBR, cameraWorldMatrix);
	Vector3 wfcBL = Vector3::Transform(fcBL, cameraWorldMatrix);

	Color color = Color::Convert(0xff1493ff);
	// 近クリップ
	LineRenderer::GetInstance()->DrawLine3D(wncTL, wncTR, color);
	LineRenderer::GetInstance()->DrawLine3D(wncTR, wncBR, color);
	LineRenderer::GetInstance()->DrawLine3D(wncBR, wncBL, color);
	LineRenderer::GetInstance()->DrawLine3D(wncBL, wncTL, color);
	// 遠クリップ
	LineRenderer::GetInstance()->DrawLine3D(wfcTL, wfcTR, color);
	LineRenderer::GetInstance()->DrawLine3D(wfcTR, wfcBR, color);
	LineRenderer::GetInstance()->DrawLine3D(wfcBR, wfcBL, color);
	LineRenderer::GetInstance()->DrawLine3D(wfcBL, wfcTL, color);
	// 近 → 遠
	LineRenderer::GetInstance()->DrawLine3D(wncTL, wfcTL, color);
	LineRenderer::GetInstance()->DrawLine3D(wncTR, wfcTR, color);
	LineRenderer::GetInstance()->DrawLine3D(wncBR, wfcBR, color);
	LineRenderer::GetInstance()->DrawLine3D(wncBL, wfcBL, color);
}