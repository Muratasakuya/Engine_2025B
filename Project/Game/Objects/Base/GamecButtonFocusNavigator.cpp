#include "GamecButtonFocusNavigator.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Input/Input.h>
#include <Game/Objects/Base/GameButton.h>

//============================================================================
//	GamecButtonFocusNavigator classMethods
//============================================================================

void GamecButtonFocusNavigator::Init(ButtonFocusGroup group, const std::vector<GameButton*>& items) {

	// 初期化、設定
	group_ = group;
	items_ = items;
	index_ = 0;
	ApplyVisuals();
}

void GamecButtonFocusNavigator::SetGroup(ButtonFocusGroup group,
	const std::vector<GameButton*>& items, size_t defaultIndex) {

	// 初期化、設定
	group_ = group;
	items_ = items;
	index_ = (defaultIndex < items_.size()) ? defaultIndex : 0;
	ApplyVisuals();
}

void GamecButtonFocusNavigator::MoveLeft() {

	// indexを左に進める
	if (!items_.empty()) {

		index_ = (index_ + items_.size() - 1) % items_.size(); ApplyVisuals();
	}
}

void GamecButtonFocusNavigator::MoveRight() {

	// indexを右に進める
	if (!items_.empty()) {

		index_ = (index_ + 1) % items_.size(); ApplyVisuals();
	}
}

void GamecButtonFocusNavigator::Confirm() {

	if (onConfirm_) {

		onConfirm_(group_, static_cast<int>(index_));
	}
}

void GamecButtonFocusNavigator::ApplyVisuals() {

	for (size_t i = 0; i < items_.size(); ++i) {

		if (!items_[i]) {
			continue;
		}
		// アクティブ状態を設定
		items_[i]->SetFocusActive(i == index_);
	}
}

void GamecButtonFocusNavigator::Update() {

	Input* input = Input::GetInstance();

	// マウス(キーボード操作も含む)かパッド操作か判定
	const auto type = input->GetType();
	if (type == InputType::Keyboard || type == InputType::GamePad) {

		for (const auto& button : items_) {
			if (button) {

				// 判定が競合しないようにする
				button->SetEnableCollision(false);
			}
		}

		// 左右入力判定
		const bool left = input->TriggerGamepadButton(GamePadButtons::ARROW_LEFT) ||
			input->TriggerKey(DIK_LEFT);
		const bool right = input->TriggerGamepadButton(GamePadButtons::ARROW_RIGHT) ||
			input->TriggerKey(DIK_RIGHT);
		if (left) {
			MoveLeft();
		}
		if (right) {
			MoveRight();
		}

		// 左スティック入力判定
		const float lx = input->GetLeftStickVal().x;
		if (std::fabs(lx) < input->GetDeadZone()) {

			axisLatched_ = false;
		}
		if (!axisLatched_) {
			if (lx <= -input->GetDeadZone()) {

				MoveLeft();
				axisLatched_ = true;
			}
			if (lx >= input->GetDeadZone()) {

				MoveRight();
				axisLatched_ = true;
			}
		}

		// 決定入力判定
		const bool decide =
			input->TriggerGamepadButton(GamePadButtons::A) ||
			input->TriggerKey(DIK_RETURN) ||
			input->TriggerKey(DIK_SPACE);
		if (decide) {

			Confirm();
		}
	}
}

void GamecButtonFocusNavigator::SetFocusIndex(size_t index) {

	// 外部からフォーカスを変更する
	index_ = (index < items_.size()) ? index : 0; ApplyVisuals();
}