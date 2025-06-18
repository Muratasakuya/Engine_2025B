#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Entity/GameEntity3D.h>

//============================================================================
//	FieldCrossMarkWall class
//============================================================================
class FieldCrossMarkWall :
	public GameEntity3D {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	FieldCrossMarkWall() = default;
	~FieldCrossMarkWall() = default;

	void Update() override;

	/*-------- collision ----------*/

	// 衝突コールバック関数
	void OnCollisionEnter([[maybe_unused]] const CollisionBody* collisionBody) override;
	void OnCollisionStay([[maybe_unused]] const CollisionBody* collisionBody) override;
	void OnCollisionExit([[maybe_unused]] const CollisionBody* collisionBody) override;

	//--------- accessor -----------------------------------------------------

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------



	//--------- functions ----------------------------------------------------

};