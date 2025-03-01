#pragma once

//============================================================================*/
//	include
//============================================================================*/

// c++
#include <random>
#include <type_traits>

//============================================================================*/
//	Random class
//============================================================================*/
class Random {
public:
	//========================================================================*/
	//	public Methods
	//========================================================================*/

	template <typename T>
	static T Generate(T min, T max);

};

template<typename T>
inline T Random::Generate(T min, T max) {

	static_assert(std::is_arithmetic<T>::value, "T must be an arithmetic type");

	static std::random_device rd;
	static std::mt19937 gen(rd());

	if constexpr (std::is_integral<T>::value) {

		std::uniform_int_distribution<T> dist(min, max);
		return dist(gen);
	} else if constexpr (std::is_floating_point<T>::value) {

		std::uniform_real_distribution<T> dist(min, max);
		return dist(gen);
	}
}