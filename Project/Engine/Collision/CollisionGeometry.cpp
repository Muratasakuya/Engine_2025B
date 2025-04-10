#include "CollisionGeometry.h"

//============================================================================
//	Collision Methods
//============================================================================

bool Collision::SphereToSphere(const CollisionShape::Sphere& sphereA,
	const CollisionShape::Sphere& sphereB) {

	float distance = Vector3(sphereA.center - sphereB.center).Length();
	if (distance <= sphereA.radius + sphereB.radius) {
		return true;
	}

	return false;
}

bool Collision::SphereToOBB(const CollisionShape::Sphere& sphere,
	const CollisionShape::OBB& obb) {

	Matrix4x4 rotateMatrix = Quaternion::MakeRotateMatrix(obb.rotate);

	Vector3 orientations[3];
	orientations[0] = Vector3::Transform(Vector3(1.0f, 0.0f, 0.0f), rotateMatrix);
	orientations[1] = Vector3::Transform(Vector3(0.0f, 1.0f, 0.0f), rotateMatrix);
	orientations[2] = Vector3::Transform(Vector3(0.0f, 0.0f, 1.0f), rotateMatrix);

	Vector3 localSphereCenter = sphere.center - obb.center;
	Vector3 closestPoint = obb.center;

	for (int i = 0; i < 3; ++i) {

		float distance = Vector3::Dot(localSphereCenter, orientations[i]);
		float halfSize = (i == 0) ? obb.size.x : (i == 1) ? obb.size.y : obb.size.z;

		if (distance > halfSize) {
			distance = halfSize;
		} else if (distance < -halfSize) {
			distance = -halfSize;
		}

		closestPoint += distance * orientations[i];
	}

	Vector3 diff = closestPoint - sphere.center;
	float distanceSquared = Vector3::Dot(diff, diff);

	return distanceSquared <= (sphere.radius * sphere.radius);
}

bool Collision::OBBToOBB(const CollisionShape::OBB& obbA, const CollisionShape::OBB& obbB) {

	auto CalculateProjection =
		[](const CollisionShape::OBB& obb, const Vector3& axis, const Vector3* axes) -> float {
		return std::abs(obb.size.x * Vector3::Dot(axes[0], axis)) +
			std::abs(obb.size.y * Vector3::Dot(axes[1], axis)) +
			std::abs(obb.size.z * Vector3::Dot(axes[2], axis));
		};

	auto GetOBBAxes = [](const CollisionShape::OBB& obb) -> std::array<Vector3, 3> {
		Matrix4x4 rotationMatrix = Quaternion::MakeRotateMatrix(obb.rotate);
		return {
			Vector3::Transform(Vector3(1.0f, 0.0f, 0.0f), rotationMatrix),
			Vector3::Transform(Vector3(0.0f, 1.0f, 0.0f), rotationMatrix),
			Vector3::Transform(Vector3(0.0f, 0.0f, 1.0f), rotationMatrix)
		};
		};

	auto obbAxesA = GetOBBAxes(obbA);
	auto obbAxesB = GetOBBAxes(obbB);

	std::vector<Vector3> axes;
	axes.insert(axes.end(), obbAxesA.begin(), obbAxesA.end());
	axes.insert(axes.end(), obbAxesB.begin(), obbAxesB.end());
	for (const auto& axisA : obbAxesA) {
		for (const auto& axisB : obbAxesB) {
			axes.push_back(Vector3::Cross(axisA, axisB));
		}
	}

	for (const auto& axis : axes) {
		if (axis.Length() < std::numeric_limits<float>::epsilon()) {
			continue;
		}

		Vector3 normalizedAxis = axis.Normalize();

		float obbAProjection = CalculateProjection(obbA, normalizedAxis, obbAxesA.data());
		float obbBProjection = CalculateProjection(obbB, normalizedAxis, obbAxesB.data());
		float distance = std::abs(Vector3::Dot(obbA.center - obbB.center, normalizedAxis));

		if (distance > obbAProjection + obbBProjection) {
			return false;
		}
	}

	return true;
}