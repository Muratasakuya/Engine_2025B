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
//	KeyframeAnimation class
//============================================================================
template<typename T>
class KeyframeAnimation {

private:
	//========================================================================
	// private Methods
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

		bool useScaledDeltaTime; // スケールされたdeltaTimeを使うかどうか
		float deltaTime;         // deltaTime
		float elapsed;           // 経過時間
		float end;               // 終了時間
		float currentT;          // 現在のt値
	};

	struct Move {

		std::vector<T> keyframes; // キーフレーム
		EasingType easingType;    // イージングの種類
	};
public:
	//========================================================================
	//	public Methods
	//========================================================================

	KeyframeAnimation();
	~KeyframeAnimation() = default;

	void ImGui(const std::string& label);

	// 0.0fから1.0fの間で補間された値を取得
	void LerpValue(T& value);

	// 動き出し開始
	void Start();
	// リセット
	void Reset();

	// アーク長を計算
	void CalArcLengths();

	// json
	void ToJson(Json& data);
	void FromJson(const Json& data);

	//--------- accessor -----------------------------------------------------

	// animationを行うかどうか
	bool IsStart() const { return loop_.isStart; };

	// 終了したかどうか
	bool IsFinished() const { return loop_.isEnd; }

	//--------- variables ----------------------------------------------------

	Loop loop_; // ループの情報
	Time time_; // タイマーの情報
	Move move_; // 動かす値の情報
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	const float itemSize_ = 224.0f;    // imguiのサイズ
	const uint32_t arcDivision_ = 256; // アーク長の分割数
	std::vector<float> arcLengths_;    // splineの間の間隔を均一にする

	//--------- functions ----------------------------------------------------

	void UpdateElapsedTime();

	void UpdateLerpValue(T& value);

	void UpdateLoop(T& value);
};

//============================================================================
//	KeyframeAnimation templateMethods
//============================================================================

template<typename T>
inline KeyframeAnimation<T>::KeyframeAnimation() {

	// keyframes >= 4になるように最初から4個追加
	move_.keyframes.resize(4);
}

