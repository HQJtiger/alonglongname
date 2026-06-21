/**
 * @file main.cpp
 * @brief 统一输入法主程序入口
 *
 * 主程序负责显示设置界面、系统托盘图标等。
 * TSF 输入法插件是独立的 DLL。
 */

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>

#include "ui/settings/SettingsDialog.h"
#include "config/ConfigManager.h"
#include "core/common/Logger.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

int main(int argc, char* argv[])
{
    // 设置应用程序信息
    QApplication::setApplicationName(QStringLiteral("UnifiedInputMethod"));
    QApplication::setApplicationVersion(QStringLiteral("0.1.0"));
    QApplication::setOrganizationName(QStringLiteral("UnifiedInputMethod"));

    QApplication app(argc, argv);

    // 初始化日志
    uim::Logger::instance().setLogLevel(uim::LogLevel::Info);
    UIM_LOG_INFO("Unified Input Method starting...");

    // 初始化配置
    uim::ConfigManager::instance().load();

    // 创建设置对话框
    uim::SettingsDialog settingsDialog;

    // 创建系统托盘图标
    QSystemTrayIcon* trayIcon = new QSystemTrayIcon(&app);
    trayIcon->setIcon(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon));
    trayIcon->setToolTip(QObject::tr("统一输入法"));

    // 创建托盘菜单
    QMenu* trayMenu = new QMenu();

    QAction* settingsAction = trayMenu->addAction(QObject::tr("设置"));
    QObject::connect(settingsAction, &QAction::triggered, &settingsDialog, &QDialog::show);

    trayMenu->addSeparator();

    QAction* aboutAction = trayMenu->addAction(QObject::tr("关于"));
    QObject::connect(aboutAction, &QAction::triggered, []() {
        QMessageBox::about(nullptr,
            QObject::tr("关于统一输入法"),
            QObject::tr("<h3>统一输入法</h3>"
                        "<p>版本 0.1.0 (MVP)</p>"
                        "<p>一个跨平台的开源输入法</p>"
                        "<p>包含拼音输入、剪贴板同步、语音输入等功能</p>"));
    });

    trayMenu->addSeparator();

    QAction* quitAction = trayMenu->addAction(QObject::tr("退出"));
    QObject::connect(quitAction, &QAction::triggered, &app, &QApplication::quit);

    trayIcon->setContextMenu(trayMenu);

    // 双击托盘图标打开设置
    QObject::connect(trayIcon, &QSystemTrayIcon::activated,
                     [&settingsDialog](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::DoubleClick) {
            settingsDialog.show();
            settingsDialog.activateWindow();
        }
    });

    trayIcon->show();

    // 显示欢迎提示
    trayIcon->showMessage(
        QObject::tr("统一输入法"),
        QObject::tr("输入法已启动，双击托盘图标打开设置"),
        QSystemTrayIcon::Information,
        3000
    );

    UIM_LOG_INFO("Unified Input Method started");

    return app.exec();
}
