#pragma once

//============================================================================
//	include
//============================================================================
#include <Game/Time/GameTimer.h>
#include <Lib/Adapter/Easing.h>
#include <Lib/MathUtils/Algorithm.h>
#include <Lib/MathUtils/MathUtils.h>

// c++
#include <format>
#include <optional>
// imgui
#include <imgui.h>

//============================================================================
//	SimpleAnimation class
//============================================================================
template <typename T>
class SimpleAnimation {
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- structure ----------------------------------------------------

	struct Loop {

		bool isLoop;           // ループするかどうか
		bool isStart;          // 動き出しフラグ
		bool isEnd;            // ループが終わったかどうか

		uint32_t maxCount;     // ループの回数
		float interval;        // ループの間隔

		uint32_t currentCount; // 現在のループの回数
		float currentInterval; // 現在のループの間隔

		void ImGui();
	};

	struct Time {

		bool useScaledDeltaTime;         // スケールされたdeltaTimeを使うかどうか
		float deltaTime;                 // deltaTime
		float elapsed;                   // 経過時間
		float end;                       // 終了時間
		float movedValueInterval;        // 動かす値の間隔
		float currentMovedValueInterval; // 現在の動かす値の間隔
		float currentT;                  // 現在のt値

		void ImGui();
	};

	struct Move {

		T start;               // 開始値
		T end;                 // 終了値
		T moveValue;           // 動かす値、currentTに沿わずendまで動く
		EasingType easingType; // イージングの種類

		void ImGui();
	};
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SimpleAnimation() = default;
	~SimpleAnimation() = default;

	void ImGui();

	// 0.0fから1.0fの間で補間された値を取得
	void LerpValue(T& value);
	// moveValue分endまで動く値を取得
	void MoveValue(T& value);

	// 動き出し開始
	void Start();
	// リセット
	void Reset();

	//--------- accessor -----------------------------------------------------

	// animationを行うかどうか
	bool IsStart() const { return loop_.isStart; };
	//--------- firend -------------------------------------------------------

	template <typename U>
	friend void swap(SimpleAnimation<U>& a, SimpleAnimation<U>& b) noexcept;

	//--------- variables ----------------------------------------------------

	Loop loop_; // ループの情報
	Time time_; // タイマーの情報
	Move move_; // 動かす値の情報
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	const float itemSize_ = 200.0f; // imguiのサイズ

	//--------- functions ----------------------------------------------------

	void UpdateElapsedTime();

	void UpdateLerpValue(T& value);
	void UpdateMoveValue(T& value);

	void UpdateLoop(T& value);
};

//============================================================================
//	SimpleAnimation templateMethods
//============================================================================

template<typename T>
inline void SimpleAnimation<T>::ImGui() {

	ImGui::PushItemWidth(itemSize_);

	loop_.ImGui();
	time_.ImGui();
	move_.ImGui();

	ImGui::PopItemWidth();
}

template<typename T>
inline void SimpleAnimation<T>::LerpValue(T& value) {

	// ループが開始していないときは何も処理をしない
	if (!loop_.isStart) {
		return;
	}

	// 経過時間を加算、t値を処理する
	UpdateElapsedTime();

	// 値の補間
	UpdateLerpValue(value);

	// ループ処理の更新
	UpdateLoop(value);
}

template<typename T>
inline void SimpleAnimation<T>::MoveValue(T& value) {

	// ループが開始していないときは何も処理をしない
	if (!loop_.isStart) {
		return;
	}

	// 経過時間を加算、t値を処理する
	UpdateElapsedTime();

	// 値の加算
	UpdateMoveValue(value);

	// ループ処理の更新
	UpdateLoop(value);
}

template<typename T>
inline void SimpleAnimation<T>::Start() {

	loop_.isStart = true;
	loop_.isEnd = false;
}

template<typename T>
inline void SimpleAnimation<T>::Reset() {

	loop_.isStart = false;
	loop_.isEnd = false;
	loop_.currentCount = 0;
	time_.elapsed = 0.0f;
	time_.currentT = 0.0f;
	time_.currentMovedValueInterval = 0.0f;
}

