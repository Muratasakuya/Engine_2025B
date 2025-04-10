#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Collision/Collider.h>

//============================================================================
//	TemplateObject3D class
//============================================================================
class TemplateObject3D :
	public Collider {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	TemplateObject3D();
	~TemplateObject3D();

	void Update();

	// collision
	void OnCollisionEnter(const CollisionBody* collisionBody) override;

	void OnCollisionStay(const CollisionBody* collisionBody) override;

	void OnCollisionExit(const CollisionBody* collisionBody) override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	uint32_t modelIdA_;
	uint32_t modelIdB_;

	// collision
	CollisionBody* collisionBodyA_;
	CollisionBody* collisionBodyB_;

	//--------- functions ----------------------------------------------------

	void UpdateCollision();
};