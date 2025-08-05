#include "Algorithm.h"

//============================================================================
//	Algorithm classMethods
//============================================================================

std::string Algorithm::RemoveSubstring(const std::string& input, const std::string& toRemove) {

	std::string result = input;
	size_t pos;

	// toRemoveが見つかる限り削除する
	while ((pos = result.find(toRemove)) != std::string::npos) {
		result.erase(pos, toRemove.length());
	}

	return result;
}

std::string Algorithm::RemoveAfterUnderscore(const std::string& input) {

	size_t pos = input.find('_');
	if (pos != std::string::npos) {
		return input.substr(0, pos);
	}
	return input;
}

std::string Algorithm::GetIndexLabel(const std::string& label, uint32_t index) {

	return label + std::to_string(index);
}

std::wstring Algorithm::ConvertString(const std::string& str) {

	if (str.empty()) {
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0) {
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

int Algorithm::LerpInt(int a, int b, float t) {

	return static_cast<int>(std::round(std::lerp(
		static_cast<float>(a), static_cast<float>(b), t)));
}

float Algorithm::GetReparameterizedT(float t, const std::vector<float>& arcLengths) {

	// arcLengthsになにも値が入ってない
	assert(!arcLengths.empty() && "arcLengths is empty");

	// tに対するアーク長を求める
	for (uint32_t i = 1; i < arcLengths.size(); ++i) {
		if (t < arcLengths[i]) {

			// 間の2つのアーク長の間で補間する
			float segmentT = (t - arcLengths[i - 1]) / (arcLengths[i] - arcLengths[i - 1]);
			float result = (static_cast<float>(i - 1) + segmentT) / static_cast<float>(arcLengths.size() - 1);
			return result;
		}
	}

	return t;
}

float Algorithm::CatmullRomInterpolation(const float& p0, const float& p1,
	const float& p2, const float& p3, float t) {

	const float s = 0.5f;

	float t2 = t * t;
	float t3 = t2 * t;

	// 各項の計算
	float e3 = 3.0f * p1 - 3.0f * p2 + p3 - p0;
	float e2 = 2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3;
	float e1 = -1.0f * p0 + p2;
	float e0 = 2.0f * p1;

	// 補間結果の計算
	float result = t3 * e3 + t2 * e2 + t * e1 + e0;

	return (s * result);
}

float Algorithm::CatmullRomValue(const std::vector<float>& points, float t) {

	assert(points.size() >= 4 && "制御点が足りません");

	// 区間数は制御点の数 - 1
	size_t division = points.size() - 1;
	// 1区間の長さ (全体を 1.0 とした割合)
	float areaWidth = 1.0f / division;

	// 区間内の始点を 0.0f、終点を 1.0f とした時の現在位置
	float t_2 = std::fmod(t, areaWidth) * division;
	// 下限(0.0f)と上限(1.0f)の範囲に収める
	t_2 = std::clamp(t_2, 0.0f, 1.0f);

	// 区間番号
	size_t index = static_cast<size_t>(t / areaWidth);
	// 区間番号が上限を超えないための計算
	index = (std::min)(index, division - 1);

	// 4点分のインデックス
	size_t index0 = index - 1;
	size_t index1 = index;
	size_t index2 = index + 1;
	size_t index3 = index + 2;

	// 最初の区間のp0はp1を重複使用する
	if (index == 0) {

		index0 = index1;
	}
	// 最後の区間のp3はp2を重複使用する
	if (points.size() <= index3) {

		index3 = index2;
	}

	// 4点の座標
	const float& p0 = points[index0];
	const float& p1 = points[index1];
	const float& p2 = points[index2];
	const float& p3 = points[index3];

	return CatmullRomInterpolation(p0, p1, p2, p3, t_2);
}