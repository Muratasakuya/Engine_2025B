#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Object3D/Player/Behavior/PlayerBehaviorType.h>

// front
class BasePlayerParts;

//============================================================================
//	IPlayerBehavior class
//============================================================================
class IPlayerBehavior {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	IPlayerBehavior() = default;
	virtual ~IPlayerBehavior() = default;

	virtual void Execute(BasePlayerParts* part) = 0;
};