#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Object/Base/GameObject3D.h>

//============================================================================
//	FieldCrossMarkWall class
//============================================================================
class FieldCrossMarkWall :
	public GameObject3D {
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