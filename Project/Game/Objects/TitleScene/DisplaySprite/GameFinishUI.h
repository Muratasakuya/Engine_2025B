#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Base/GameButton.h>

//============================================================================
//	GameFinishUI class
//============================================================================
class GameFinishUI {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	GameFinishUI() = default;
	~GameFinishUI() = default;

	void Init();

	void Update();

	// editor
	void ImGui();

	// json
	void ApplyJson(const Json& data);
	void SaveJson(Json& data);

	//--------- accessor -----------------------------------------------------

	// ゲーム終了フラグ
	bool IsSelectFinish() const;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	// 状態
	enum class State {

		Power,  // 電源ボタン表示
		Select, // 終了するかキャンセルか
		Finish  // 終了
	};

	//--------- variables ----------------------------------------------------

	// 現在の状態
	State currentState_;

	// 表示するスプライト
	// 常に表示
	std::unique_ptr<GameButton> powerIcon_; // 電源アイコン

	// 状態に応じて表示
	std::unique_ptr<GameObject2D> askFinish_;           // 終了しますか表示
	std::unique_ptr<GameObject2D> askFinishBackground_; // 終了しますか表示の背景

	std::unique_ptr<GameObject2D> selectCancel_; // キャンセル
	std::unique_ptr<GameObject2D> selectOK_;     // OK -> Finish

	// parameters
	float selectButtonSpacing_; // ボタンの間の距離

	//--------- functions ----------------------------------------------------

	// init
	void InitSprites();
	void SetSpritePos();

	// update
	void UpdateState();
};