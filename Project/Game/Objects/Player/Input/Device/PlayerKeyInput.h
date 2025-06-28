#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Player/Input/Interface/PlayerIInputDevice.h>

//============================================================================
//	PlayerKeyInput class
//============================================================================
class PlayerKeyInput :
	public PlayerIInputDevice {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerKeyInput(Input* input) { input_ = input; }
	~PlayerKeyInput() = default;

	//--------- accessor -----------------------------------------------------

	// 連続入力
	float GetVector(PlayerAction axis)  const override;

	// 単入力
	bool IsPressed(PlayerAction button) const override;
	bool IsTriggered(PlayerAction button) const override;
};