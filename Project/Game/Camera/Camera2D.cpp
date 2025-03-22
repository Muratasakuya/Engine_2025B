#include "Camera2D.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Window/WinApp.h>

//============================================================================
//	Camera2D classMethods
//============================================================================

void Camera2D::Init() {

	// 初期値設定
	transform_.Init();

	// 行列更新
	transform_.UpdateMatrix();
	viewMatrix_ = Matrix4x4::Inverse(transform_.matrix.world);

	projectionMatrix_ =
		Matrix4x4::MakeOrthographicMatrix(0.0f, 0.0f, static_cast<float>(WinApp::GetWindowWidth()),
			static_cast<float>(WinApp::GetWindowHeight()), 0.0f, 100.0f);

	// 更新1回でいいので、Update関数は実装しない
	viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;
}