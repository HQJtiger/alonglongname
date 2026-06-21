#include "WinUtils.h"
#include "core/common/Logger.h"

#include <QGuiApplication>
#include <QScreen>
#include <shlwapi.h>
#include <msctf.h>
#include <inputScope.h>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "msctf.lib")

namespace uim {

QRect WinUtils::getCaretRect()
{
    GUITHREADINFO gti = {0};
    gti.cbSize = sizeof(GUITHREADINFO);

    DWORD threadId = GetWindowThreadProcessId(GetForegroundWindow(), nullptr);
    if (GetGUIThreadInfo(threadId, &gti)) {
        if (gti.hwndCaret != nullptr) {
            RECT rect = gti.rcCaret;
            MapWindowPoints(gti.hwndCaret, nullptr, reinterpret_cast<POINT*>(&rect), 2);
            return QRect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
        }
    }

    // 如果获取不到光标位置，返回鼠标位置
    POINT pt;
    GetCursorPos(&pt);
    return QRect(pt.x, pt.y, 0, 20);
}

HWND WinUtils::getActiveWindow()
{
    return GetForegroundWindow();
}

QString WinUtils::getWindowTitle(HWND hwnd)
{
    if (!hwnd) {
        return QString();
    }

    wchar_t title[256] = {0};
    GetWindowTextW(hwnd, title, 256);
    return QString::fromWCharArray(title);
}

QString WinUtils::getWindowClassName(HWND hwnd)
{
    if (!hwnd) {
        return QString();
    }

    wchar_t className[256] = {0};
    GetClassNameW(hwnd, className, 256);
    return QString::fromWCharArray(className);
}

bool WinUtils::registerDll(const QString& dllPath)
{
    HMODULE hModule = LoadLibraryW(reinterpret_cast<LPCWSTR>(dllPath.utf16()));
    if (!hModule) {
        UIM_LOG_ERROR(QString("Failed to load DLL: %1, error: %2")
                      .arg(dllPath).arg(GetLastError()));
        return false;
    }

    typedef HRESULT (STDAPICALLTYPE *DllRegisterServerFunc)();
    DllRegisterServerFunc pfn = reinterpret_cast<DllRegisterServerFunc>(
        GetProcAddress(hModule, "DllRegisterServer"));

    bool result = false;
    if (pfn) {
        result = SUCCEEDED(pfn());
    }

    FreeLibrary(hModule);
    return result;
}

bool WinUtils::unregisterDll(const QString& dllPath)
{
    HMODULE hModule = LoadLibraryW(reinterpret_cast<LPCWSTR>(dllPath.utf16()));
    if (!hModule) {
        UIM_LOG_ERROR(QString("Failed to load DLL: %1, error: %2")
                      .arg(dllPath).arg(GetLastError()));
        return false;
    }

    typedef HRESULT (STDAPICALLTYPE *DllUnregisterServerFunc)();
    DllUnregisterServerFunc pfn = reinterpret_cast<DllUnregisterServerFunc>(
        GetProcAddress(hModule, "DllUnregisterServer"));

    bool result = false;
    if (pfn) {
        result = SUCCEEDED(pfn());
    }

    FreeLibrary(hModule);
    return result;
}

int WinUtils::getSystemDpi()
{
    HDC hdc = GetDC(nullptr);
    int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(nullptr, hdc);
    return dpi;
}

int WinUtils::physicalToLogicalPixels(int physicalPixels)
{
    int dpi = getSystemDpi();
    return MulDiv(physicalPixels, 96, dpi);
}

int WinUtils::logicalToPhysicalPixels(int logicalPixels)
{
    int dpi = getSystemDpi();
    return MulDiv(logicalPixels, dpi, 96);
}

bool WinUtils::isHighDpi()
{
    return getSystemDpi() > 96;
}

HKL WinUtils::getCurrentKeyboardLayout()
{
    return GetKeyboardLayout(0);
}

bool WinUtils::switchInputMethod(HKL hkl)
{
    return PostMessage(GetForegroundWindow(), WM_INPUTLANGCHANGEREQUEST, 0,
                       reinterpret_cast<LPARAM>(hkl)) != 0;
}

bool WinUtils::registerInputMethod(const QString& dllPath,
                                    const GUID& profileId,
                                    const QString& description)
{
    // 注册 TSF 文本服务
    ITfInputProcessorProfiles* pProfiles = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_TF_InputProcessorProfiles,
                                  nullptr, CLSCTX_INPROC_SERVER,
                                  IID_ITfInputProcessorProfiles,
                                  reinterpret_cast<void**>(&pProfiles));
    if (FAILED(hr)) {
        UIM_LOG_ERROR(QString("Failed to create TSF profiles object, hr: %1").arg(hr, 0, 16));
        return false;
    }

    // 注册语言配置文件
    hr = pProfiles->RegisterTextService(profileId);
    if (FAILED(hr)) {
        UIM_LOG_ERROR(QString("Failed to register text service, hr: %1").arg(hr, 0, 16));
        pProfiles->Release();
        return false;
    }

    // 添加到中文语言
    hr = pProfiles->AddLanguageProfile(profileId,
                                        MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED),
                                        profileId,
                                        description.toStdWstring().c_str(),
                                        description.length(),
                                        nullptr, 0, 0);
    if (FAILED(hr)) {
        UIM_LOG_WARN(QString("Failed to add language profile, hr: %1").arg(hr, 0, 16));
        // 继续执行，不视为致命错误
    }

    pProfiles->Release();

    UIM_LOG_INFO("Input method registered successfully");
    return true;
}

bool WinUtils::unregisterInputMethod(const GUID& profileId)
{
    ITfInputProcessorProfiles* pProfiles = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_TF_InputProcessorProfiles,
                                  nullptr, CLSCTX_INPROC_SERVER,
                                  IID_ITfInputProcessorProfiles,
                                  reinterpret_cast<void**>(&pProfiles));
    if (FAILED(hr)) {
        UIM_LOG_ERROR(QString("Failed to create TSF profiles object, hr: %1").arg(hr, 0, 16));
        return false;
    }

    hr = pProfiles->UnregisterTextService(profileId);
    pProfiles->Release();

    if (FAILED(hr)) {
        UIM_LOG_ERROR(QString("Failed to unregister text service, hr: %1").arg(hr, 0, 16));
        return false;
    }

    UIM_LOG_INFO("Input method unregistered successfully");
    return true;
}

QString WinUtils::getAppDataPath()
{
    wchar_t path[MAX_PATH] = {0};
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, path))) {
        return QString::fromWCharArray(path) + "\\UnifiedInputMethod";
    }
    return QString();
}

QString WinUtils::getInstallPath()
{
    wchar_t path[MAX_PATH] = {0};
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    QString filePath = QString::fromWCharArray(path);
    return QFileInfo(filePath).absolutePath();
}

} // namespace uim
