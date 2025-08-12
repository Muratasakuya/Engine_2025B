#pragma once

//============================================================================
//	include
//============================================================================

// c++
#include <vector>
#include <functional>
// front
class GameButton;

//============================================================================
//	GamecButtonFocusNavigator enum class
//============================================================================

// 選択しているボタン
enum class ButtonFocusGroup {

	Top,
	Select
};

//============================================================================
//	GamecButtonFocusNavigator class
//============================================================================
class GamecButtonFocusNavigator {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	GamecButtonFocusNavigator() = default;
	~GamecButtonFocusNavigator() = default;

	void Init(ButtonFocusGroup group, const std::vector<GameButton*>& items);
	void SetGroup(ButtonFocusGroup group, const std::vector<GameButton*>& items, size_t defaultIndex = 0);

	void Update();

	//--------- accessor -----------------------------------------------------

	void SetOnConfirm(std::function<void(ButtonFocusGroup, int)> onConfirm) { onConfirm_ = std::move(onConfirm); }
	void SetFocusIndex(size_t index);
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	ButtonFocusGroup group_;

	bool axisLatched_; // 押しっぱなし入力回避
	bool hasFocus_;    // まだフォーカスされていない
	size_t defaultIndex_ = 0; // 最初に立てるインデックス
	size_t index_ = 0;        // 現在のインデックス

	std::vector<GameButton*> items_;
	std::function<void(ButtonFocusGroup, int)> onConfirm_;

	//--------- functions ----------------------------------------------------

	// helper
	void MoveLeft();
	void MoveRight();
	void Confirm();
	void ApplyVisuals();
	void ClearFocus();
};