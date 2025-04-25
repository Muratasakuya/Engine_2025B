#include "IPlayerBehavior.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Input/Input.h>

//============================================================================
//	IPlayerBehavior classMethods
//============================================================================

void IPlayerBehavior::InputKey(Vector2& inputValue) {

	Input* input = Input::GetInstance();
	if (input->PushKey(DIK_W)) {

		inputValue.y += 1.0f;
	} else if (input->PushKey(DIK_S)) {

		inputValue.y -= 1.0f;
	}
	if (input->PushKey(DIK_D)) {

		inputValue.x += 1.0f;
	} else if (input->PushKey(DIK_A)) {

		inputValue.x -= 1.0f;
	}
}