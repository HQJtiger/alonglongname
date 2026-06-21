#pragma once

#include <windows.h>
#include <msctf.h>
#include <QString>

namespace uim {

class TSFTextService;

/**
 * @brief TSF 语言栏
 *
 * 管理输入法在任务栏语言栏上的图标和菜单。
 */
class TSFLanguageBar : public ITfLangBarItemButton,
                        public ITfSource {
public:
    explicit TSFLanguageBar(TSFTextService* textService);
    ~TSFLanguageBar();

    /**
     * @brief 初始化语言栏
     */
    bool initialize();

    /**
     * @brief 反初始化语言栏
     */
    void uninitialize();

    // IUnknown 接口
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObj);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // ITfLangBarItem 接口
    STDMETHODIMP GetInfo(TF_LANGBARITEMINFO* pInfo);
    STDMETHODIMP GetStatus(DWORD* pdwStatus);
    STDMETHODIMP Show(BOOL fShow);
    STDMETHODIMP GetTooltipString(BSTR* pbstrToolTip);

    // ITfLangBarItemButton 接口
    STDMETHODIMP OnClick(TfLBIClick click, POINT pt, const RECT* prcArea);
    STDMETHODIMP InitMenu(ITfMenu* pMenu);
    STDMETHODIMP OnMenuSelect(UINT wID);
    STDMETHODIMP GetIcon(HICON* phIcon);
    STDMETHODIMP GetText(BSTR* pbstrText);

    // ITfSource 接口
    STDMETHODIMP AdviseSink(REFIID riid, IUnknown* punk, DWORD* pdwCookie);
    STDMETHODIMP UnadviseSink(DWORD dwCookie);

private:
    LONG m_refCount;                    ///< 引用计数
    TSFTextService* m_textService;      ///< 文本服务
    DWORD m_langBarCookie;              ///< 语言栏 Cookie
    ITfLangBarItemSink* m_itemSink;     ///< 语言栏项事件接收器

    bool m_isVisible;                   ///< 是否可见
    bool m_isEnabled;                   ///< 是否启用
};

} // namespace uim
