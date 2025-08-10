#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Utility/GameTimer.h>
#include <Lib/Adapter/Easing.h>
#include <Engine/Utility/JsonAdapter.h>
#include <Lib/MathUtils/Algorithm.h>
#include <Lib/MathUtils/MathUtils.h>

// c++
#include <format>
#include <optional>
// imgui
#include <imgui.h>

//============================================================================
//	SimpleAnimation enum class
//============================================================================

// 補間の仕方
enum class SimpleAnimationType {

	None,  // start -> end
	Return // end -> start
};

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

		void ImGui(const std::string& label);
	};

	struct Time {

		bool useScaledDeltaTime;         // スケールされたdeltaTimeを使うかどうか
		float deltaTime;                 // deltaTime
		float elapsed;                   // 経過時間
		float end;                       // 終了時間
		float movedValueInterval;        // 動かす値の間隔
		float currentMovedValueInterval; // 現在の動かす値の間隔
		float currentT;                  // 現在のt値
	};

	struct Move {

		T start;               // 開始値
		T end;                 // 終了値
		T moveValue;           // 動かす値、currentTに沿わずendまで動く
		EasingType easingType; // イージングの種類
	};
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SimpleAnimation() = default;
	~SimpleAnimation() = default;

	void ImGui(const std::string& label);

	// 0.0fから1.0fの間で補間された値を取得
	void LerpValue(T& value);
	// 0.0fから1.0fの間でkeyframeの補間された値を取得
	void LerpKeyframeValue(T& value);
	// moveValue分endまで動く値を取得
	void MoveValue(T& value);

	// 動き出し開始
	void Start();
	// リセット
	void Reset();

	// json
	void ToJson(Json& data);
	void FromJson(const Json& data);

	//--------- accessor -----------------------------------------------------

	// animationを行うかどうか
	bool IsStart() const { return loop_.isStart; };

	// 終了したかどうか
	bool IsFinished() const { return loop_.isEnd; }
	//--------- firend -------------------------------------------------------

	template <typename U>
	friend void swap(SimpleAnimation<U>& a, SimpleAnimation<U>& b) noexcept;

	//--------- variables ----------------------------------------------------

	Loop loop_; // ループの情報
	Time time_; // タイマーの情報
	Move move_; // 動かす値の情報

	SimpleAnimationType type_;

	std::vector<T> keyframes_; // 外部から設定する
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	const float itemSize_ = 224.0f; // imguiのサイズ

	//--------- functions ----------------------------------------------------

	void UpdateElapsedTime();

	void UpdateLerpValue(T& value);
	void UpdateLerpKeyframeValue(T& value);
	void UpdateMoveValue(T& value);

	void UpdateLoop(T& value);
};

//============================================================================
//	SimpleAnimation templateMethods
//============================================================================

