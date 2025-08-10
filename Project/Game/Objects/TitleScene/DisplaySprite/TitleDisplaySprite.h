#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>
#include <Game/Objects/Base/GamecButtonFocusNavigator.h>
#include <Game/Objects/TitleScene/DisplaySprite/GameFinishUI.h>

//============================================================================
//	TitleDisplaySprite class
//============================================================================
class TitleDisplaySprite :
	public IGameEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	TitleDisplaySprite() :IGameEditor("TitleDisplaySprite") {}
	~TitleDisplaySprite() = default;

	void Init();

	void Update();

	void ImGui() override;

	//--------- accessor -----------------------------------------------------

	// ゲーム終了フラグ
	bool IsSelectFinish() const { return finishUI_->IsSelectFinish(); }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 表示するスプライト
	std::unique_ptr<GameObject2D> name_;  // 名前
	std::unique_ptr<GameButton> start_;   // 開始文字

	std::unique_ptr<GameFinishUI> finishUI_; // 終了表示

	// パッド操作入力管理
	std::unique_ptr<GamecButtonFocusNavigator> buttonFocusNavigator_;
	ButtonFocusGroup currentFocusGroup_;

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();

	// init
	void InitSprites();
	void InitNavigator();
	void SetSpritePos();
	
	// update
	void UpdateInputGamepad();
};