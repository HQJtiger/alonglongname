/**
 * @file TSFPlugin.cpp
 * @brief TSF 输入法插件 DLL 入口
 *
 * 实现 DLL 入口和 COM 类工厂，用于 Windows TSF 输入法注册。
 */

#include <windows.h>
#include <msctf.h>
#include <olectl.h>

#include "TSFTextService.h"
#include "core/common/Logger.h"

// TSF 文本服务 CLSID
// {B5F67F2E-8A7E-4A3D-9C2B-1E4F6A8D2C1B}
static const GUID CLSID_UIMTextService =
{ 0xb5f67f2e, 0x8a7e, 0x4a3d, { 0x9c, 0x2b, 0x1e, 0x4f, 0x6a, 0x8d, 0x2c, 0x1b } };

// 配置文件 GUID（与 CLSID 相同，简化处理）
static const GUID GUID_Profile = CLSID_UIMTextService;

// DLL 模块句柄
HINSTANCE g_hInstance = nullptr;

// DLL 引用计数
LONG g_dllRefCount = 0;

// 输入法描述
static const wchar_t* kDescription = L"统一输入法";
static const wchar_t* kDescriptionFile = L"UnifiedInputMethodTSF.dll";

/**
 * @brief COM 类工厂
 */
class CClassFactory : public IClassFactory {
public:
    CClassFactory() : m_refCount(1) {}

    // IUnknown 接口
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObj) override {
        if (ppvObj == nullptr) return E_INVALIDARG;
        *ppvObj = nullptr;

        if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory)) {
            *ppvObj = static_cast<IClassFactory*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() override {
        return InterlockedIncrement(&m_refCount);
    }

    STDMETHODIMP_(ULONG) Release() override {
        LONG ref = InterlockedDecrement(&m_refCount);
        if (ref == 0) delete this;
        return ref;
    }

    // IClassFactory 接口
    STDMETHODIMP CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObj) override {
        if (ppvObj == nullptr) return E_INVALIDARG;
        *ppvObj = nullptr;

        if (pUnkOuter != nullptr) {
            return CLASS_E_NOAGGREGATION;
        }

        // 创建 TSF 文本服务对象
        uim::TSFTextService* pTextService = new uim::TSFTextService();
        if (pTextService == nullptr) {
            return E_OUTOFMEMORY;
        }

        HRESULT hr = pTextService->QueryInterface(riid, ppvObj);
        pTextService->Release();

        return hr;
    }

    STDMETHODIMP LockServer(BOOL fLock) override {
        if (fLock) {
            InterlockedIncrement(&g_dllRefCount);
        } else {
            InterlockedDecrement(&g_dllRefCount);
        }
        return S_OK;
    }

private:
    LONG m_refCount;
};

/**
 * @brief DLL 入口函数
 */
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    Q_UNUSED(lpReserved);

    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        g_hInstance = hModule;
        DisableThreadLibraryCalls(hModule);
        break;

    case DLL_PROCESS_DETACH:
        g_hInstance = nullptr;
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }

    return TRUE;
}

/**
 * @brief 获取类对象
 */
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    if (ppv == nullptr) {
        return E_INVALIDARG;
    }

    *ppv = nullptr;

    // 只处理我们的 CLSID
    if (!IsEqualCLSID(rclsid, CLSID_UIMTextService)) {
        return CLASS_E_CLASSNOTAVAILABLE;
    }

    CClassFactory* pFactory = new CClassFactory();
    if (pFactory == nullptr) {
        return E_OUTOFMEMORY;
    }

    HRESULT hr = pFactory->QueryInterface(riid, ppv);
    pFactory->Release();

    return hr;
}

/**
 * @brief 检查 DLL 是否可以卸载
 */
STDAPI DllCanUnloadNow(void)
{
    return g_dllRefCount == 0 ? S_OK : S_FALSE;
}

/**
 * @brief 注册服务器
 */
