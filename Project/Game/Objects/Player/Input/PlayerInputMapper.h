#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Player/Input/Interface/PlayerIInputDevice.h>

// c++
#include <memory>
#include <vector>
#include <algorithm>

//============================================================================
//	PlayerInputMapper class
//============================================================================
class PlayerInputMapper {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerInputMapper() = default;
	~PlayerInputMapper() = default;

	// 使用する入力デバイスを追加
	void AddDevice(std::unique_ptr<PlayerIInputDevice> device);

	//--------- accessor -----------------------------------------------------

	float GetVector(PlayerAction action) const;

	bool IsPressed(PlayerAction button) const;
	bool IsTriggered(PlayerAction button) const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 使用する入力デバイス
	std::vector<std::unique_ptr<PlayerIInputDevice>> devices_;
};