#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Camera/Follow/Input/Interface/FollowCameraIInputDevice.h>

//============================================================================
//	FollowCameraGamePadInput class
//============================================================================
class FollowCameraGamePadInput :
	public FollowCameraIInputDevice {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	FollowCameraGamePadInput(Input* input) { input_ = input; }
	~FollowCameraGamePadInput() = default;

	//--------- accessor -----------------------------------------------------

	// 連続入力
	float GetVector(FollowCameraAction axis)  const override;

	// 単入力
	bool IsPressed(FollowCameraAction button) const override;
	bool IsTriggered(FollowCameraAction button) const override;
};