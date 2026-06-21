#pragma once

#include <QObject>
#include <QString>
#include <windows.h>

namespace uim {

/**
 * @brief TSF 模块管理类
 *
 * 负责管理 TSF 输入法模块的初始化、注册、注销等。
 */
class TSFModule : public QObject {
    Q_OBJECT

public:
    explicit TSFModule(QObject* parent = nullptr);
    ~TSFModule() override;

    /**
     * @brief 初始化 TSF 模块
     * @return 是否成功
     */
    bool initialize();

    /**
     * @brief 反初始化 TSF 模块
     */
    void shutdown();

    /**
     * @brief 注册输入法到系统
     * @param dllPath DLL 路径
     * @return 是否成功
     */
    bool registerInputMethod(const QString& dllPath);

    /**
     * @brief 从系统注销输入法
     * @return 是否成功
     */
    bool unregisterInputMethod();

    /**
     * @brief 检查输入法是否已注册
     */
    bool isRegistered() const;

    /**
     * @brief 获取输入法描述
     */
    QString description() const { return m_description; }

    /**
     * @brief 设置输入法描述
     */
    void setDescription(const QString& desc) { m_description = desc; }

private:
    bool m_initialized;     ///< 是否已初始化
    bool m_registered;      ///< 是否已注册
    QString m_description;  ///< 输入法描述
    GUID m_profileId;       ///< 配置文件 ID
};

} // namespace uim
