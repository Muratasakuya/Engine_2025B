#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Lib/ComponentStructures.h>
#include <Engine/Component/ColliderComponent.h>

// c++
#include <string>
#include <optional>
#include <typeinfo>

//============================================================================
//	BaseGameObject class
//============================================================================
class BaseGameObject {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	BaseGameObject() = default;
	~BaseGameObject() = default;

	// collision
	ColliderComponent* AddCollider(const CollisionShape::Shapes& shape);
	void RemoveCollider(ColliderComponent* collider);

	virtual void OnCollisionEnter(
		[[maybe_unused]] const ColliderComponent* collider,
		[[maybe_unused]] const ColliderID colliderId) {};

	virtual void OnCollisionStay(
		[[maybe_unused]] const ColliderComponent* collider,
		[[maybe_unused]] const ColliderID colliderId) {};

	virtual void OnCollisionExit(
		[[maybe_unused]] const ColliderComponent* collider,
		[[maybe_unused]] const ColliderID colliderId) {};

	//--------- accessor -----------------------------------------------------

	std::string GetObjectName() const;
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

private:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ColliderID colliderId_ = 0;

	//--------- functions ----------------------------------------------------

};