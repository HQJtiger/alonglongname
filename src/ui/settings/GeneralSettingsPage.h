#pragma once

#include <QWidget>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFormLayout>

namespace uim {

/**
 * @brief 通用设置页
 *
 * 包含输入法的通用设置选项。
 */
class GeneralSettingsPage : public QWidget {
    Q_OBJECT

public:
    explicit GeneralSettingsPage(QWidget* parent = nullptr);
    ~GeneralSettingsPage() override;

    /**
     * @brief 加载设置
     */
    void loadSettings();

    /**
     * @brief 保存设置
     */
    void saveSettings();

private:
    /**
     * @brief 初始化 UI
     */
    void setupUi();

    // 输入设置
    QCheckBox* m_enableChinese;        ///< 默认中文输入
    QCheckBox* m_enableJianpin;        ///< 启用简拼
    QCheckBox* m_enableAutoSpace;      ///< 自动空格
    QSpinBox* m_candidateCount;        ///< 候选词数量

    // 快捷键设置
    QComboBox* m_toggleKey;            ///< 中英文切换键

    // 剪贴板设置
    QCheckBox* m_enableClipboardSync;  ///< 启用剪贴板同步
    QCheckBox* m_enableClipboardHistory;///< 启用剪贴板历史
    QSpinBox* m_clipboardHistorySize;  ///< 剪贴板历史条数

    // 启动设置
    QCheckBox* m_autoStart;            ///< 开机自启
    QCheckBox* m_minimizeToTray;       ///< 最小化到托盘
};

} // namespace uim
