#include "TSFModule.h"
#include "utils/WinUtils.h"
#include "core/common/Logger.h"

#include <msctf.h>

namespace uim {

// TSF 文本服务 CLSID
// {B5F67F2E-8A7E-4A3D-9C2B-1E4F6A8D2C1B}
static const GUID CLSID_UIMTextService =
{ 0xb5f67f2e, 0x8a7e, 0x4a3d, { 0x9c, 0x2b, 0x1e, 0x4f, 0x6a, 0x8d, 0x2c, 0x1b } };

TSFModule::TSFModule(QObject* parent)
    : QObject(parent)
    , m_initialized(false)
    , m_registered(false)
    , m_description(QStringLiteral("统一输入法"))
    , m_profileId(CLSID_UIMTextService)
{
}

TSFModule::~TSFModule()
{
    if (m_initialized) {
        shutdown();
    }
}

bool TSFModule::initialize()
{
    if (m_initialized) {
        return true;
    }

    UIM_LOG_INFO("Initializing TSF module...");

    // 初始化 COM
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        UIM_LOG_ERROR(QString("Failed to initialize COM, hr: %1").arg(hr, 0, 16));
        return false;
    }

    m_initialized = true;

    UIM_LOG_INFO("TSF module initialized");
    return true;
}

void TSFModule::shutdown()
{
    if (!m_initialized) {
        return;
    }

    UIM_LOG_INFO("Shutting down TSF module...");

    CoUninitialize();

    m_initialized = false;
    UIM_LOG_INFO("TSF module shut down");
}

bool TSFModule::registerInputMethod(const QString& dllPath)
{
    if (!m_initialized) {
        if (!initialize()) {
            return false;
        }
    }

    UIM_LOG_INFO(QString("Registering input method: %1").arg(dllPath));

    // 注册 DLL
    if (!WinUtils::registerDll(dllPath)) {
        UIM_LOG_ERROR("Failed to register DLL");
        return false;
    }

    // 注册 TSF 文本服务
    if (!WinUtils::registerInputMethod(dllPath, m_profileId, m_description)) {
        UIM_LOG_ERROR("Failed to register TSF input method");
        return false;
    }

    m_registered = true;
    UIM_LOG_INFO("Input method registered successfully");
    return true;
}

bool TSFModule::unregisterInputMethod()
{
    if (!m_initialized) {
        return false;
    }

    UIM_LOG_INFO("Unregistering input method...");

    // 注销 TSF 文本服务
    if (!WinUtils::unregisterInputMethod(m_profileId)) {
        UIM_LOG_WARN("Failed to unregister TSF input method");
    }

    m_registered = false;
    UIM_LOG_INFO("Input method unregistered");
    return true;
}

bool TSFModule::isRegistered() const
{
    return m_registered;
}

} // namespace uim
