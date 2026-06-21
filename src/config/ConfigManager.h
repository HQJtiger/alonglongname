#pragma once

#include <QObject>
#include <QString>
#include <QVariant>
#include <QSettings>

namespace uim {

/**
 * @brief 配置管理器
 *
 * 负责管理输入法的所有配置项，支持读写和持久化。
 * 使用单例模式。
 */
class ConfigManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief 获取单例实例
     */
    static ConfigManager& instance();

    /**
     * @brief 加载配置
     * @return 是否成功
     */
    bool load();

    /**
     * @brief 保存配置
     * @return 是否成功
     */
    bool save();

    /**
     * @brief 获取配置值
     * @param key 配置键
     * @param defaultValue 默认值
     * @return 配置值
     */
    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;

    /**
     * @brief 设置配置值
     * @param key 配置键
     * @param value 配置值
     */
    void setValue(const QString& key, const QVariant& value);

    /**
     * @brief 检查配置是否存在
     * @param key 配置键
     * @return 是否存在
     */
    bool contains(const QString& key) const;

    /**
     * @brief 移除配置项
     * @param key 配置键
     */
    void remove(const QString& key);

    /**
     * @brief 重置为默认配置
     */
    void resetToDefaults();

    /**
     * @brief 获取配置文件路径
     */
    QString configFilePath() const;

signals:
    /**
     * @brief 配置变化信号
     * @param key 变化的配置键
     */
    void configChanged(const QString& key);

private:
    ConfigManager();
    ~ConfigManager() override;
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    /**
     * @brief 初始化默认配置
     */
    void initDefaults();

    QSettings* m_settings;   ///< 设置对象
    bool m_dirty;            ///< 是否有未保存的修改
};

} // namespace uim
