#pragma once

#include <QString>
#include <QRect>
#include <windows.h>

namespace uim {

/**
 * @brief Windows 平台工具函数
 *
 * 提供 Windows 平台相关的工具函数。
 */
class WinUtils {
public:
    /**
     * @brief 获取当前光标位置
     * @return 光标在屏幕上的位置
     */
    static QRect getCaretRect();

    /**
     * @brief 获取当前活动窗口
     * @return 窗口句柄
     */
    static HWND getActiveWindow();

    /**
     * @brief 获取窗口标题
     * @param hwnd 窗口句柄
     * @return 窗口标题
     */
    static QString getWindowTitle(HWND hwnd);

    /**
     * @brief 获取窗口类名
     * @param hwnd 窗口句柄
     * @return 窗口类名
     */
    static QString getWindowClassName(HWND hwnd);

    /**
     * @brief 注册 DLL
     * @param dllPath DLL 路径
     * @return 是否成功
     */
    static bool registerDll(const QString& dllPath);

    /**
     * @brief 注销 DLL
     * @param dllPath DLL 路径
     * @return 是否成功
     */
    static bool unregisterDll(const QString& dllPath);

    /**
     * @brief 获取系统 DPI
     * @return DPI 值
     */
    static int getSystemDpi();

    /**
     * @brief 物理像素转逻辑像素
     * @param physicalPixels 物理像素
     * @return 逻辑像素
     */
    static int physicalToLogicalPixels(int physicalPixels);

    /**
     * @brief 逻辑像素转物理像素
     * @param logicalPixels 逻辑像素
     * @return 物理像素
     */
    static int logicalToPhysicalPixels(int logicalPixels);

    /**
     * @brief 是否是高 DPI 屏幕
     */
    static bool isHighDpi();

    /**
     * @brief 获取当前键盘布局
     * @return 键盘布局 ID
     */
    static HKL getCurrentKeyboardLayout();

    /**
     * @brief 切换输入法
     * @param hkl 键盘布局句柄
     * @return 是否成功
     */
    static bool switchInputMethod(HKL hkl);

    /**
     * @brief 注册输入法到系统
     * @param dllPath DLL 路径
     * @param profileId 配置文件 ID
     * @param description 描述
     * @return 是否成功
     */
    static bool registerInputMethod(const QString& dllPath,
                                     const GUID& profileId,
                                     const QString& description);

    /**
     * @brief 从系统注销输入法
     * @param profileId 配置文件 ID
     * @return 是否成功
     */
    static bool unregisterInputMethod(const GUID& profileId);

    /**
     * @brief 获取程序数据目录
     * @return 目录路径
     */
    static QString getAppDataPath();

    /**
     * @brief 获取程序安装目录
     * @return 目录路径
     */
    static QString getInstallPath();
};

} // namespace uim
