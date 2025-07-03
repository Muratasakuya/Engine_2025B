#include "FollowCameraKeyInput.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Input/Input.h>

//============================================================================
//	FollowCameraKeyInput classMethods
//============================================================================

float FollowCameraKeyInput::GetVector(FollowCameraAction axis) const {

	const Vector2 value = input_->GetMouseMoveValue();
	if (axis == FollowCameraAction::RotateX) {

		return value.x;
	}
	if (axis == FollowCameraAction::RotateY) {

		return value.y;
	}
	return 0.0f;
}

bool FollowCameraKeyInput::IsPressed(FollowCameraAction button) const {

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

bool FollowCameraKeyInput::IsTriggered(FollowCameraAction button) const {

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