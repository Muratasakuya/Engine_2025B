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

		Vector3 center;
		float radius;

		static Sphere Default() {
			Sphere sphere = {
				.center = Vector3::AnyInit(0.0f),
				.radius = 1.0f
			};
			return sphere;
		};
	};

	struct OBB {

		Vector3 center;
		Vector3 size;
		Quaternion rotate;

		static OBB Default() {
			OBB obb = {
				.center = Vector3::AnyInit(0.0f),
				.size = Vector3::AnyInit(1.0f),
				.rotate = Quaternion::IdentityQuaternion()
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

	bool SphereToSphere(const CollisionShape::Sphere& sphereA, const CollisionShape::Sphere& sphereB);

	bool SphereToOBB(const CollisionShape::Sphere& sphere, const CollisionShape::OBB& obb);

	bool OBBToOBB(const CollisionShape::OBB& obbA, const CollisionShape::OBB& obbB);
}