template<typename T>
inline void SimpleAnimation<T>::UpdateElapsedTime() {

	// ループが終了しているときは何も処理をしない
	if (loop_.isEnd) {
		return;
	}

	// スケールされたdeltaTimeを使うかどうかで分岐
	if (time_.useScaledDeltaTime) {

		time_.deltaTime = GameTimer::GetScaledDeltaTime();
	} else {

		time_.deltaTime = GameTimer::GetDeltaTime();
	}
	time_.elapsed += time_.deltaTime;

	// 現在の進捗、0.0fから1.0fの間でclampする
	time_.currentT = std::clamp(time_.elapsed / time_.end, 0.0f, 1.0f);
}

template<typename T>
inline void SimpleAnimation<T>::UpdateLerpValue(T& value) {

	// ループが終了しているときは何も処理をしない
	if (loop_.isEnd) {
		return;
	}

	// 補間する
	value = Algorithm::Lerp<T>(move_.start, move_.end,
		EasedValue(move_.easingType, time_.currentT));

	// 1ループ終了
	if (time_.currentT == 1.0f) {

		// ループの終了フラグを立てる
		loop_.isEnd = true;
	}
}

template<typename T>
inline void SimpleAnimation<T>::UpdateMoveValue(T& value) {

	// ループが終了しているときは何も処理をしない
	if (loop_.isEnd) {
		return;
	}

	// 動かすまでのインターバルを加算
	time_.currentMovedValueInterval += time_.deltaTime;
	// 動かす値の間隔を超えた時
	if (time_.currentMovedValueInterval >= time_.movedValueInterval) {

		// 動かす値を加算
		value += move_.moveValue;
		// 動かす値の間隔をリセット
		time_.currentMovedValueInterval = 0.0f;
	}

	// 1ループ終了
	if (value >= move_.end) {

		// ループの終了フラグを立てる
		loop_.isEnd = true;
	}
}

template<typename T>
inline void SimpleAnimation<T>::UpdateLoop(T& value) {

	if (!loop_.isLoop || !loop_.isEnd) {
		return;
	}

	// ループ間のインターバルを加算
	loop_.currentInterval += time_.deltaTime;

	// インターバル経過したら再開判定
	if (loop_.currentInterval >= loop_.interval) {

		// ループ回数が上限を超えていなければ
		if (loop_.currentCount < loop_.maxCount) {

			// ループ回数をインクリメント
			loop_.currentCount++;

			// ループ再開
			loop_.currentInterval = 0.0f;
			time_.elapsed = 0.0f;
			time_.currentT = 0.0f;
			loop_.isEnd = false;

			// 値をstartに戻す
			value = move_.start;
		} else {

			// ループ終了
			Reset();
		}
	}
}

template<typename T>
inline void SimpleAnimation<T>::Loop::ImGui() {

	ImGui::SeparatorText("Loop");

	// 終了フラグ
	ImGui::Text(std::format("isEnd: {}", isEnd).c_str());
	ImGui::Text(std::format("loopCount: {}", currentCount).c_str());
	ImGui::Checkbox("isLoop##Loop", &isLoop);

	ImGui::DragInt("maxCount##Loop", (int*)&maxCount, 1.0f, 0, 100);
	ImGui::DragFloat("interval##Loop", &interval, 0.01f, 0.0f, 10.0f);
}

