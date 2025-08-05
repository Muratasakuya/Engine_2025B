#pragma once

//============================================================================
//	include
//============================================================================

// c++
#include <string>
#include <filesystem>

//============================================================================
//	FileSystem namespace
//============================================================================
namespace Filesystem {

	//--------- functions ----------------------------------------------------

	bool Found(const std::filesystem::path& basePath, const std::string& fileName,
		std::filesystem::path& fullPath);

	// 指定の拡張子ファイル探索
	bool FindByStem(const std::filesystem::path& basePath,
		const std::string& stem, const std::vector<std::string>& extensions,
		std::filesystem::path& outPath);
};