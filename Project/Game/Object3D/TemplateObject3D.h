#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Component/Base/IComponent.h>
#include <Engine/Component/User/Collider.h>

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
	void OnCollisionEnter(const ColliderComponent* collider) override;

	void OnCollisionStay(const ColliderComponent* collider) override;

	void OnCollisionExit(const ColliderComponent* collider) override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	EntityID modelIdA_;
	EntityID modelIdB_;

	// collision
	ColliderComponent* colliderA_;
	ColliderComponent* colliderB_;

	//--------- functions ----------------------------------------------------

	void UpdateCollision();
};