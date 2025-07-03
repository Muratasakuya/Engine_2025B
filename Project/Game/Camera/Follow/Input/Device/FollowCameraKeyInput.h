#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Camera/Follow/Input/Interface/FollowCameraIInputDevice.h>

//============================================================================
//	FollowCameraKeyInput class
//============================================================================
class FollowCameraKeyInput :
	public FollowCameraIInputDevice {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	FollowCameraKeyInput(Input* input) { input_ = input; }
	~FollowCameraKeyInput() = default;

	//--------- accessor -----------------------------------------------------

	// 連続入力
	float GetVector(FollowCameraAction axis)  const override;

	// 単入力
	bool IsPressed(FollowCameraAction button) const override;
	bool IsTriggered(FollowCameraAction button) const override;
};