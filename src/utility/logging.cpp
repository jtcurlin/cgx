// Copyright © 2024 Jacob Curlin

#include "utility/logging.h"

namespace cgx::util
{
std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> LoggingHandler::s_console_sink;
std::shared_ptr<spdlog::logger>                      LoggingHandler::s_core_logger;
std::shared_ptr<spdlog::logger>                      LoggingHandler::s_client_logger;

LoggingHandler::LoggingHandler()  = default;
LoggingHandler::~LoggingHandler() = default;

void LoggingHandler::initialize()
{
    // console sink for logs
    s_console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    s_console_sink->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e] %n: %v%$");

    std::vector<spdlog::sink_ptr> sinks{s_console_sink};

    // core engine logger
    s_core_logger = std::make_shared<spdlog::logger>("core", sinks.begin(), sinks.end());
    s_core_logger->set_level(spdlog::level::trace);
    s_core_logger->flush_on(spdlog::level::trace); // flush on receipt of every trace-level log message

    spdlog::register_logger(s_core_logger);

    // client logger
    s_client_logger = std::make_shared<spdlog::logger>("client", sinks.begin(), sinks.end());
    s_client_logger->set_level(spdlog::level::trace);
    s_client_logger->flush_on(spdlog::level::trace); // flush on receipt of every trace-level log message

    spdlog::register_logger(s_client_logger);
}

void LoggingHandler::shutdown()
{
    spdlog::shutdown();
}

std::shared_ptr<spdlog::logger>& LoggingHandler::get_core_logger()
{
    return s_core_logger;
}

std::shared_ptr<spdlog::logger>& LoggingHandler::get_client_logger()
{
    return s_client_logger;
}
}
