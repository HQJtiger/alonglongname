#pragma once

#include <QWidget>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QSlider>

namespace uim {

/**
 * @brief 外观设置页
 *
 * 包含输入法外观和主题设置。
 */
class AppearanceSettingsPage : public QWidget {
    Q_OBJECT

public:
    explicit AppearanceSettingsPage(QWidget* parent = nullptr);
    ~AppearanceSettingsPage() override;

    /**
     * @brief 加载设置
     */
    void loadSettings();

    /**
     * @brief 保存设置
     */
    void saveSettings();

private slots:
    /**
     * @brief 主题变化
     */
    void onThemeChanged(int index);

private:
    /**
     * @brief 初始化 UI
     */
    void setupUi();

    // 主题设置
    QComboBox* m_themeCombo;           ///< 主题选择
    QCheckBox* m_followSystem;         ///< 跟随系统主题

    // 字体设置
    QSpinBox* m_fontSize;              ///< 字体大小
    QComboBox* m_fontFamily;           ///< 字体选择

    // 窗口设置
    QSlider* m_opacitySlider;          ///< 窗口透明度
    QSpinBox* m_windowWidth;           ///< 窗口宽度
    QCheckBox* m_showPinyin;           ///< 显示拼音
    QCheckBox* m_showIndex;            ///< 显示序号

    // 候选词样式
    QComboBox* m_candidateStyle;       ///< 候选词样式
    QCheckBox* m_highlightFirst;       ///< 高亮第一个候选
};

} // namespace uim
