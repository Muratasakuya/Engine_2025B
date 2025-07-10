#pragma once

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/Vector2.h>
#include <Lib/MathUtils/Vector3.h>
#include <Lib/MathUtils/Vector4.h>
#include <Lib/MathUtils/Quaternion.h>
#include <Lib/MathUtils/Matrix4x4.h>

// c++
#include <numbers>

constexpr float pi = std::numbers::pi_v<float>;
constexpr float radian = pi / 180.0f;

//============================================================================
//	Math namespace
//============================================================================

namespace Math {

	float GetYawRadian(const Vector3& direction);

	Vector3 RandomPointOnArc(const Vector3& center, const Vector3& direction,
		float radius, float halfAngle);

	Vector3 RandomPointOnArcInSquare(const Vector3& arcCenter, const Vector3& direction,
		float radius, float halfAngle, const Vector3& squareCenter,
		float clampHalfSize, int tryCount = 12);

	Vector3 RotateY(const Vector3& v, float rad);
}