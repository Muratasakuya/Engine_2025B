#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Collision/CollisionBody.h>

// c++
#include <string>
#include <optional>

//============================================================================
//	Collider class
//============================================================================
class Collider {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	Collider() = default;
	~Collider() = default;

	// collision
	CollisionBody* AddCollider(const CollisionShape::Shapes& shape);
	void RemoveCollider(CollisionBody* collisionBody);

	virtual void OnCollisionEnter([[maybe_unused]] const CollisionBody* collisionBody) {};

	virtual void OnCollisionStay([[maybe_unused]] const CollisionBody* collisionBody) {};

	virtual void OnCollisionExit([[maybe_unused]] const CollisionBody* collisionBody) {};
};