// Copyright © 2024 Jacob Curlin

#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace cgx::util
{
class LoggingHandler
{
public:
    LoggingHandler();
    ~LoggingHandler();

    static void initialize();
    static void shutdown();

    static std::shared_ptr<spdlog::logger>& get_core_logger();
    static std::shared_ptr<spdlog::logger>& get_client_logger();

private:
    static std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> s_console_sink;
    static std::shared_ptr<spdlog::logger>                      s_core_logger;
    static std::shared_ptr<spdlog::logger>                      s_client_logger;
};
}

#define CGX_DEFAULT_LOGGER_NAME "core"

#define CGX_TRACE(...)      if (spdlog::get(CGX_DEFAULT_LOGGER_NAME) != nullptr) {spdlog::get(CGX_DEFAULT_LOGGER_NAME)->trace(__VA_ARGS__);}
#define CGX_DEBUG(...)      if (spdlog::get(CGX_DEFAULT_LOGGER_NAME) != nullptr) {spdlog::get(CGX_DEFAULT_LOGGER_NAME)->debug(__VA_ARGS__);}
#define CGX_INFO(...)       if (spdlog::get(CGX_DEFAULT_LOGGER_NAME) != nullptr) {spdlog::get(CGX_DEFAULT_LOGGER_NAME)->info(__VA_ARGS__);}
#define CGX_WARN(...)       if (spdlog::get(CGX_DEFAULT_LOGGER_NAME) != nullptr) {spdlog::get(CGX_DEFAULT_LOGGER_NAME)->warn(__VA_ARGS__);}
#define CGX_ERROR(...)      if (spdlog::get(CGX_DEFAULT_LOGGER_NAME) != nullptr) {spdlog::get(CGX_DEFAULT_LOGGER_NAME)->error(__VA_ARGS__);}
#define CGX_CRITICAL(...)   if (spdlog::get(CGX_DEFAULT_LOGGER_NAME) != nullptr) {spdlog::get(CGX_DEFAULT_LOGGER_NAME)->critical(__VA_ARGS__);}

#if __cplusplus >= 202002L
#include <source_location>
#endif

// platform/compiler detection for abort 
#if defined(_MSC_VER) // MSVC
    #define CGX_DEBUG_BREAK() __debugbreak()
#elif defined(__GNUC__) || defined(__clang__) // GCC or Clang
#if defined(__APPLE__) || defined(__linux__)
#include <signal.h>
#define CGX_DEBUG_BREAK() raise(SIGTRAP)
#else
        #define CGX_DEBUG_BREAK() __builtin_trap()
#endif
#else
    #define CGX_DEBUG_BREAK() std::abort() // Fallback for other compilers
#endif

// CGX_FATAL macro conditional on support of std::source_location
#if __cplusplus >= 202002L && defined(__cpp_lib_source_location)
#define CGX_FATAL(format, ...) do { \
        auto location = std::source_location::current(); \
        printf("%s:%u " format "\n", location.file_name(), location.line(), ##__VA_ARGS__); \
        CGX_DEBUG_BREAK(); \
        std::abort(); \
    } while (0)
#else
    #define CGX_FATAL(format, ...) do { \
        printf("%s:%d " format "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
        CGX_DEBUG_BREAK(); \
        std::abort(); \
    } while (0)
#endif

#define CGX_ASSERT(x, msg) do { if (!(x)) { CGX_FATAL("assert failed: %s\nmessage: %s", #x, msg); } } while (0)
#define CGX_VERIFY(x) do { if (!(x)) { CGX_FATAL("assert failed: %s", #x); } } while (0)

