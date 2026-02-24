#pragma once

#include <spdlog/spdlog.h>
#include <memory>
#include <string>

/**
 * # Logger class
 * Simple wrapper around spdlog for logging messages. For now and because I'm lazy you can't switch logging levels.
 * Example usage:
 *
 * ```
 * Logger::init("Test logger");
 *
 * Logger::info("Hello, {}!", "World"); // You can use fmt formatted strings. For further insight look at spdlog and fmt docs
 * ```
 */
class Logger
{
public:
    static void init(const std::string &name = "Calamity");
    static void shutdown();

    template <typename... Args>
    static void info(const char *fmt, Args &&...args)
    {
        s_Logger->info(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void warn(const char *fmt, Args &&...args)
    {
        s_Logger->warn(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void error(const char *fmt, Args &&...args)
    {
        s_Logger->error(fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    static void debug(const char *fmt, Args &&...args)
    {
        s_Logger->debug(fmt, std::forward<Args>(args)...);
    }

private:
    static std::shared_ptr<spdlog::logger> s_Logger;
};