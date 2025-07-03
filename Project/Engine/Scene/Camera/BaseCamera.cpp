#include "BaseCamera.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Config.h>
#include <Lib/MathUtils/MathUtils.h>

//============================================================================
//	BaseCamera classMethods
//============================================================================

BaseCamera::BaseCamera() {

	// 初期値設定
	aspectRatio_ = Config::kWindowWidthf / Config::kWindowHeightf;

	// transformを一回初期化
	transform_.Init();
}

void BaseCamera::CalBillboardMatrix() {

	// billboardMatrixを計算する
	Matrix4x4 backToFrontMatrix = Matrix4x4::MakeYawMatrix(pi);

	billboardMatrix_ = Matrix4x4::Multiply(backToFrontMatrix, transform_.matrix.world);
	billboardMatrix_.m[3][0] = 0.0f;
	billboardMatrix_.m[3][1] = 0.0f;
	billboardMatrix_.m[3][2] = 0.0f;
}