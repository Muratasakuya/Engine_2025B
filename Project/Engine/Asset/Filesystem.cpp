#include "Filesystem.h"

//============================================================================
//	Filesystem namespaceMethods
//============================================================================

bool Filesystem::Found(const fs::path& basePath, const std::string& fileName, fs::path& fullPath) {

	bool found = false;

	// ディレクトリを再帰的に探索
	for (const auto& entry : fs::recursive_directory_iterator(basePath)) {
		if (entry.is_regular_file() && entry.path().filename() == fileName) {

			fullPath = entry.path();
			found = true;
			break;
		}
	}
	return found;
}