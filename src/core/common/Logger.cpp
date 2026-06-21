#include "Logger.h"

#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QtDebug>

namespace uim {

Logger& Logger::instance()
{
    static Logger instance;
    return instance;
}

Logger::Logger()
    : m_logLevel(LogLevel::Info)
    , m_logToFile(false)
{
}

Logger::~Logger() = default;

void Logger::setLogLevel(LogLevel level)
{
    m_logLevel = level;
}

LogLevel Logger::logLevel() const
{
    return m_logLevel;
}

bool Logger::setLogFile(const QString& filePath)
{
    m_logFilePath = filePath;
    m_logToFile = !filePath.isEmpty();
    return true;
}

void Logger::debug(const QString& message)
{
    instance().log(LogLevel::Debug, message);
}

void Logger::info(const QString& message)
{
    instance().log(LogLevel::Info, message);
}

void Logger::warning(const QString& message)
{
    instance().log(LogLevel::Warning, message);
}

void Logger::error(const QString& message)
{
    instance().log(LogLevel::Error, message);
}

void Logger::fatal(const QString& message)
{
    instance().log(LogLevel::Fatal, message);
}

void Logger::log(LogLevel level, const QString& message)
{
    if (level < m_logLevel) {
        return;
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString levelStr = levelToString(level);
    QString logMessage = QString("[%1] [%2] %3").arg(timestamp, levelStr, message);

    // 输出到控制台
    switch (level) {
    case LogLevel::Debug:
        qDebug().noquote() << logMessage;
        break;
    case LogLevel::Info:
        qInfo().noquote() << logMessage;
        break;
    case LogLevel::Warning:
        qWarning().noquote() << logMessage;
        break;
    case LogLevel::Error:
    case LogLevel::Fatal:
        qCritical().noquote() << logMessage;
        break;
    }

    // 输出到文件
    if (m_logToFile && !m_logFilePath.isEmpty()) {
        QFile file(m_logFilePath);
        if (file.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << logMessage << "\n";
            file.close();
        }
    }
}

QString Logger::levelToString(LogLevel level) const
{
    switch (level) {
    case LogLevel::Debug:   return "DEBUG";
    case LogLevel::Info:    return "INFO";
    case LogLevel::Warning: return "WARN";
    case LogLevel::Error:   return "ERROR";
    case LogLevel::Fatal:   return "FATAL";
    default:                return "UNKNOWN";
    }
}

} // namespace uim
