#pragma once

#include <spdlog/spdlog.h>
#include <memory>
#include <string>

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