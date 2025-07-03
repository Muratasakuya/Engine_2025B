#include "FollowCameraGamePadInput.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Input/Input.h>

//============================================================================
//	FollowCameraGamePadInput classMethods
//============================================================================

float FollowCameraGamePadInput::GetVector(FollowCameraAction axis) const {

	const Vector2 value = Input::GetInstance()->GetRightStickVal() * 1.0f / 32767.0f;
	switch (axis) {
	case FollowCameraAction::RotateX: {

		return value.x;
	}
	case FollowCameraAction::RotateY: {

		return value.y;
	}
	}
	return 0.0f;
}

bool FollowCameraGamePadInput::IsPressed(FollowCameraAction button) const {

	switch (button) {
	case FollowCameraAction::RotateX: {
		return false;
	}
	case FollowCameraAction::RotateY: {
		return false;
	}
	}
	return false;
}

bool FollowCameraGamePadInput::IsTriggered(FollowCameraAction button) const {

	switch (button) {
	case FollowCameraAction::RotateX: {
		return false;
	}
	case FollowCameraAction::RotateY: {
		return false;
	}
	}
	return false;
}