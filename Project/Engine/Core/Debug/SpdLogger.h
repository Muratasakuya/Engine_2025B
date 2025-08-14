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
#include <chrono>
#include <string>

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
	static void InitAsset(const std::string& fileName = "assetCheck.log", bool truncate = true);

	static void Log(const std::string& message, LogLevel level = LogLevel::INFO);

	template <typename... Args>
	static void LogFormat(LogLevel level, fmt::format_string<Args...> fmt, Args&&... args);

	//--------- accessor -----------------------------------------------------

	static std::shared_ptr<spdlog::logger>& Get() { return logger_; }
	static std::shared_ptr<spdlog::logger>& GetAsset() { return assetLogger_; }
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	static inline std::shared_ptr<spdlog::logger> logger_;
	static inline std::shared_ptr<spdlog::logger> assetLogger_;
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

#define LOG_ASSET_INFO(...)  SPDLOG_LOGGER_CALL(SpdLogger::GetAsset(), spdlog::level::info, __VA_ARGS__)

//============================================================================
//	ScopedMsLog class
//============================================================================
class ScopedMsLog final {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	explicit ScopedMsLog(std::string label) :
		label_(std::move(label)), start_(std::chrono::steady_clock::now()) {}

	~ScopedMsLog() {

		using namespace std::chrono;
		const auto us = duration_cast<microseconds>(steady_clock::now() - start_).count();
		const double ms = static_cast<double>(us) / 1000.0;
		SpdLogger::Get()->log(spdlog::level::info, "[TIMER] {} : {:.3f} ms", label_, ms);
	}
private:
	//========================================================================
	//	private Methods
	//========================================================================

	//--------- variables ----------------------------------------------------

	std::string label_;
	std::chrono::steady_clock::time_point start_;
};

//============================================================================
//	ScopedMsLog defines
//============================================================================
#ifndef SPDLOG_FUNCTION
#   define SPDLOG_FUNCTION __FUNCSIG__
#endif
#define CONCAT_INNER(a,b) a##b
#define CONCAT(a,b) CONCAT_INNER(a,b)
#define LOG_SCOPE_MS_THIS() ::ScopedMsLog CONCAT(_scopems_, __LINE__)(SPDLOG_FUNCTION)
#define LOG_SCOPE_MS_LABEL(label) ::ScopedMsLog CONCAT(_scopems_, __LINE__)(label)