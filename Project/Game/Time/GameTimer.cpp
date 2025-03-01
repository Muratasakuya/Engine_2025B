#include "GameTimer.h"

//============================================================================
//	include
//============================================================================
#include <Lib/Adapter/Easing.h>

// imgui
#include <imgui.h>

//============================================================================
//	GameTimer classMethods
//============================================================================

std::chrono::steady_clock::time_point GameTimer::lastFrameTime_ = std::chrono::steady_clock::now();
float GameTimer::deltaTime_ = 0.0f;
float GameTimer::timeScale_ = 1.0f;
float GameTimer::lerpSpeed_ = 1.8f;
float GameTimer::waitTimer_ = 0.0f;
float GameTimer::waitTime_ = 0.08f;
bool GameTimer::returnScaleEnable_ = true;

void GameTimer::Update() {

	auto currentFrameTime = std::chrono::steady_clock::now();
	std::chrono::duration<float> elapsedTime = currentFrameTime - lastFrameTime_;
	deltaTime_ = elapsedTime.count();

	lastFrameTime_ = currentFrameTime;

	if (returnScaleEnable_) {
		// timeScaleを1.0fに戻す処理
		if (timeScale_ != 1.0f) {

			// 硬直させる
			waitTimer_ += deltaTime_;
			if (waitTimer_ >= waitTime_) {

				float t = lerpSpeed_ * deltaTime_;
				float easedT = EaseOutExpo(t);

				timeScale_ += (1.0f - timeScale_) * easedT;
				if (std::abs(1.0f - timeScale_) < 0.01f) {
					timeScale_ = 1.0f;

					waitTimer_ = 0.0f;
				}
			}
		}
	}
}
void GameTimer::ImGui() {

	ImGui::SeparatorText("Performance");
	ImGui::Text("frameRate:       %.1f fps", ImGui::GetIO().Framerate); //* フレームレート情報
	ImGui::Text("deltaTime:       %.3f s", deltaTime_);                 //* ΔTime
	ImGui::Text("scaledDeltaTime: %.3f s", GetScaledDeltaTime());       //* ScaledΔTime
	ImGui::SeparatorText("TimeScale");

	ImGui::PushItemWidth(168.0f);
	ImGui::DragFloat("timeScale", &timeScale_, 0.01f);
	ImGui::DragFloat("lerpSpeed", &lerpSpeed_, 0.01f);
	ImGui::DragFloat("waitTime", &waitTime_, 0.01f);
	ImGui::PopItemWidth();
}