#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/ColliderComponent.h>

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
	ColliderComponent* AddCollider(const CollisionShape::Shapes& shape);
	void RemoveCollider(ColliderComponent* collider);

	virtual void OnCollisionEnter([[maybe_unused]] const ColliderComponent* collider) {};

	virtual void OnCollisionStay([[maybe_unused]] const ColliderComponent* collider) {};

	virtual void OnCollisionExit([[maybe_unused]] const ColliderComponent* collider) {};
};