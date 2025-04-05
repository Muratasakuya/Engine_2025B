#pragma once

//============================================================================
//	include
//============================================================================

// c++
#include <string>
#include <filesystem>
// namespace
namespace fs = std::filesystem;

//============================================================================
//	FileSystem namespace
//============================================================================
namespace Filesystem {

	//--------- functions ----------------------------------------------------

	bool Found(const fs::path& basePath, const std::string& fileName, fs::path& fullPath);
};