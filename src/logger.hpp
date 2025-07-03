#pragma once
#include <iostream>
#include <mutex>
#include <string>

/**
 * @enum LogLevel
 * @brief Represents the severity level of log messages
 *
 * Verbose: Very detailed messages for debugging purposes
 * Debug: Detailed messages for debugging purposes
 * Info: General informational messages
 * Warn: Warnings indicating potential issues
 * Error: Errors indicating failures or critical problems
 */
enum class LogLevel { Verbose, Debug, Info, Warn, Error };

/**
 * @class Logger
 * @brief Thread-safe logging utility for outputting messages with severity levels
 *
 * Provides static logging methods and serializes output using a mutex to avoid
 * race conditions when logging from multiple threads
 */
class Logger {
  public:
    /// @brief The log level that defines which log output should be captured
    static LogLevel logLevel;

    /**
     * @brief Logs a message with the specified severity level
     * @param level Severity level of the log message
     * @param message The message to log
     */
    static void log(LogLevel level, const std::string &message);

  private:
    /// Mutex to synchronize access to logging output
    static std::mutex logMutex;

    /**
     * @brief Converts a LogLevel enum to a readable string
     * @param level The log level to convert
     * @return A string representation of the log level
     */
    static std::string levelToString(LogLevel level);
};

/**
 * @def LOG_VERBOSE
 * @brief Macro for logging a verbose-level message
 */
#define LOG_VERBOSE(msg) Logger::log(LogLevel::Verbose, msg)

/**
 * @def LOG_DEBUG
 * @brief Macro for logging a debug-level message
 */
#define LOG_DEBUG(msg) Logger::log(LogLevel::Debug, msg)

/**
 * @def LOG_INFO
 * @brief Macro for logging an info-level message
 */
#define LOG_INFO(msg) Logger::log(LogLevel::Info, msg)

/**
 * @def LOG_WARN
 * @brief Macro for logging a warning-level message
 */
#define LOG_WARN(msg) Logger::log(LogLevel::Warn, msg)

/**
 * @def LOG_ERROR
 * @brief Macro for logging an error-level message
 */
#define LOG_ERROR(msg) Logger::log(LogLevel::Error, msg)