STDAPI DllRegisterServer(void)
{
    // 注册 COM 组件
    wchar_t szModule[MAX_PATH];
    if (GetModuleFileNameW(g_hInstance, szModule, MAX_PATH) == 0) {
        return SELFREG_E_CLASS;
    }

    // 注册 CLSID
    HRESULT hr = SELFREG_E_CLASS;

    // 创建注册表项
    HKEY hKey = nullptr;
    wchar_t szClsid[64];
    StringFromGUID2(CLSID_UIMTextService, szClsid, _countof(szClsid));

    wchar_t szKeyPath[256];
    swprintf_s(szKeyPath, L"CLSID\\%s", szClsid);

    if (RegCreateKeyExW(HKEY_CLASSES_ROOT, szKeyPath, 0, nullptr,
                        REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
        // 设置默认值
        RegSetValueExW(hKey, nullptr, 0, REG_SZ,
                       reinterpret_cast<const BYTE*>(kDescription),
                       static_cast<DWORD>(wcslen(kDescription) + 1) * sizeof(wchar_t));

        // 创建 InprocServer32 子键
        HKEY hInprocKey = nullptr;
        if (RegCreateKeyExW(hKey, L"InprocServer32", 0, nullptr,
                            REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr,
                            &hInprocKey, nullptr) == ERROR_SUCCESS) {
            RegSetValueExW(hInprocKey, nullptr, 0, REG_SZ,
                           reinterpret_cast<const BYTE*>(szModule),
                           static_cast<DWORD>(wcslen(szModule) + 1) * sizeof(wchar_t));

            RegSetValueExW(hInprocKey, L"ThreadingModel", 0, REG_SZ,
                           reinterpret_cast<const BYTE*>(L"Apartment"),
                           sizeof(L"Apartment"));

            RegCloseKey(hInprocKey);
        }

        RegCloseKey(hKey);
        hr = S_OK;
    }

    // 注册 TSF 文本服务
    ITfInputProcessorProfiles* pProfiles = nullptr;
    if (SUCCEEDED(CoCreateInstance(CLSID_TF_InputProcessorProfiles,
                                    nullptr, CLSCTX_INPROC_SERVER,
                                    IID_ITfInputProcessorProfiles,
                                    reinterpret_cast<void**>(&pProfiles)))) {
        // 注册文本服务
        pProfiles->RegisterTextService(CLSID_UIMTextService);

        // 添加中文语言配置文件
        pProfiles->AddLanguageProfile(CLSID_UIMTextService,
                                       MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED),
                                       GUID_Profile,
                                       kDescription,
                                       static_cast<ULONG>(wcslen(kDescription)),
                                       nullptr, 0, 0);

        // 启用配置文件
        pProfiles->EnableLanguageProfile(CLSID_UIMTextService,
                                          MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED),
                                          GUID_Profile,
                                          TRUE);

        pProfiles->Release();
    }

    return hr;
}

/**
 * @brief 注销服务器
 */
STDAPI DllUnregisterServer(void)
{
    HRESULT hr = S_OK;

    // 注销 TSF 文本服务
    ITfInputProcessorProfiles* pProfiles = nullptr;
    if (SUCCEEDED(CoCreateInstance(CLSID_TF_InputProcessorProfiles,
                                    nullptr, CLSCTX_INPROC_SERVER,
                                    IID_ITfInputProcessorProfiles,
                                    reinterpret_cast<void**>(&pProfiles)))) {
        // 移除语言配置文件
        pProfiles->RemoveLanguageProfile(CLSID_UIMTextService,
                                          MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED),
                                          GUID_Profile);

        // 注销文本服务
        pProfiles->UnregisterTextService(CLSID_UIMTextService);

        pProfiles->Release();
    }

    // 删除注册表项
    wchar_t szClsid[64];
    StringFromGUID2(CLSID_UIMTextService, szClsid, _countof(szClsid));

    wchar_t szKeyPath[256];
    swprintf_s(szKeyPath, L"CLSID\\%s", szClsid);

    // 递归删除注册表项
    RegDeleteTreeW(HKEY_CLASSES_ROOT, szKeyPath);

    return hr;
}
