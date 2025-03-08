#pragma once

//============================================================================
//	include
//============================================================================
#include <Engine/Core/Debug/Assert.h>

//============================================================================
//	Algorithm namespace
//============================================================================
namespace Algorithm {

	//========================================================================
	//	Find
	//========================================================================

	template <typename, typename = std::void_t<>>
	struct has_find_method : std::false_type {};
	template <typename T>
	struct has_find_method<T, std::void_t<decltype(std::declval<T>().find(std::declval<typename T::key_type>()))>>
		: std::true_type {
	};
	template <typename T>
	constexpr bool has_find_method_v = has_find_method<T>::value;

	template <typename TA, typename TB>
	typename std::enable_if_t<has_find_method_v<TA>, bool>
		Find(const TA& object, const TB& key, bool assertionEnable = false) {

		auto it = object.find(key);
		bool found = it != object.end();

		if (!found && assertionEnable) {
			ASSERT(false, "not found this object");
		}

		return found;
	}
	template <typename TA, typename TB>
	typename std::enable_if_t<!has_find_method_v<TA>, bool>
		Find(const TA& object, const TB& key, bool assertionEnable = false) {

		auto it = std::find(object.begin(), object.end(), key);
		bool found = it != object.end();

		if (!found && assertionEnable) {
			ASSERT(false, "not found this object");
		}

		return found;
	}

	//========================================================================
	//	Find
	//========================================================================


}