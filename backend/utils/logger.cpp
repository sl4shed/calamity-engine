#include "logger.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> Logger::s_Logger;

void Logger::init(const std::string &name)
{
    s_Logger = spdlog::stdout_color_mt(name);
    s_Logger->set_level(spdlog::level::debug);
    s_Logger->set_pattern("%^[%T] %n: %v%$");
}

void Logger::shutdown()
{
    s_Logger.reset();
}