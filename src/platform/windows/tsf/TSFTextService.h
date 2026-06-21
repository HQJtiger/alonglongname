#pragma once

#include <windows.h>
#include <msctf.h>
#include <QObject>
#include <QString>

#include "core/pinyin/PinyinEngine.h"

namespace uim {

class CandidateWindow;
class TSFLanguageBar;

/**
 * @brief TSF 文本服务
 *
 * 实现 TSF 文本服务接口，是输入法的核心 COM 类。
 * 负责处理键盘输入、管理候选词窗口、提交文本等。
 */
class TSFTextService : public QObject,
                        public ITfTextInputProcessor,
                        public ITfKeyEventSink,
                        public ITfCompositionSink,
                        public ITfThreadMgrEventSink {
    Q_OBJECT

public:
    TSFTextService();
    ~TSFTextService() override;

    // IUnknown 接口
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObj) override;
    STDMETHODIMP_(ULONG) AddRef() override;
    STDMETHODIMP_(ULONG) Release() override;

    // ITfTextInputProcessor 接口
    STDMETHODIMP Activate(ITfThreadMgr* ptim, TfClientId tid) override;
    STDMETHODIMP Deactivate() override;

    // ITfKeyEventSink 接口
    STDMETHODIMP OnSetFocus(BOOL fForeground) override;
    STDMETHODIMP OnTestKeyDown(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten) override;
    STDMETHODIMP OnKeyDown(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten) override;
    STDMETHODIMP OnTestKeyUp(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten) override;
    STDMETHODIMP OnKeyUp(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten) override;
    STDMETHODIMP OnGuideline(ITfContext* pic, const GUID* pguid) override;

    // ITfCompositionSink 接口
    STDMETHODIMP OnCompositionTerminated(TfEditCookie ecWrite, ITfComposition* pComposition) override;

    // ITfThreadMgrEventSink 接口
    STDMETHODIMP OnInitDocumentMgr(ITfDocumentMgr* pdm) override;
    STDMETHODIMP OnUninitDocumentMgr(ITfDocumentMgr* pdm) override;
    STDMETHODIMP OnSetFocus(ITfDocumentMgr* pudmFocus, ITfDocumentMgr* pudmPrev) override;
    STDMETHODIMP OnPushContext(ITfContext* pic) override;
    STDMETHODIMP OnPopContext(ITfContext* pic) override;

    /**
     * @brief 获取客户端 ID
     */
    TfClientId clientId() const { return m_clientId; }

    /**
     * @brief 获取线程管理器
     */
    ITfThreadMgr* threadMgr() const { return m_threadMgr; }

    /**
     * @brief 获取拼音引擎
     */
    PinyinEngine* pinyinEngine() const { return m_pinyinEngine; }

private slots:
    /**
     * @brief 处理提交文本
     */
    void onCommitText(const QString& text);

    /**
     * @brief 候选词变化
     */
    void onCandidatesChanged();

    /**
     * @brief 预编辑文本变化
     */
    void onCompositionChanged();

private:
    /**
     * @brief 初始化拼音引擎
     */
    bool initPinyinEngine();

    /**
     * @brief 初始化候选词窗口
     */
    bool initCandidateWindow();

    /**
     * @brief 开始组合
     */
    bool startComposition(ITfContext* context);

    /**
     * @brief 结束组合
     */
    void endComposition();

    /**
     * @brief 更新组合文本
     */
    void updateComposition();

    /**
     * @brief 提交文本
     */
    void commitString(const QString& text);

    /**
     * @brief 更新候选词窗口位置
     */
    void updateCandidateWindowPosition();

    /**
     * @brief 显示候选词窗口
     */
    void showCandidateWindow();

    /**
     * @brief 隐藏候选词窗口
     */
    void hideCandidateWindow();

    /**
     * @brief 注册键盘事件
     */
    bool registerKeyEventSink();

    /**
     * @brief 注销键盘事件
     */
    void unregisterKeyEventSink();

    /**
     * @brief 注册线程管理器事件
     */
    bool registerThreadMgrEventSink();

    /**
     * @brief 注销线程管理器事件
     */
    void unregisterThreadMgrEventSink();

    LONG m_refCount;                    ///< 引用计数
    ITfThreadMgr* m_threadMgr;          ///< 线程管理器
    TfClientId m_clientId;              ///< 客户端 ID
    ITfComposition* m_composition;      ///< 当前组合
    ITfContext* m_currentContext;       ///< 当前上下文

    DWORD m_keyEventSinkCookie;         ///< 键盘事件 Cookie
    DWORD m_threadMgrEventCookie;       ///< 线程管理器事件 Cookie

    PinyinEngine* m_pinyinEngine;       ///< 拼音引擎
    CandidateWindow* m_candidateWindow; ///< 候选词窗口
    TSFLanguageBar* m_languageBar;      ///< 语言栏

    bool m_isActivated;                 ///< 是否已激活
    bool m_isComposing;                 ///< 是否正在组合
};

} // namespace uim
