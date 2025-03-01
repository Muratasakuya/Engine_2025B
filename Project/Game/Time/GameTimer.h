#pragma once

//============================================================================
//	include
//============================================================================

// c++
#include <cstdint>
#include <chrono>

//============================================================================
//	GameTimer class
//============================================================================
class GameTimer {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	GameTimer() = default;
	~GameTimer() = default;

	static void Update();

	//--------- accessor -----------------------------------------------------

	static float GetDeltaTime() { return deltaTime_; }
	static float GetScaledDeltaTime() { return deltaTime_ * timeScale_; }

	static float GetTimeScale() { return timeScale_; }

	static void SetTimeScale(float timeScale) { timeScale_ = timeScale; }

	static void SetReturnScaleEnable(bool enable) { returnScaleEnable_ = enable; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	static std::chrono::steady_clock::time_point lastFrameTime_;

	static float deltaTime_;

	static float timeScale_;

	static float lerpSpeed_;

	static float waitTimer_;
	static float waitTime_;

	static bool returnScaleEnable_;
};