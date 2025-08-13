#pragma once

//============================================================================
//	include
//============================================================================

//imgui
#include <imgui.h>

// 遷移状態
enum class TransitionState {

	Begin,   // 開始
	Load,    // 読み込み中
	LoadEnd, // 読み込み終了
	End      // 終了
};

//============================================================================
//	ITransition class
//============================================================================
class ITransition {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ITransition() = default;
	virtual ~ITransition() = default;

	virtual void Init() = 0;

	virtual void Update() = 0;

	virtual void BeginUpdate() = 0;

	virtual void LoadUpdate() = 0;
	virtual void LoadEndUpdate() = 0;

	virtual void EndUpdate() = 0;

	virtual void ImGui() = 0;

	//--------- accessor -----------------------------------------------------

	void SetLoadingFinished(bool finished) { loadingFinished_ = finished; }

	TransitionState GetState() const { return state_; }
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 遷移状態
	TransitionState state_;

	// 読み込みが終了したかどうか
	bool loadingFinished_;
};