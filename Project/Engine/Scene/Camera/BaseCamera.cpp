#include "BaseCamera.h"

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/MathUtils.h>

//============================================================================
//	BaseCamera classMethods
//============================================================================

void BaseCamera::CalBillboardMatrix() {

	// billboardMatrixを計算する
	Matrix4x4 backToFrontMatrix = Matrix4x4::MakeYawMatrix(pi);

	billboardMatrix_ = Matrix4x4::Multiply(backToFrontMatrix, transform_.matrix.world);
	billboardMatrix_.m[3][0] = 0.0f;
	billboardMatrix_.m[3][1] = 0.0f;
	billboardMatrix_.m[3][2] = 0.0f;
}