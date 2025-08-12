#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/TitleScene/DisplaySprite/TitleDisplaySprite.h>

//============================================================================
//	TitleController class
//============================================================================
class TitleController {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	TitleController() = default;
	~TitleController() = default;

	void Init();

	void Update();

	//--------- accessor -----------------------------------------------------

	// ゲーム終了フラグ
	bool IsSelectFinish() const { return displaySprite_->IsSelectFinish(); }

	// ゲーム遷移フラグ
	bool IsGameStart() const { return displaySprite_->IsGameStart(); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 表示スプライト
	std::unique_ptr<TitleDisplaySprite> displaySprite_;
};