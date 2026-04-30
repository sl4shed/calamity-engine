#pragma once
#include <spdlog/spdlog.h>
#include <memory>
#include <string>

/**
 * # Logger
 * 
 * The logger class is a static wrapper around [spdlog](https://github.com/gabime/spdlog).
 * 
 * Example usage:
 * ```cpp
 * Logger::info("this is an info log");
 * Logger::warn("this is a warning");
 * Logger::error("this is an error!");
 * 
 * Logger::info("Here is a float: {}", 20.0f);
 * Logger::warning("Here are two floats: {}... and {}!", 20.0f, 10.0f);
 * ```
 */
class Logger
{
public:
    static void init(const std::string &name = "Calamity");
    static void exit();

    template <typename... Args>
    static void info(fmt::format_string<Args...> fmt, Args &&...args)
    {
        s_Logger->info(fmt, std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void warn(fmt::format_string<Args...> fmt, Args &&...args)
    {
        s_Logger->warn(fmt, std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void error(fmt::format_string<Args...> fmt, Args &&...args)
    {
        s_Logger->error(fmt, std::forward<Args>(args)...);
    }
    template <typename... Args>
    static void debug(fmt::format_string<Args...> fmt, Args &&...args)
    {
        s_Logger->debug(fmt, std::forward<Args>(args)...);
    }

private:
    static std::shared_ptr<spdlog::logger> s_Logger;
};