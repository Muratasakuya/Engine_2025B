#include "PlayerKeyInput.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Input/Input.h>

//============================================================================
//	PlayerKeyInput classMethods
//============================================================================

float PlayerKeyInput::GetVector(PlayerAction axis) const {

	float vector = 0.0f;
	if (axis == PlayerAction::MoveX) {
		if (input_->PushKey(DIK_A)) {

			vector -= 1.0f;
		} else if (input_->PushKey(DIK_D)) {

			vector += 1.0f;
		}
		return vector;
	}
	if (axis == PlayerAction::MoveZ) {

		if (input_->PushKey(DIK_S)) {

			vector -= 1.0f;
		} else if (input_->PushKey(DIK_W)) {

			vector += 1.0f;
		}
		return vector;
	}
	return 0.0f;
}

bool PlayerKeyInput::IsPressed(PlayerAction button) const {

	switch (button) {
	case PlayerAction::Dash: {

		return input_->PushMouseRight();
	}
	case PlayerAction::Attack: {

		return input_->PushMouseLeft();
	}
	}
	return false;
}

bool PlayerKeyInput::IsTriggered(PlayerAction button) const {

	switch (button) {
	case PlayerAction::Dash: {

		return input_->TriggerMouseRight();
	}
	case PlayerAction::Attack: {

		return input_->TriggerMouseLeft();
	}
	}
	return false;
}