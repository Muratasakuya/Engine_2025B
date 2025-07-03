#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Camera/Follow/Input/Interface/FollowCameraIInputDevice.h>

// c++
#include <memory>
#include <vector>
#include <algorithm>

//============================================================================
//	FollowCameraInputMapper class
//============================================================================
class FollowCameraInputMapper {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	FollowCameraInputMapper() = default;
	~FollowCameraInputMapper() = default;

	// 使用する入力デバイスを追加
	void AddDevice(std::unique_ptr<FollowCameraIInputDevice> device);

	//--------- accessor -----------------------------------------------------

	float GetVector(FollowCameraAction action) const;

	bool IsPressed(FollowCameraAction button) const;
	bool IsTriggered(FollowCameraAction button) const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 使用する入力デバイス
	std::vector<std::unique_ptr<FollowCameraIInputDevice>> devices_;
};