#include "MathUtils.h"

//============================================================================
//	include
//============================================================================
#include <Lib/Adapter/RandomGenerator.h>

//============================================================================
//	MathUtils namespaceMethods
//============================================================================

float Math::GetYawRadian(const Vector3& direction) {

	return std::atan2(direction.z, direction.x);
}

Vector3 Math::RandomPointOnArc(const Vector3& center,
	const Vector3& direction, float radius, float halfAngle) {

	const float baseYaw = GetYawRadian(direction.Normalize());
	const float halfRad = pi * halfAngle / 180.0f;
	const float randYaw = RandomGenerator::Generate(-halfRad, halfRad);
	const float yaw = baseYaw + randYaw;

	return { center.x + radius * std::cos(yaw),center.y,center.z + radius * std::sin(yaw) };
}