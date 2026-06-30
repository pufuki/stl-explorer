#include "utils/logger.hpp"
#include <iostream>

namespace stl_explorer {
namespace utils {

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

void Logger::setLogFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (log_file_.is_open()) {
        log_file_.close();
    }
    log_file_.open(filename, std::ios::app);
    file_enabled_ = log_file_.is_open();
}

void Logger::setLogLevel(LogLevel level) {
    min_level_ = level;
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < min_level_) return;

    std::lock_guard<std::mutex> lock(mutex_);
    std::string entry = "[" + getCurrentTimestamp() + "] "
                      + levelToString(level) + ": " + message;

    if (file_enabled_) {
        log_file_ << entry << std::endl;
    }

    if (level >= LogLevel::Warning) {
        std::cerr << entry << std::endl;
    }
}

void Logger::debug(const std::string& message) { log(LogLevel::Debug, message); }
void Logger::info(const std::string& message) { log(LogLevel::Info, message); }
void Logger::warning(const std::string& message) { log(LogLevel::Warning, message); }
void Logger::error(const std::string& message) { log(LogLevel::Error, message); }

std::string Logger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        default: return "UNKNOWN";
    }
}

std::string Logger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now_time_t), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

} // namespace utils
} // namespace stl_explorer
