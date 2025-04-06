#include "SceneTransition.h"

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>

//============================================================================
//	SceneTransition classMethods
//============================================================================

void SceneTransition::Init() {

	isTransition_ = false;
	isBeginTransitionFinished_ = false;

	state_ = TransitionState::Begin;
}

void SceneTransition::Update() {

	if (!isTransition_) {
		return;
	}

	assert(transition_);

	switch (state_) {
	case TransitionState::Begin: {

		transition_->BeginUpdate();
		if (transition_->GetState() == TransitionState::Wait) {

			// 次の状態へ遷移
			state_ = TransitionState::Wait;
		}
		break;
	}
	case TransitionState::Wait: {

		transition_->WaitUpdate();
		if (transition_->GetState() == TransitionState::End) {

			// SceneManagerに次に進めるフラグを通達
			// 次の状態へ遷移
			state_ = TransitionState::End;
			isBeginTransitionFinished_ = true;
		}
		break;
	}
	case TransitionState::End: {

		transition_->EndUpdate();
		if (transition_->GetState() == TransitionState::Begin) {

			// 最初の状態に戻す
			state_ = TransitionState::Begin;
			isTransition_ = false;
		}
		break;
	}
	}
	transition_->Update();
}

void SceneTransition::ImGui() {

	if (!isTransition_) {
		return;
	}

	transition_->ImGui();
}

void SceneTransition::SetTransition(std::unique_ptr<ITransition> transition) {

	transition_.reset();

	isTransition_ = true;
	transition_ = std::move(transition);
}

void SceneTransition::SetTransition() {

	isTransition_ = true;
}