#include "StateTimer.h"

//============================================================================
//	include
//============================================================================
#include <Game/Time/GameTimer.h>

//============================================================================
//	StateTimer classMethods
//============================================================================

void StateTimer::Update() {

	current_ += GameTimer::GetDeltaTime();
	t_ = current_ / target_;
	easedT_ = EasedValue(easeingType_, t_);
}

void StateTimer::Reset() {

	current_ = 0.0f;
	t_ = 0.0f;
	easedT_ = 0.0f;
}

bool StateTimer::IsReached() const {

	// 現在の時間が目標時間に達したら
	return current_ > target_;
}