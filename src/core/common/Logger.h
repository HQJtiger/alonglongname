#pragma once

#include <QString>
#include <QMessageLogger>

namespace uim {

/**
 * @brief 日志级别
 */
enum class LogLevel {
    Debug,      ///< 调试信息
    Info,       ///< 一般信息
    Warning,    ///< 警告
    Error,      ///< 错误
    Fatal       ///< 致命错误
};

/**
 * @brief 日志工具类
 *
 * 提供统一的日志输出接口，支持不同级别和输出目标。
 */
class Logger {
public:
    /**
     * @brief 获取单例实例
     */
    static Logger& instance();

    /**
     * @brief 设置日志级别
     * @param level 日志级别
     */
    void setLogLevel(LogLevel level);

    /**
     * @brief 获取当前日志级别
     */
    LogLevel logLevel() const;

    /**
     * @brief 设置日志文件路径
     * @param filePath 日志文件路径
     * @return 是否成功
     */
    bool setLogFile(const QString& filePath);

    /**
     * @brief 输出调试日志
     */
    static void debug(const QString& message);

    /**
     * @brief 输出信息日志
     */
    static void info(const QString& message);

    /**
     * @brief 输出警告日志
     */
    static void warning(const QString& message);

    /**
     * @brief 输出错误日志
     */
    static void error(const QString& message);

    /**
     * @brief 输出致命错误日志
     */
    static void fatal(const QString& message);

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void log(LogLevel level, const QString& message);
    QString levelToString(LogLevel level) const;

    LogLevel m_logLevel;
    QString m_logFilePath;
    bool m_logToFile;
};

} // namespace uim

// 便捷宏
#define UIM_LOG_DEBUG(msg) uim::Logger::debug(msg)
#define UIM_LOG_INFO(msg)  uim::Logger::info(msg)
#define UIM_LOG_WARN(msg)  uim::Logger::warning(msg)
#define UIM_LOG_ERROR(msg) uim::Logger::error(msg)
#define UIM_LOG_FATAL(msg) uim::Logger::fatal(msg)
