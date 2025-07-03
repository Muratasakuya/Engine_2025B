#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Camera/Follow/Input/Structures/FollowCameraAction.h>

// front
class Input;

//============================================================================
//	FollowCameraIInputDevice class
//============================================================================
class FollowCameraIInputDevice {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	FollowCameraIInputDevice() = default;
	virtual ~FollowCameraIInputDevice() = default;

	//--------- accessor -----------------------------------------------------

	// 連続入力
	virtual float GetVector(FollowCameraAction axis)  const = 0;

	// 単入力
	virtual bool IsPressed(FollowCameraAction button) const = 0;
	virtual bool IsTriggered(FollowCameraAction button) const = 0;
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Input* input_;
};