#include "TSFLanguageBar.h"
#include "TSFTextService.h"
#include "core/common/Logger.h"

namespace uim {

// 语言栏项 GUID
// {C8F6D2A1-3B4E-4F5A-9C7D-2E8F1A3B5C7D}
static const GUID GUID_LangBarItem =
{ 0xc8f6d2a1, 0x3b4e, 0x4f5a, { 0x9c, 0x7d, 0x2e, 0x8f, 0x1a, 0x3b, 0x5c, 0x7d } };

// 菜单 ID
enum MenuId {
    MENU_ID_SETTINGS = 1001,
    MENU_ID_ABOUT = 1002,
    MENU_ID_SEPARATOR = 1003,
};

TSFLanguageBar::TSFLanguageBar(TSFTextService* textService)
    : m_refCount(1)
    , m_textService(textService)
    , m_langBarCookie(TF_INVALID_COOKIE)
    , m_itemSink(nullptr)
    , m_isVisible(true)
    , m_isEnabled(true)
{
}

TSFLanguageBar::~TSFLanguageBar()
{
    if (m_itemSink) {
        m_itemSink->Release();
        m_itemSink = nullptr;
    }
}

bool TSFLanguageBar::initialize()
{
    if (!m_textService) {
        return false;
    }

    ITfThreadMgr* threadMgr = m_textService->threadMgr();
    if (!threadMgr) {
        return false;
    }

    ITfLangBarMgr* pLangBarMgr = nullptr;
    HRESULT hr = threadMgr->QueryInterface(IID_ITfLangBarMgr,
                                            reinterpret_cast<void**>(&pLangBarMgr));
    if (FAILED(hr)) {
        UIM_LOG_WARN("Failed to get language bar manager");
        return false;
    }

    hr = pLangBarMgr->AddItem(this, &m_langBarCookie);
    if (FAILED(hr)) {
        UIM_LOG_WARN("Failed to add language bar item");
        pLangBarMgr->Release();
        return false;
    }

    pLangBarMgr->Release();

    UIM_LOG_INFO("Language bar initialized");
    return true;
}

void TSFLanguageBar::uninitialize()
{
    if (!m_textService || m_langBarCookie == TF_INVALID_COOKIE) {
        return;
    }

    ITfThreadMgr* threadMgr = m_textService->threadMgr();
    if (!threadMgr) {
        return;
    }

    ITfLangBarMgr* pLangBarMgr = nullptr;
    if (SUCCEEDED(threadMgr->QueryInterface(IID_ITfLangBarMgr,
                                             reinterpret_cast<void**>(&pLangBarMgr)))) {
        pLangBarMgr->RemoveItem(m_langBarCookie);
        pLangBarMgr->Release();
    }

    m_langBarCookie = TF_INVALID_COOKIE;

    if (m_itemSink) {
        m_itemSink->Release();
        m_itemSink = nullptr;
    }

    UIM_LOG_INFO("Language bar uninitialized");
}

// IUnknown 接口实现

STDMETHODIMP TSFLanguageBar::QueryInterface(REFIID riid, void** ppvObj)
{
    if (ppvObj == nullptr) {
        return E_INVALIDARG;
    }

    *ppvObj = nullptr;

    if (IsEqualIID(riid, IID_IUnknown) ||
        IsEqualIID(riid, IID_ITfLangBarItem) ||
        IsEqualIID(riid, IID_ITfLangBarItemButton)) {
        *ppvObj = static_cast<ITfLangBarItemButton*>(this);
    } else if (IsEqualIID(riid, IID_ITfSource)) {
        *ppvObj = static_cast<ITfSource*>(this);
    } else {
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

STDMETHODIMP_(ULONG) TSFLanguageBar::AddRef()
{
    return InterlockedIncrement(&m_refCount);
}

STDMETHODIMP_(ULONG) TSFLanguageBar::Release()
{
    LONG ref = InterlockedDecrement(&m_refCount);
    if (ref == 0) {
        delete this;
    }
    return ref;
}

// ITfLangBarItem 接口实现

STDMETHODIMP TSFLanguageBar::GetInfo(TF_LANGBARITEMINFO* pInfo)
{
    if (pInfo == nullptr) {
        return E_INVALIDARG;
    }

    pInfo->clsidService = GUID_NULL;  // 简化处理
    pInfo->guidItem = GUID_LangBarItem;
    pInfo->dwStyle = TF_LBI_STYLE_BTN_MENU;
    pInfo->ulSort = 0;

    wcscpy_s(pInfo->szDescription, L"统一输入法");

    return S_OK;
}

STDMETHODIMP TSFLanguageBar::GetStatus(DWORD* pdwStatus)
{
    if (pdwStatus == nullptr) {
        return E_INVALIDARG;
    }

    *pdwStatus = 0;

    if (m_isVisible) {
        *pdwStatus |= TF_LBI_STATUS_SHOWN;
    }

    if (m_isEnabled) {
        *pdwStatus |= TF_LBI_STATUS_ENABLED;
    }

    return S_OK;
}

STDMETHODIMP TSFLanguageBar::Show(BOOL fShow)
{
    m_isVisible = fShow ? true : false;

    if (m_itemSink) {
        m_itemSink->OnUpdate(TF_LBI_STATUS | TF_LBI_ICON);
    }

    return S_OK;
}

STDMETHODIMP TSFLanguageBar::GetTooltipString(BSTR* pbstrToolTip)
{
    if (pbstrToolTip == nullptr) {
        return E_INVALIDARG;
    }

    *pbstrToolTip = SysAllocString(L"统一输入法 - 点击打开菜单");
    return S_OK;
}

// ITfLangBarItemButton 接口实现

STDMETHODIMP TSFLanguageBar::OnClick(TfLBIClick click, POINT pt, const RECT* prcArea)
{
    Q_UNUSED(pt);
    Q_UNUSED(prcArea);

    if (click == TF_LBI_CLK_LEFT) {
        // 左键点击：切换中英文
        // 这里简化处理，后续实现
        UIM_LOG_DEBUG("Language bar left click");
    }

    return S_OK;
}

STDMETHODIMP TSFLanguageBar::InitMenu(ITfMenu* pMenu)
{
    if (pMenu == nullptr) {
        return E_INVALIDARG;
    }

    // 添加菜单项
    pMenu->AddMenuItem(MENU_ID_SETTINGS, 0,
                       L"设置", nullptr, nullptr, 0, nullptr);
    pMenu->AddMenuItem(MENU_ID_ABOUT, 0,
                       L"关于", nullptr, nullptr, 0, nullptr);

    return S_OK;
}

STDMETHODIMP TSFLanguageBar::OnMenuSelect(UINT wID)
{
    switch (wID) {
    case MENU_ID_SETTINGS:
        UIM_LOG_INFO("Settings menu selected");
        // TODO: 打开设置窗口
        break;

    case MENU_ID_ABOUT:
        UIM_LOG_INFO("About menu selected");
        // TODO: 显示关于对话框
        break;

    default:
        break;
    }

    return S_OK;
}

STDMETHODIMP TSFLanguageBar::GetIcon(HICON* phIcon)
{
    if (phIcon == nullptr) {
        return E_INVALIDARG;
    }

    // MVP 阶段使用默认图标
    *phIcon = LoadIcon(nullptr, IDI_APPLICATION);
    return S_OK;
}

STDMETHODIMP TSFLanguageBar::GetText(BSTR* pbstrText)
{
    if (pbstrText == nullptr) {
        return E_INVALIDARG;
    }

    *pbstrText = SysAllocString(L"中");
    return S_OK;
}

// ITfSource 接口实现

STDMETHODIMP TSFLanguageBar::AdviseSink(REFIID riid, IUnknown* punk, DWORD* pdwCookie)
{
    if (pdwCookie == nullptr || punk == nullptr) {
        return E_INVALIDARG;
    }

    if (!IsEqualIID(riid, IID_ITfLangBarItemSink)) {
        return E_INVALIDARG;
    }

    if (m_itemSink) {
        return CONNECT_E_ADVISELIMIT;
    }

    HRESULT hr = punk->QueryInterface(IID_ITfLangBarItemSink,
                                       reinterpret_cast<void**>(&m_itemSink));
    if (FAILED(hr)) {
        return hr;
    }

    *pdwCookie = 1;
    return S_OK;
}

STDMETHODIMP TSFLanguageBar::UnadviseSink(DWORD dwCookie)
{
    if (dwCookie != 1 || m_itemSink == nullptr) {
        return CONNECT_E_NOCONNECTION;
    }

    m_itemSink->Release();
    m_itemSink = nullptr;

    return S_OK;
}

} // namespace uim
