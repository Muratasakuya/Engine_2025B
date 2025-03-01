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

//============================================================================
//	MathUtils
//============================================================================

constexpr float pi = std::numbers::pi_v<float>;
constexpr float radian = pi / 180.0f;