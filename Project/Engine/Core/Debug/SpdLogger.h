#pragma once

//============================================================================
//	include
//============================================================================

// spdlog
#ifndef SPDLOG_FUNCTION
#   define SPDLOG_FUNCTION __FUNCSIG__
#endif
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#if defined(_MSC_VER)
#include <spdlog/sinks/msvc_sink.h>
#endif
#include <spdlog/sinks/basic_file_sink.h>
// c++
#include <filesystem>
#include <vector>

//============================================================================
//	SpdLogger class
//============================================================================
class SpdLogger {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	SpdLogger() = default;
	~SpdLogger() = default;

	// ログのレベル
	enum class LogLevel {
		INFO,
		ASSERT_ERROR
	};

	static void Init(const std::string& fileName = "engine.log", bool truncate = true);

	static void Log(const std::string& message, LogLevel level = LogLevel::INFO);

	template <typename... Args>
	static void LogFormat(LogLevel level, fmt::format_string<Args...> fmt, Args&&... args);

	//--------- accessor -----------------------------------------------------

	static std::shared_ptr<spdlog::logger>& Get() { return logger_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	static inline std::shared_ptr<spdlog::logger> logger_;
};

//============================================================================
//	SpdLogger templateMethods
//============================================================================

template<typename ...Args>
inline void SpdLogger::LogFormat(LogLevel level, fmt::format_string<Args...> fmt, Args && ...args) {

	switch (level) {
	case LogLevel::INFO:

		logger_->log(spdlog::level::info, fmt, std::forward<Args>(args)...);
		break;
	case LogLevel::ASSERT_ERROR:

		logger_->log(spdlog::level::critical, fmt, std::forward<Args>(args)...);
		break;
	}
}

//============================================================================
//	SpdLogger defines
//============================================================================

#define LOG_INFO(...)  SPDLOG_LOGGER_CALL(SpdLogger::Get(), spdlog::level::info,     __VA_ARGS__)
#define LOG_WARN(...)  SPDLOG_LOGGER_CALL(SpdLogger::Get(), spdlog::level::warn,     __VA_ARGS__)
#define LOG_ERROR(...) SPDLOG_LOGGER_CALL(SpdLogger::Get(), spdlog::level::err,      __VA_ARGS__)
#define LOG_CRIT(...)  SPDLOG_LOGGER_CALL(SpdLogger::Get(), spdlog::level::critical, __VA_ARGS__)