template<typename T>
inline void KeyframeAnimation<T>::ImGui(const std::string& label) {

	ImGui::PushItemWidth(itemSize_);

	if (ImGui::BeginTabBar(("KeyframeAnimation##" + label).c_str())) {

		// 共通
		if (ImGui::BeginTabItem(("Loop##" + label).c_str())) {

			loop_.ImGui(label);
			ImGui::EndTabItem();
		}
		// 時間
		if (ImGui::BeginTabItem(("Time##" + label).c_str())) {

			// 経過時間
			ImGui::Text(std::format("elapsed: {}", time_.elapsed).c_str());
			ImGui::Text(std::format("currentT: {}", time_.currentT).c_str());

			ImGui::Checkbox(("useScaledDeltaTime##Time" + label).c_str(), &time_.useScaledDeltaTime);
			ImGui::DragFloat(("end##Time" + label).c_str(), &time_.end, 0.01f);
			ImGui::EndTabItem();
		}
		// 補間で動く値の操作
		if (ImGui::BeginTabItem(("Move##" + label).c_str())) {

			Easing::SelectEasingType(move_.easingType, label);

			ImGui::SeparatorText("Keyframe");

			if (ImGui::Button("Add keyframe", ImVec2(itemSize_, 32.0f))) {

				move_.keyframes.push_back(move_.keyframes.back());
			}

			if (ImGui::Button("CalArcLengths", ImVec2(itemSize_, 32.0f))) {

				CalArcLengths();
			}

			for (uint32_t index = 0; index < move_.keyframes.size();) {
				if (ImGui::Button("Remove keyframe", ImVec2(itemSize_, 32.0f))) {

					// index番目を削除する
					move_.keyframes.erase(move_.keyframes.begin() + index);
					continue;
				}

				if constexpr (std::is_same_v<T, float>) {

					ImGui::DragFloat(("keyframe" + std::to_string(index) + "##Move" + label).c_str(), &move_.keyframes[index], 0.01f);
				} else if constexpr (std::is_same_v<T, int>) {

					ImGui::DragInt(("keyframe" + std::to_string(index) + "##Move" + label).c_str(), &move_.keyframes[index], 1);
				} else if constexpr (std::is_same_v<T, Vector2>) {

					ImGui::DragFloat2(("keyframe" + std::to_string(index) + "##Move" + label).c_str(), &move_.keyframes[index].x, 0.01f);
				} else if constexpr (std::is_same_v<T, Vector3>) {

					ImGui::DragFloat3(("keyframe" + std::to_string(index) + "##Move" + label).c_str(), &move_.keyframes[index].x, 0.01f);
				}

				++index;
			}

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::PopItemWidth();
}

template<typename T>
inline void KeyframeAnimation<T>::LerpValue(T& value) {

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
inline void KeyframeAnimation<T>::Start() {

	loop_.isStart = true;
	loop_.isEnd = false;
}

template<typename T>
inline void KeyframeAnimation<T>::Reset() {

	loop_.isStart = false;
	loop_.isEnd = false;
	loop_.currentCount = 0;
	time_.elapsed = 0.0f;
	time_.currentT = 0.0f;
}

template<typename T>
inline void KeyframeAnimation<T>::CalArcLengths() {

	arcLengths_ = Algorithm::ComputeArcLengths<T>(move_.keyframes, arcDivision_);
}

template<typename T>
inline void KeyframeAnimation<T>::UpdateElapsedTime() {

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
inline void KeyframeAnimation<T>::UpdateLerpValue(T& value) {

	// ループが終了しているときは何も処理をしない
	if (loop_.isEnd) {
		return;
	}

	// t値
	float t = time_.currentT;

	// アーク長でT値を再計算
	if (!arcLengths_.empty()) {

		t = Algorithm::GetReparameterizedT(t, arcLengths_);
	}

	// 補間処理
	if constexpr (std::is_same_v<T, float>) {

		value = Algorithm::CatmullRomValue(move_.keyframes, EasedValue(move_.easingType));
	} else {

		value = Algorithm::CatmullRomValue<T>(move_.keyframes, EasedValue(move_.easingType));
	}

	// 1ループ終了
	if (time_.currentT == 1.0f) {

		// ループの終了フラグを立てる
		loop_.isEnd = true;
	}
}

template<typename T>
inline void KeyframeAnimation<T>::UpdateLoop(T& value) {

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
inline void KeyframeAnimation<T>::Loop::ImGui(const std::string& label) {

	ImGui::SeparatorText("Loop");

	// 終了フラグ
	ImGui::Text(std::format("isEnd: {}", isEnd).c_str());
	ImGui::Text(std::format("loopCount: {}", currentCount).c_str());
	ImGui::Checkbox(("isLoop##Loop" + label).c_str(), &isLoop);
	ImGui::DragInt(("maxCount##Loop" + label).c_str(), (int*)&maxCount, 1.0f, 0, 100);
	ImGui::DragFloat(("interval##Loop" + label).c_str(), &interval, 0.01f, 0.0f, 10.0f);
}

template<typename T>
inline void KeyframeAnimation<T>::ToJson(Json& data) {

	// loopの値を保存
	data["loop_.isLoop"] = loop_.isLoop;
	data["loop_.maxCount"] = loop_.maxCount;
	data["loop_.interval"] = loop_.interval;

	// timeの値を保存
	data["time_.useScaledDeltaTime"] = time_.useScaledDeltaTime;
	data["time_.end"] = time_.end;

	// moveの値を保存
	if constexpr (std::is_same_v<T, float> || std::is_same_v<T, int>) {

		// keyframe
		if (!move_.keyframes.empty()) {
			for (uint32_t index = 0; index < move_.keyframes.size(); ++index) {

				data["Keyframes"][std::to_string(index)] =
					move_.keyframes[index];
			}
		}
	} else {

		// keyframe
		if (!move_.keyframes.empty()) {
			for (uint32_t index = 0; index < move_.keyframes.size(); ++index) {

				data["Keyframes"][std::to_string(index)] =
				JsonAdapter::FromObject<T>(move_.keyframes[index]);
			}
		}
	}
	data["move_.easingType"] = move_.easingType;
}

template<typename T>
inline void KeyframeAnimation<T>::FromJson(const Json& data) {

	// loopの値を適応
	loop_.isLoop = JsonAdapter::GetValue<bool>(data, "loop_.isLoop");
	loop_.maxCount = JsonAdapter::GetValue<uint32_t>(data, "loop_.maxCount");
	loop_.interval = JsonAdapter::GetValue<float>(data, "loop_.interval");

	// timeの値を適応
	time_.useScaledDeltaTime = JsonAdapter::GetValue<float>(data, "time_.useScaledDeltaTime");
	time_.end = JsonAdapter::GetValue<float>(data, "time_.end");

	// moveの値を適応
	if constexpr (std::is_same_v<T, float> || std::is_same_v<T, int>) {

		// keyframe
		if (data.contains("Keyframes")) {

			move_.keyframes.clear();
			const auto& keyData = data["Keyframes"];
			for (size_t i = 0; i < keyData.size(); ++i) {

				std::string key = std::to_string(i);
				if (keyData.contains(key)) {

					move_.keyframes.emplace_back(JsonAdapter::GetValue<float>(keyData, key));
				}
			}
		}
	} else {

		// keyframe
		if (data.contains("Keyframes")) {

			move_.keyframes.clear();
			const auto& keyData = data["Keyframes"];
			for (size_t i = 0; i < keyData.size(); ++i) {

				std::string key = std::to_string(i);
				if (keyData.contains(key)) {

					move_.keyframes.emplace_back(JsonAdapter::ToObject<T>(keyData[key]));
				}
			}
		}
	}
	move_.easingType = JsonAdapter::GetValue<EasingType>(data, "move_.easingType");
}