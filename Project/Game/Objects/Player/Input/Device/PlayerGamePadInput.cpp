#include "PlayerGamePadInput.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Input/Input.h>

//============================================================================
//	PlayerGamePadInput classMethods
//============================================================================

float PlayerGamePadInput::GetVector(PlayerAction axis) const {

	// 左スティック入力
	const Vector2 value = input_->GetLeftStickVal();
	switch (axis) {
	case PlayerAction::MoveX: {

		return value.x;
	}
	case PlayerAction::MoveZ: {

		return value.y;
	}
	}
	return 0.0f;
}

bool PlayerGamePadInput::IsPressed(PlayerAction button) const {

	switch (button) {
	case PlayerAction::Dash: {

		return input_->PushGamepadButton(GamePadButtons::A);
	}
	case PlayerAction::Attack: {

		return input_->PushGamepadButton(GamePadButtons::X);
	}
	case PlayerAction::Skill: {

		return input_->PushGamepadButton(GamePadButtons::Y);
	}
	case PlayerAction::Special: {

		return input_->PushGamepadButton(GamePadButtons::RIGHT_TRIGGER);
	}
	}
	return false;
}

bool PlayerGamePadInput::IsTriggered(PlayerAction button) const {

	switch (button) {
	case PlayerAction::Dash: {

		return input_->TriggerGamepadButton(GamePadButtons::A);
	}
	case PlayerAction::Attack: {

		return input_->TriggerGamepadButton(GamePadButtons::X);
	}
	case PlayerAction::Skill: {

		return input_->TriggerGamepadButton(GamePadButtons::Y);
	}
	case PlayerAction::Special: {

		return input_->TriggerGamepadButton(GamePadButtons::RIGHT_TRIGGER);
	}
	}
	return false;
}