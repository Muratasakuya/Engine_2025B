#pragma once

//============================================================================
//	include
//============================================================================
#include <Lib/Adapter/RandomGenerator.h>
#include <Lib/MathUtils/Vector3.h>
#include <Lib/MathUtils/Vector4.h>

// imgui
#include <imgui.h>
// c++
#include <string>
#include <variant>

//============================================================================
//	ParticleValue structures
//============================================================================

// billboardの種類
enum class ParticleBillboardType {

	None,
	All,
	YAxis
};
namespace ParticleBillboard {

	void SelectBillboardType(ParticleBillboardType& billboardType, const std::string& label = "label");
}

// 値の種類
enum class ParticleValueType {

	Constant,
	Random
};
// 定数
template<typename T>
struct ParticleConstantValue {

	T value;
};
// ランダム値
template<typename T>
struct ParticleRandomRangeValue {

	T min;
	T max;

	// ランダム値を生成して取得
	T GetRandomValue() const {
		return RandomGenerator::Generate(min, max);
	}
};

//============================================================================
//	ParticleValue class
//============================================================================
template<typename T>
class ParticleValue {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	ParticleValue() = default;
	~ParticleValue() = default;

	//--------- functions ----------------------------------------------------

	// 定数とランダム値に同じ値を設定
	static ParticleValue<T> SetValue(T value);

	// 分けて処理
	void EditDragValue(const std::string& label);
	void EditColor(const std::string& label);

	//--------- variables ----------------------------------------------------

	ParticleValueType valueType;

	// 値
	ParticleConstantValue<T> constant;  // 定数
	ParticleRandomRangeValue<T> random; // ランダム

private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- functions ----------------------------------------------------

	void SelectType(const std::string& label);
};

//============================================================================
//	ParticleValue templateMethods
//============================================================================

template<typename T>
inline ParticleValue<T> ParticleValue<T>::SetValue(T value) {

	ParticleValue<T> particleValue{};
	particleValue.constant.value = value;
	particleValue.random.min = value;
	particleValue.random.max = value;
	return particleValue;
}

template<typename T>
inline void ParticleValue<T>::SelectType(const std::string& label) {

	// 定数値かランダムを設定する
	const char* kItems[] = { "Constant", "Random" };
	int comboIndex = static_cast<int>(valueType);
	ImGui::SetNextItemWidth(132.0f);
	ImGui::SetCursorPosX(560.0f);
	if (ImGui::Combo(("##Type" + label).c_str(), &comboIndex, kItems, IM_ARRAYSIZE(kItems))) {

		valueType = static_cast<ParticleValueType>(comboIndex);
	}
}

template<typename T>
inline void ParticleValue<T>::EditDragValue(const std::string& label) {

	ImGui::PushID(label.c_str());

	// 分岐処理
	if constexpr (std::is_same_v<T, uint32_t>) {
		if (valueType == ParticleValueType::Constant) {

			ImGui::DragInt(label.c_str(), reinterpret_cast<int*>(&constant.value), 1, 0, 128);
		} else if (valueType == ParticleValueType::Random) {

			ImGui::DragInt((label + ":Min").c_str(), reinterpret_cast<int*>(&random.min), 1, 0, 128);
			// 定数値かランダムを設定する
			ImGui::SameLine();
			SelectType(label);

			ImGui::DragInt((label + ":Max").c_str(), reinterpret_cast<int*>(&random.max), 1, 0, 128);
		}
	} else if constexpr (std::is_same_v<T, float>) {
		if (valueType == ParticleValueType::Constant) {

			ImGui::DragFloat(label.c_str(), &constant.value, 0.001f);
		} else if (valueType == ParticleValueType::Random) {

			ImGui::DragFloat((label + ":Min").c_str(), &random.min, 0.001f);
			// 定数値かランダムを設定する
			ImGui::SameLine();
			SelectType(label);

			ImGui::DragFloat((label + ":Max").c_str(), &random.max, 0.001f);
		}
	} else if constexpr (std::is_same_v<T, Vector3>) {
		if (valueType == ParticleValueType::Constant) {

			ImGui::DragFloat3(label.c_str(), &constant.value.x, 0.001f);
		} else if (valueType == ParticleValueType::Random) {

			ImGui::DragFloat3((label + ":Min").c_str(), &random.min.x, 0.001f);
			// 定数値かランダムを設定する
			ImGui::SameLine();
			SelectType(label);

			ImGui::DragFloat3((label + ":Max").c_str(), &random.max.x, 0.001f);
		}
	}

	if (valueType == ParticleValueType::Constant) {

		// 定数値かランダムを設定する
		ImGui::SameLine();
		SelectType(label);
	}

	ImGui::PopID();
}

template<typename T>
inline void ParticleValue<T>::EditColor(const std::string& label) {

	ImGui::PushID(label.c_str());

	if constexpr (std::is_same_v<T, Vector3>) {
		if (valueType == ParticleValueType::Constant) {

			ImGui::ColorEdit3(label.c_str(), &constant.value.x);
		} else if (valueType == ParticleValueType::Random) {

			ImGui::ColorEdit3((label + ":Min").c_str(), &random.min.x);
			// 定数値かランダムを設定する
			ImGui::SameLine();
			SelectType(label);

			ImGui::ColorEdit3((label + ":Max").c_str(), &random.max.x);
		}
	} else if constexpr (std::is_same_v<T, Color>) {
		if (valueType == ParticleValueType::Constant) {

			ImGui::ColorEdit4(label.c_str(), &constant.value.r);
		} else if (valueType == ParticleValueType::Random) {

			ImGui::ColorEdit4((label + ":Min").c_str(), &random.min.r);
			// 定数値かランダムを設定する
			ImGui::SameLine();
			SelectType(label);

			ImGui::ColorEdit4((label + ":Max").c_str(), &random.max.r);
		}
	}

	if (valueType == ParticleValueType::Constant) {

		// 定数値かランダムを設定する
		ImGui::SameLine();
		SelectType(label);
	}

	ImGui::PopID();
}