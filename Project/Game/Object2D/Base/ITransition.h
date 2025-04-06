#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Component/Base/IComponent.h>

//imgui
#include <imgui.h>

// 遷移状態
enum class TransitionState {

	Begin, // 開始
	Wait,  // 待ち時間
	End    // 終了
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

	virtual void WaitUpdate() = 0;

	virtual void EndUpdate() = 0;

	virtual void ImGui() = 0;

	//--------- accessor -----------------------------------------------------

	TransitionState GetState() const { return state_; }
protected:
	//========================================================================
	//	protected Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 遷移状態
	TransitionState state_;
};