template<typename T>
inline void SimpleAnimation<T>::Time::ImGui() {

	ImGui::SeparatorText("Time");

	// 経過時間
	ImGui::Text(std::format("elapsed: {}", elapsed).c_str());
	ImGui::Text(std::format("currentT: {}", currentT).c_str());
	ImGui::Text(std::format("currentMovedValueInterval: {}", currentMovedValueInterval).c_str());

	ImGui::Checkbox("useScaledDeltaTime##Time", &useScaledDeltaTime);
	ImGui::DragFloat("end##Time", &end, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("movedValueInterval##Time", &movedValueInterval, 0.001f, 0.0f, 10.0f);
}

template<typename T>
inline void SimpleAnimation<T>::Move::ImGui() {

	ImGui::SeparatorText("Move");

	if constexpr (std::is_same_v<T, float>) {

		ImGui::DragFloat("start##Move", &start, 0.01f);
		ImGui::DragFloat("end##Move", &end, 0.01f);
		ImGui::DragFloat("moveValue##Move", &moveValue, 0.01f);
	} else if constexpr (std::is_same_v<T, int>) {

		ImGui::DragInt("start##Move", &start, 1);
		ImGui::DragInt("end##Move", &end, 1);
		ImGui::DragInt("moveValue##Move", &moveValue, 1);
	} else if constexpr (std::is_same_v<T, Vector2>) {

		ImGui::DragFloat2("start##Move", &start.x, 0.01f);
		ImGui::DragFloat2("end##Move", &end.x, 0.01f);
		ImGui::DragFloat2("moveValue##Move", &moveValue.x, 0.01f);
	} else if constexpr (std::is_same_v<T, Vector3>) {

		ImGui::DragFloat3("start##Move", &start.x, 0.01f);
		ImGui::DragFloat3("end##Move", &end.x, 0.01f);
		ImGui::DragFloat3("moveValue##Move", &moveValue.x, 0.01f);
	} else if constexpr (std::is_same_v<T, Color>) {

		ImGui::ColorEdit4("start##Move", &start.a);
		ImGui::ColorEdit4("end##Move", &end.a);
		ImGui::ColorEdit4("moveValue##Move", &moveValue.a);
	}

	const char* easingOptions[] = {
			"EaseInSine", "EaseInQuad", "EaseInCubic", "EaseInQuart", "EaseInQuint", "EaseInExpo", "EaseInCirc", "EaseInBack", "EaseInBounce",
			"EaseOutSine", "EaseOutQuad", "EaseOutCubic", "EaseOutQuart", "EaseOutQuint", "EaseOutExpo", "EaseOutCirc", "EaseOutBack", "EaseOutBounce",
			"EaseInOutSine", "EaseInOutQuad", "EaseInOutCubic", "EaseInOutQuart", "EaseInOutQuint", "EaseInOutExpo", "EaseInOutCirc", "EaseInOutBounce"
	};

	// Enum values
	const char* easeInOptions[] = {
		"EaseInSine", "EaseInQuad", "EaseInCubic", "EaseInQuart", "EaseInQuint", "EaseInExpo", "EaseInCirc", "EaseInBack", "EaseInBounce"
	};
	const char* easeOutOptions[] = {
		"EaseOutSine", "EaseOutQuad", "EaseOutCubic", "EaseOutQuart", "EaseOutQuint", "EaseOutExpo", "EaseOutCirc", "EaseOutBack", "EaseOutBounce"
	};
	const char* easeInOutOptions[] = {
		"EaseInOutSine", "EaseInOutQuad", "EaseInOutCubic", "EaseInOutQuart", "EaseInOutQuint", "EaseInOutExpo", "EaseInOutCirc", "EaseInOutBounce"
	};

	int easingIndex = static_cast<int>(easingType);
	if (ImGui::BeginCombo("EasingType", easingOptions[easingIndex])) {

		// EaseIn
		if (ImGui::BeginCombo("EaseIn", "")) {
			for (int i = 0; i < IM_ARRAYSIZE(easeInOptions); i++) {
				const bool isSelected = (easingIndex == i);
				if (ImGui::Selectable(easeInOptions[i], isSelected)) {

					easingType = static_cast<EasingType>(i);
					easingIndex = i;
				}
				if (isSelected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		// EaseOut
		if (ImGui::BeginCombo("EaseOut", "")) {
			for (int i = 0; i < IM_ARRAYSIZE(easeOutOptions); i++) {
				const bool isSelected = (easingIndex == i + IM_ARRAYSIZE(easeInOptions));
				if (ImGui::Selectable(easeOutOptions[i], isSelected)) {

					easingType = static_cast<EasingType>(i + IM_ARRAYSIZE(easeInOptions));
					easingIndex = i + IM_ARRAYSIZE(easeInOptions);
				}
				if (isSelected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		// EaseInOut
		if (ImGui::BeginCombo("EaseInOut", "")) {
			for (int i = 0; i < IM_ARRAYSIZE(easeInOutOptions); i++) {
				const bool isSelected = (easingIndex == i + IM_ARRAYSIZE(easeInOptions) + IM_ARRAYSIZE(easeOutOptions));
				if (ImGui::Selectable(easeInOutOptions[i], isSelected)) {

					easingType = static_cast<EasingType>(i + IM_ARRAYSIZE(easeInOptions) + IM_ARRAYSIZE(easeOutOptions));
					easingIndex = i + IM_ARRAYSIZE(easeInOptions) + IM_ARRAYSIZE(easeOutOptions);
				}
				if (isSelected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::EndCombo();
	}
}

template<typename U>
inline void swap(SimpleAnimation<U>& a, SimpleAnimation<U>& b) noexcept {

	std::swap(a.loop_, b.loop_);
	std::swap(a.time_, b.time_);
	std::swap(a.move_, b.move_);
}