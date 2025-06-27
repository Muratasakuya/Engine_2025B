#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Player/Input/Structures/PlayerAction.h>

// front
class Input;

//============================================================================
//	PlayerIInputDevice class
//============================================================================
class PlayerIInputDevice {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerIInputDevice() = default;
	virtual ~PlayerIInputDevice() = default;

	//--------- accessor -----------------------------------------------------

	// 連続入力
	virtual float GetVector(PlayerAction axis)  const = 0;

	// 単入力
	virtual bool IsPressed(PlayerAction button) const = 0;
	virtual bool IsTriggered(PlayerAction button) const = 0;
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	Input* input_;
};