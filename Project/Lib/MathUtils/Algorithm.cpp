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
