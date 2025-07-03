#include "logger.hpp"

std::mutex Logger::logMutex;
LogLevel Logger::logLevel = LogLevel::Debug;

void Logger::log(LogLevel level, const std::string &message) {
    if (logLevel > level) {
        return;
    }
    // Lock mutex for thread safety
    std::lock_guard<std::mutex> lock(logMutex);

    // Select output stream based on log level
    std::ostream &out = (level == LogLevel::Error) ? std::cerr : std::cout;
    out << "[" << levelToString(level) << "] " << message << std::endl;
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
    case LogLevel::Debug:
        return "Debug";
    case LogLevel::Info:
        return "Info";
    case LogLevel::Warn:
        return "Warn";
    case LogLevel::Error:
        return "Error";
    default:
        return "Unknown";
    }
}
