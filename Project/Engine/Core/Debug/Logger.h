#pragma once

//============================================================================
//	include
//============================================================================

// windows
#include <windows.h>
// c++
#include <iostream>
#include <fstream>
#include <string>
#include <cstdarg>
#include <filesystem> 

//============================================================================
//	Logger class
//============================================================================
class Logger {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	Logger() = default;
	~Logger() = default;

	// ログのレベル
	enum class LogLevel {
		INFO,
		ASSERT_ERROR
	};

	static void Log(const std::string& message, LogLevel level = LogLevel::INFO);

	static void LogFormat(LogLevel level, const char* format, ...);

	static void EnableFileLogging(const std::string& filename = "log.md");

	static void DisableFileLogging();
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	static inline bool logToFile = false;
	static inline std::string logFileName = "log.md";

	//--------- functions ----------------------------------------------------

	static std::string GetLogLevelPrefix(LogLevel level);

	static void WriteToFile(const std::string& message);
};