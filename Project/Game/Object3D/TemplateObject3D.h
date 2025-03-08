#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Base/BaseGameObject.h>

//============================================================================
//	TemplateObject3D class
//============================================================================
class TemplateObject3D :
	public BaseGameObject {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	TemplateObject3D();
	~TemplateObject3D() = default;

	void Update();

	//* collision *//

	void OnCollisionEnter(const ColliderComponent* collider, const ColliderID colliderId) override;

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ColliderComponent* colliderA_;
	ColliderComponent* colliderB_;

	//--------- functions ----------------------------------------------------

	void UpdateCollision();
};