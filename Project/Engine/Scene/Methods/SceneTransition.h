#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Objects/Base/ITransition.h>

// c++
#include <memory>

//============================================================================
//	SceneTransition class
//============================================================================
class SceneTransition {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SceneTransition() = default;
	~SceneTransition() = default;

	void Init();

	void Update();

	//--------- accessor -----------------------------------------------------

	void SetTransition(std::unique_ptr<ITransition> transition);
	void SetTransition();
	void SetResetBeginTransition() { isBeginTransitionFinished_ = false; }

	bool IsTransition() const { return isTransition_; }

	bool IsBeginTransitionFinished() const { return isBeginTransitionFinished_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	// 遷移フラグ
	bool isTransition_;

	// 次のSceneに進めるフラグ
	bool isBeginTransitionFinished_;

	// 遷移状態
	TransitionState state_;

	// 遷移を行うクラス
	std::unique_ptr<ITransition> transition_;
};