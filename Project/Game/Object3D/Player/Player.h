#pragma once

//============================================================================
//	include
//============================================================================

// 各parts
#include <Game/Object3D/Player/Parts/PlayerPartsController.h>
// behavior
#include <Game/Object3D/Player/Behavior/PlayerBehaviorController.h>

//============================================================================
//	Player class
//============================================================================
class Player {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	Player() = default;
	~Player() = default;

	void Init(FollowCamera* followCamera);

	void Update();

	//--------- accessor -----------------------------------------------------

	const Transform3DComponent& GetTransform() const { return partsController_->GetTransform(); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 各parts
	std::unique_ptr<PlayerPartsController> partsController_;

	// behavior
	std::unique_ptr<PlayerBehaviorController> behaviorController_;
};