#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Editor/Base/IGameEditor.h>
#include <Engine/Utility/StateTimer.h>
#include <Game/Objects/Base/GameTimerDisplay.h>

//============================================================================
//	GameResultDisplay class
//============================================================================
class GameResultDisplay :
	public IGameEditor {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	GameResultDisplay() :IGameEditor("GameResultDisplay") {}
	~GameResultDisplay() = default;

	void Init();

	void Update();
	void Measurement();

	void ImGui() override;

	//--------- accessor -----------------------------------------------------

	// 表示開始
	void StartDisplay();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	// 現在の状態
	enum class State {

		None,
		BeginTime,   // 時間表示
		Result       // クリア表示
	};

	//--------- variables ----------------------------------------------------

	// 現在の状態
	State  currentState_;

	// 経過時間
	StateTimer resultTimer_;

	// 使用するオブジェクト
	std::unique_ptr<GameTimerDisplay> resultTime_; // 時間表示

	// parameters
	// BeginTime
	StateTimer randomDisplayTimer_; // 時間をランダムで表示する時間
	Vector2 timerTranslation_; // タイマーの座標
	float timerOffsetX_;       // タイマーの間の間隔X
	Vector2 timerSize_;        // タイマーのサイズ
	Vector2 timerSymbolSize_;  // タイマーの記号のサイズ

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();

	// update
	void UpdateBeginTime();
	void UpdateResult();
};