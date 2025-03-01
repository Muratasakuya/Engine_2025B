#include "Logger.h"

//============================================================================
//	Logger classMethods
//============================================================================

void Logger::Log(const std::string& message, LogLevel level) {

	std::string prefix = GetLogLevelPrefix(level);
	std::cout << prefix << message << std::endl;

	// コンソール出力
	OutputDebugStringA((prefix + message + "\n").c_str());

	// ログファイルにも出力
	if (logToFile) {
		WriteToFile(prefix + message);
	}
}

void Logger::LogFormat(LogLevel level, const char* format, ...) {

	char buffer[512];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	Log(buffer, level);
}

void Logger::EnableFileLogging(const std::string& filename) {

	logFileName = filename;
	logToFile = true;

	std::string logDirectory = "./Log/";
	std::string fullPath = logDirectory + logFileName;
	std::ofstream file(fullPath, std::ios::trunc);
}

void Logger::DisableFileLogging() {

	logToFile = false;
}

std::string Logger::GetLogLevelPrefix(LogLevel level) {

	switch (level) {
	case LogLevel::INFO:         return "INFO: ";
	case LogLevel::ASSERT_ERROR: return "[ERROR] ";
	default:                     return "[UNKNOWN] ";
	}
}

void Logger::WriteToFile(const std::string& message) {

	std::string logDirectory = "./Log/";
	std::string fullPath = logDirectory + logFileName;

	// ディレクトリが存在しなければ作成
	if (!std::filesystem::exists(logDirectory)) {
		std::filesystem::create_directories(logDirectory);
	}

	std::ofstream file(fullPath, std::ios::app);
	if (file.is_open()) {
		file << message << std::endl;
	}
}