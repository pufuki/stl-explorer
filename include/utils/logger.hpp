#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace stl_explorer {
namespace utils {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

class Logger {
public:
    static Logger& instance();

    void setLogFile(const std::string& filename);
    void setLogLevel(LogLevel level);
    void log(LogLevel level, const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);

private:
    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string levelToString(LogLevel level) const;
    std::string getCurrentTimestamp() const;

    std::ofstream log_file_;
    std::mutex mutex_;
    LogLevel min_level_{LogLevel::Info};
    bool file_enabled_{false};
};

#define LOG_DEBUG(msg) stl_explorer::utils::Logger::instance().debug(msg)
#define LOG_INFO(msg) stl_explorer::utils::Logger::instance().info(msg)
#define LOG_WARNING(msg) stl_explorer::utils::Logger::instance().warning(msg)
#define LOG_ERROR(msg) stl_explorer::utils::Logger::instance().error(msg)

} // namespace utils
} // namespace stl_explorer
