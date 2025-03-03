#pragma once

//============================================================================
//	include
//============================================================================
#include <Lib/MathUtils/MathUtils.h>

// c++
#include <variant>

//============================================================================
//	CollisionShape namespace
//============================================================================
namespace CollisionShape {

	struct Sphere {

		float radius;

		static Sphere Default() {
			Sphere sphere = {
				.radius = 1.0f
			};
			return sphere;
		};
	};

	struct OBB {

		Vector3 size;
		Vector3 center;
		Quaternion rotate;

		static OBB Default() {
			OBB obb = {
				.size = {1.0f,1.0f,1.0f},
				.center = {0.0f,0.0f,0.0f},
				.rotate = {0.0f,0.0f,0.0f}
			};
			return obb;
		};
	};

	using Shapes = std::variant<Sphere, OBB>;

};

enum class ShapeType {

	Type_Sphere,
	Type_OBB
};

//============================================================================
//	Collision namespace
//============================================================================
namespace Collision {

	bool SphereToSphere(const CollisionShape::Sphere& sphereA, const CollisionShape::Sphere& sphereB,
		const Vector3& centerA, const Vector3& centerB);

	bool SphereToOBB(const CollisionShape::Sphere& sphere, const CollisionShape::OBB& obb,
		const Vector3& sphereCenter);

	bool OBBToOBB(const CollisionShape::OBB& obbA, const CollisionShape::OBB& obbB);
}