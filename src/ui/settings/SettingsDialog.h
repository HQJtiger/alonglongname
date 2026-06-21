#pragma once

#include <QDialog>
#include <QListWidget>
#include <QStackedWidget>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace uim {

class GeneralSettingsPage;
class AppearanceSettingsPage;

/**
 * @brief 设置对话框
 *
 * 输入法设置主窗口，包含多个设置页面。
 */
class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    ~SettingsDialog() override;

    /**
     * @brief 加载设置
     */
    void loadSettings();

    /**
     * @brief 保存设置
     */
    void saveSettings();

signals:
    /**
     * @brief 设置变化信号
     */
    void settingsChanged();

private slots:
    /**
     * @brief 切换页面
     */
    void onPageChanged(int index);

    /**
     * @brief 确定按钮
     */
    void onAccepted();

    /**
     * @brief 应用按钮
     */
    void onApplied();

private:
    /**
     * @brief 初始化 UI
     */
    void setupUi();

    /**
     * @brief 创建设置页面
     */
    void createPages();

    QListWidget* m_navigationList;     ///< 左侧导航列表
    QStackedWidget* m_contentStack;    ///< 右侧内容堆栈

    GeneralSettingsPage* m_generalPage;      ///< 通用设置页
    AppearanceSettingsPage* m_appearancePage;///< 外观设置页

    QDialogButtonBox* m_buttonBox;     ///< 按钮框
};

} // namespace uim