template<typename T>
inline void SimpleAnimation<T>::ImGui(const std::string& label) {

	ImGui::PushItemWidth(itemSize_);

	if (ImGui::BeginTabBar(("SimpleAnimation##" + label).c_str())) {

		// 共通
		if (ImGui::BeginTabItem(("Loop##" + label).c_str())) {

			loop_.ImGui(label);
			ImGui::EndTabItem();
		}
		// 補間で動く値の操作
		if (ImGui::BeginTabItem(("LerpValue##" + label).c_str())) {

			ImGui::SeparatorText("Time");

			// 経過時間
			ImGui::Text(std::format("elapsed: {}", time_.elapsed).c_str());
			ImGui::Text(std::format("currentT: {}", time_.currentT).c_str());

			ImGui::Checkbox(("useScaledDeltaTime##Time" + label).c_str(), &time_.useScaledDeltaTime);
			ImGui::DragFloat(("end##Time" + label).c_str(), &time_.end, 0.01f);

			ImGui::SeparatorText("Move");

			if constexpr (std::is_same_v<T, float>) {

				ImGui::DragFloat(("start##Move" + label).c_str(), &move_.start, 0.01f);
				ImGui::DragFloat(("end##Move" + label).c_str(), &move_.end, 0.01f);
			} else if constexpr (std::is_same_v<T, int>) {

				ImGui::DragInt(("start##Move" + label).c_str(), &move_.start, 1);
				ImGui::DragInt(("end##Move" + label).c_str(), &move_.end, 1);
			} else if constexpr (std::is_same_v<T, Vector2>) {

				ImGui::DragFloat2(("start##Move" + label).c_str(), &move_.start.x, 0.01f);
				ImGui::DragFloat2(("end##Move" + label).c_str(), &move_.end.x, 0.01f);
			} else if constexpr (std::is_same_v<T, Vector3>) {

				ImGui::DragFloat3(("start##Move" + label).c_str(), &move_.start.x, 0.01f);
				ImGui::DragFloat3(("end##Move" + label).c_str(), &move_.end.x, 0.01f);
			} else if constexpr (std::is_same_v<T, Color>) {

				ImGui::ColorEdit4(("start##Move" + label).c_str(), &move_.start.a);
				ImGui::ColorEdit4(("end##Move" + label).c_str(), &move_.end.a);
			}

			Easing::SelectEasingType(move_.easingType, label);
			ImGui::EndTabItem();
		}
		// 一定値でendまで動く値の操作
		if (ImGui::BeginTabItem(("MoveValue##" + label).c_str())) {

			ImGui::SeparatorText("Time");

			ImGui::Text(std::format("currentMovedValueInterval: {}", time_.currentMovedValueInterval).c_str());

			ImGui::Checkbox(("useScaledDeltaTime##Time" + label).c_str(), &time_.useScaledDeltaTime);
			ImGui::DragFloat(("movedValueInterval##Time" + label).c_str(), &time_.movedValueInterval, 0.001f);

			ImGui::SeparatorText("Move");

			if constexpr (std::is_same_v<T, float>) {

				ImGui::DragFloat(("start##Move" + label).c_str(), &move_.start, 0.01f);
				ImGui::DragFloat(("end##Move" + label).c_str(), &move_.end, 0.01f);
			} else if constexpr (std::is_same_v<T, int>) {

				ImGui::DragInt(("end##Move" + label).c_str(), &move_.end, 1);
				ImGui::DragInt(("moveValue##Move" + label).c_str(), &move_.moveValue, 1);
			} else if constexpr (std::is_same_v<T, Vector2>) {

				ImGui::DragFloat2(("end##Move" + label).c_str(), &move_.end.x, 0.01f);
				ImGui::DragFloat2(("moveValue##Move" + label).c_str(), &move_.moveValue.x, 0.01f);
			} else if constexpr (std::is_same_v<T, Vector3>) {

				ImGui::DragFloat3(("end##Move" + label).c_str(), &move_.end.x, 0.01f);
				ImGui::DragFloat3(("moveValue##Move" + label).c_str(), &move_.moveValue.x, 0.01f);
			} else if constexpr (std::is_same_v<T, Color>) {

				ImGui::ColorEdit4(("end##Move" + label).c_str(), &move_.end.a);
				ImGui::ColorEdit4(("moveValue##Move" + label).c_str(), &move_.moveValue.a);
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

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
inline void SimpleAnimation<T>::LerpKeyframeValue(T& value) {

	// keyframeが何もなければエラー
	ASSERT(!keyframes_.empty(), "keyframes is empty");

	// ループが開始していないときは何も処理をしない
	if (!loop_.isStart) {
		return;
	}

	// 経過時間を加算、t値を処理する
	UpdateElapsedTime();

	// 値の補間
	UpdateLerpKeyframeValue(value);

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

	T from = move_.start;
	T to = move_.end;

	// 逆補間なら逆にして補間させる
	if (type_ == SimpleAnimationType::Return) {

		std::swap(from, to);
	}

	// 補間する
	value = Algorithm::Lerp<T>(from, to, EasedValue(move_.easingType, time_.currentT));

	// 1ループ終了
	if (time_.currentT == 1.0f) {

		// ループの終了フラグを立てる
		loop_.isEnd = true;
	}
}

template<typename T>
inline void SimpleAnimation<T>::UpdateLerpKeyframeValue(T& value) {

	// ループが終了しているときは何も処理をしない
	if (loop_.isEnd) {
		return;
	}

	// とりあえずVector3のみ実装
	if constexpr (std::is_same_v<T, Vector3>) {

		value = Vector3::CatmullRomValue(keyframes_, EasedValue(move_.easingType, time_.currentT));
	}

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
		value = move_.end;
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
inline void SimpleAnimation<T>::Loop::ImGui(const std::string& label) {

	ImGui::SeparatorText("Loop");

	// 終了フラグ
	ImGui::Text(std::format("isEnd: {}", isEnd).c_str());
	ImGui::Text(std::format("loopCount: {}", currentCount).c_str());
	ImGui::Checkbox(("isLoop##Loop" + label).c_str(), &isLoop);
	ImGui::DragInt(("maxCount##Loop" + label).c_str(), (int*)&maxCount, 1.0f, 0, 100);
	ImGui::DragFloat(("interval##Loop" + label).c_str(), &interval, 0.01f, 0.0f, 10.0f);
}

template<typename U>
inline void swap(SimpleAnimation<U>& a, SimpleAnimation<U>& b) noexcept {

	std::swap(a.loop_, b.loop_);
	std::swap(a.time_, b.time_);
	std::swap(a.move_, b.move_);
}

template<typename T>
inline void SimpleAnimation<T>::ToJson(Json& data) {

	// loopの値を保存
	data["loop_.isLoop"] = loop_.isLoop;
	data["loop_.maxCount"] = loop_.maxCount;
	data["loop_.interval"] = loop_.interval;

	// timeの値を保存
	data["time_.useScaledDeltaTime"] = time_.useScaledDeltaTime;
	data["time_.end"] = time_.end;
	data["time_.movedValueInterval"] = time_.movedValueInterval;

	// moveの値を保存
	if constexpr (std::is_same_v<T, float> || std::is_same_v<T, int>) {

		data["move_.start"] = move_.start;
		data["move_.end"] = move_.end;
		data["move_.moveValue"] = move_.moveValue;
	} else {

		data["move_.start"] = JsonAdapter::FromObject<T>(move_.start);
		data["move_.end"] = JsonAdapter::FromObject<T>(move_.end);
		data["move_.moveValue"] = JsonAdapter::FromObject<T>(move_.moveValue);
	}
	data["move_.easingType"] = move_.easingType;
}

template<typename T>
inline void SimpleAnimation<T>::FromJson(const Json& data) {

	// loopの値を適応
	loop_.isLoop = JsonAdapter::GetValue<bool>(data, "loop_.isLoop");
	loop_.maxCount = JsonAdapter::GetValue<uint32_t>(data, "loop_.maxCount");
	loop_.interval = JsonAdapter::GetValue<float>(data, "loop_.interval");

	// timeの値を適応
	time_.useScaledDeltaTime = JsonAdapter::GetValue<float>(data, "time_.useScaledDeltaTime");
	time_.end = JsonAdapter::GetValue<float>(data, "time_.end");
	time_.movedValueInterval = JsonAdapter::GetValue<float>(data, "time_.movedValueInterval");

	// moveの値を適応
	if constexpr (std::is_same_v<T, float> || std::is_same_v<T, int>) {

		move_.start = JsonAdapter::GetValue<float>(data, "move_.start");
		move_.end = JsonAdapter::GetValue<float>(data, "move_.end");
		move_.moveValue = JsonAdapter::GetValue<float>(data, "move_.moveValue");
	} else {

		move_.start = JsonAdapter::ToObject<T>(data["move_.start"]);
		move_.end = JsonAdapter::ToObject<T>(data["move_.end"]);
		move_.moveValue = JsonAdapter::ToObject<T>(data["move_.moveValue"]);
	}
	move_.easingType = JsonAdapter::GetValue<EasingType>(data, "move_.easingType");
}