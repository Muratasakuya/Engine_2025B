#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Player/Input/Interface/PlayerIInputDevice.h>

//============================================================================
//	PlayerGamePadInput class
//============================================================================
class PlayerGamePadInput :
	public PlayerIInputDevice {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerGamePadInput(Input* input) { input_ = input; }
	~PlayerGamePadInput() = default;

	//--------- accessor -----------------------------------------------------

	// 連続入力
	float GetVector(PlayerAction axis)  const override;

	// 単入力
	bool IsPressed(PlayerAction button) const override;
	bool IsTriggered(PlayerAction button) const override;
};