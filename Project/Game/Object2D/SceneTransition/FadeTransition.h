#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Object2D/Base/ITransition.h>
#include <Game/Algorithm/StateTimer.h>

//============================================================================
//	FadeTransition class
//============================================================================
class FadeTransition :
	public ITransition {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	FadeTransition() = default;
	~FadeTransition() = default;

	void Init() override;

	void Update() override;

	void BeginUpdate() override;

	void WaitUpdate() override;

	void EndUpdate() override;

	void ImGui() override;
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	uint32_t fadeSpriteId_;

	StateTimer beginTimer_;
	StateTimer waitTimer_;
	StateTimer endTimer_;
};