#include "ConfigManager.h"
#include "core/common/Logger.h"

#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>

namespace uim {

ConfigManager& ConfigManager::instance()
{
    static ConfigManager instance;
    return instance;
}

ConfigManager::ConfigManager()
    : QObject(nullptr)
    , m_settings(nullptr)
    , m_dirty(false)
{
    // 获取配置文件路径
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (configDir.isEmpty()) {
        configDir = QDir::homePath() + "/.uim";
    }

    QDir().mkpath(configDir);
    QString configFile = configDir + "/config.ini";

    m_settings = new QSettings(configFile, QSettings::IniFormat, this);

    UIM_LOG_INFO(QString("Config file: %1").arg(configFile));

    // 初始化默认配置
    initDefaults();
}

ConfigManager::~ConfigManager()
{
    // 自动保存
    if (m_dirty) {
        save();
    }
}

bool ConfigManager::load()
{
    // QSettings 会自动加载，这里主要是确保文件存在
    if (m_settings->status() != QSettings::NoError) {
        UIM_LOG_ERROR("Failed to load config file");
        return false;
    }

    UIM_LOG_INFO("Config loaded successfully");
    return true;
}

bool ConfigManager::save()
{
    m_settings->sync();

    if (m_settings->status() != QSettings::NoError) {
        UIM_LOG_ERROR("Failed to save config file");
        return false;
    }

    m_dirty = false;
    UIM_LOG_DEBUG("Config saved");
    return true;
}

QVariant ConfigManager::value(const QString& key, const QVariant& defaultValue) const
{
    return m_settings->value(key, defaultValue);
}

void ConfigManager::setValue(const QString& key, const QVariant& value)
{
    if (m_settings->value(key) != value) {
        m_settings->setValue(key, value);
        m_dirty = true;
        emit configChanged(key);
    }
}

bool ConfigManager::contains(const QString& key) const
{
    return m_settings->contains(key);
}

void ConfigManager::remove(const QString& key)
{
    if (m_settings->contains(key)) {
        m_settings->remove(key);
        m_dirty = true;
        emit configChanged(key);
    }
}

void ConfigManager::resetToDefaults()
{
    m_settings->clear();
    initDefaults();
    m_dirty = true;
    emit configChanged(QString());
}

QString ConfigManager::configFilePath() const
{
    return m_settings->fileName();
}

void ConfigManager::initDefaults()
{
    // 输入设置
    if (!contains("input/defaultChinese")) {
        setValue("input/defaultChinese", true);
    }
    if (!contains("input/enableJianpin")) {
        setValue("input/enableJianpin", true);
    }
    if (!contains("input/autoSpace")) {
        setValue("input/autoSpace", false);
    }
    if (!contains("input/candidateCount")) {
        setValue("input/candidateCount", 9);
    }
    if (!contains("input/toggleKey")) {
        setValue("input/toggleKey", 0);  // Shift
    }

    // 外观设置
    if (!contains("appearance/theme")) {
        setValue("appearance/theme", 0);  // 浅色
    }
    if (!contains("appearance/followSystem")) {
        setValue("appearance/followSystem", true);
    }
    if (!contains("appearance/fontSize")) {
        setValue("appearance/fontSize", 14);
    }
    if (!contains("appearance/fontFamily")) {
        setValue("appearance/fontFamily", QString());
    }
    if (!contains("appearance/opacity")) {
        setValue("appearance/opacity", 95);
    }
    if (!contains("appearance/windowWidth")) {
        setValue("appearance/windowWidth", 280);
    }
    if (!contains("appearance/showPinyin")) {
        setValue("appearance/showPinyin", true);
    }
    if (!contains("appearance/showIndex")) {
        setValue("appearance/showIndex", true);
    }
    if (!contains("appearance/candidateStyle")) {
        setValue("appearance/candidateStyle", 0);  // 横排
    }
    if (!contains("appearance/highlightFirst")) {
        setValue("appearance/highlightFirst", true);
    }

    // 剪贴板设置
    if (!contains("clipboard/enableSync")) {
        setValue("clipboard/enableSync", false);
    }
    if (!contains("clipboard/enableHistory")) {
        setValue("clipboard/enableHistory", true);
    }
    if (!contains("clipboard/historySize")) {
        setValue("clipboard/historySize", 100);
    }

    // 常规设置
    if (!contains("general/autoStart")) {
        setValue("general/autoStart", false);
    }
    if (!contains("general/minimizeToTray")) {
        setValue("general/minimizeToTray", true);
    }

    // 语音设置
    if (!contains("voice/enabled")) {
        setValue("voice/enabled", false);
    }
    if (!contains("voice/engine")) {
        setValue("voice/engine", "vosk");
    }
    if (!contains("voice/modelPath")) {
        setValue("voice/modelPath", QString());
    }

    m_dirty = false;
}

} // namespace uim
