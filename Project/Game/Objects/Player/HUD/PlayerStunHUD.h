#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Base/GameTimerDisplay.h>

// c++
#include <array>

//============================================================================
//	PlayerStunHUD class
//============================================================================
class PlayerStunHUD {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	PlayerStunHUD() = default;
	~PlayerStunHUD() = default;

	void Init();

	void Update();

	void ImGui();

	//--------- accessor -----------------------------------------------------

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	struct ChainInput {

		std::unique_ptr<GameEntity2D> rightChain; // 切り替え入力: 右
		std::unique_ptr<GameEntity2D> leftChain;  // 切り替え入力: 左
		std::unique_ptr<GameEntity2D> cancel;     // キャンセル入力

		void Init(const std::string& rightTex, const std::string& leftTex,
			const std::string& cancelTex);
	};

	//--------- variables ----------------------------------------------------

	std::unique_ptr<GameTimerDisplay> restTimerDisplay_; // 時間表示

	// 切り替え先アイコン表示
	static const uint32_t iconCount_ = 2;
	std::array<std::unique_ptr<GameEntity2D>, iconCount_> stunChainIcon_;

	// 入力表示
	ChainInput keyInput_;     // キー表示
	ChainInput gamepadInput_; // パッド表示

	std::unique_ptr<GameEntity2D> progressBarBackground_; // 経過率背景
	std::unique_ptr<GameEntity2D> progressBar_;           // 経過率
	std::unique_ptr<GameEntity2D> chainAttackText_;       // 文字

	float restTimer_; // 時間経過
	float restTime_;  // スタン選択時間

	// parameters
	// 座標
	const float centerTranslationX_ = 960.0f;
	float timerTranslationX_;  // タイマーのX座標
	float barTranslationY_;    // バーのY座標
	float timerTranslationY_;  // タイマーのY座標
	float cancelTranslationY_; // キャンセルのY座標
	float endOffsetX_;         // 両端のオフセットX
	float timerOffsetX_;       // タイマーの間の間隔X

	// サイズ
	Vector2 iconSize_;        // アイコンのサイズ
	Vector2 progressBarBackgroundSize_; // 経過率の背景サイズ
	Vector2 progressBarSize_;           // 経過率のサイズ
	Vector2 chainAttackTextSize_;       // 文字サイズ
	Vector2 chainInputSize_;  // 入力キーのサイズ
	Vector2 cancelInputSize_; // キャンセルのサイズ
	Vector2 timerSize_;       // タイマーのサイズ
	Vector2 timerSymbolSize_; // タイマーの記号のサイズ

	// test
	bool checkAnimation_; // デバッグ時のみ有効のフラグ

	//--------- functions ----------------------------------------------------

	// json
	void ApplyJson();
	void SaveJson();

	// update
	void UpdateLayout();
	void UpdateSize();
};