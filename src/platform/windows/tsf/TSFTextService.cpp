#include "TSFTextService.h"
#include "TSFLanguageBar.h"
#include "ui/candidate/CandidateWindow.h"
#include "utils/WinUtils.h"
#include "core/common/Logger.h"

#include <QApplication>
#include <QScreen>

namespace uim {

// TSF 文本服务 CLSID
// {B5F67F2E-8A7E-4A3D-9C2B-1E4F6A8D2C1B}
static const GUID CLSID_UIMTextService =
{ 0xb5f67f2e, 0x8a7e, 0x4a3d, { 0x9c, 0x2b, 0x1e, 0x4f, 0x6a, 0x8d, 0x2c, 0x1b } };

TSFTextService::TSFTextService()
    : QObject(nullptr)
    , m_refCount(1)
    , m_threadMgr(nullptr)
    , m_clientId(TF_CLIENTID_NULL)
    , m_composition(nullptr)
    , m_currentContext(nullptr)
    , m_keyEventSinkCookie(TF_INVALID_COOKIE)
    , m_threadMgrEventCookie(TF_INVALID_COOKIE)
    , m_pinyinEngine(nullptr)
    , m_candidateWindow(nullptr)
    , m_languageBar(nullptr)
    , m_isActivated(false)
    , m_isComposing(false)
{
    UIM_LOG_INFO("TSFTextService created");
}

TSFTextService::~TSFTextService()
{
    UIM_LOG_INFO("TSFTextService destroyed");

    if (m_candidateWindow) {
        delete m_candidateWindow;
        m_candidateWindow = nullptr;
    }

    if (m_pinyinEngine) {
        delete m_pinyinEngine;
        m_pinyinEngine = nullptr;
    }
}

// IUnknown 接口实现

STDMETHODIMP TSFTextService::QueryInterface(REFIID riid, void** ppvObj)
{
    if (ppvObj == nullptr) {
        return E_INVALIDARG;
    }

    *ppvObj = nullptr;

    if (IsEqualIID(riid, IID_IUnknown) ||
        IsEqualIID(riid, IID_ITfTextInputProcessor)) {
        *ppvObj = static_cast<ITfTextInputProcessor*>(this);
    } else if (IsEqualIID(riid, IID_ITfKeyEventSink)) {
        *ppvObj = static_cast<ITfKeyEventSink*>(this);
    } else if (IsEqualIID(riid, IID_ITfCompositionSink)) {
        *ppvObj = static_cast<ITfCompositionSink*>(this);
    } else if (IsEqualIID(riid, IID_ITfThreadMgrEventSink)) {
        *ppvObj = static_cast<ITfThreadMgrEventSink*>(this);
    } else {
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

STDMETHODIMP_(ULONG) TSFTextService::AddRef()
{
    return InterlockedIncrement(&m_refCount);
}

STDMETHODIMP_(ULONG) TSFTextService::Release()
{
    LONG ref = InterlockedDecrement(&m_refCount);
    if (ref == 0) {
        delete this;
    }
    return ref;
}

// ITfTextInputProcessor 接口实现

STDMETHODIMP TSFTextService::Activate(ITfThreadMgr* ptim, TfClientId tid)
{
    if (ptim == nullptr) {
        return E_INVALIDARG;
    }

    if (m_isActivated) {
        return S_OK;
    }

    UIM_LOG_INFO("TSFTextService activating...");

    m_threadMgr = ptim;
    m_threadMgr->AddRef();
    m_clientId = tid;

    // 初始化拼音引擎
    if (!initPinyinEngine()) {
        UIM_LOG_ERROR("Failed to initialize pinyin engine");
        return E_FAIL;
    }

    // 初始化候选词窗口
    if (!initCandidateWindow()) {
        UIM_LOG_ERROR("Failed to initialize candidate window");
        return E_FAIL;
    }

    // 注册键盘事件
    if (!registerKeyEventSink()) {
        UIM_LOG_WARN("Failed to register key event sink");
    }

    // 注册线程管理器事件
    if (!registerThreadMgrEventSink()) {
        UIM_LOG_WARN("Failed to register thread manager event sink");
    }

    // 创建语言栏
    m_languageBar = new TSFLanguageBar(this);
    m_languageBar->initialize();

    m_isActivated = true;

    UIM_LOG_INFO("TSFTextService activated successfully");
    return S_OK;
}

STDMETHODIMP TSFTextService::Deactivate()
{
    if (!m_isActivated) {
        return S_OK;
    }

    UIM_LOG_INFO("TSFTextService deactivating...");

    // 结束组合
    endComposition();

    // 注销事件
    unregisterKeyEventSink();
    unregisterThreadMgrEventSink();

    // 释放语言栏
    if (m_languageBar) {
        m_languageBar->uninitialize();
        delete m_languageBar;
        m_languageBar = nullptr;
    }

    // 释放候选词窗口
    if (m_candidateWindow) {
        m_candidateWindow->hideWindow();
        delete m_candidateWindow;
        m_candidateWindow = nullptr;
    }

    // 释放拼音引擎
    if (m_pinyinEngine) {
        delete m_pinyinEngine;
        m_pinyinEngine = nullptr;
    }

    // 释放线程管理器
    if (m_threadMgr) {
        m_threadMgr->Release();
        m_threadMgr = nullptr;
    }

    m_clientId = TF_CLIENTID_NULL;
    m_isActivated = false;

    UIM_LOG_INFO("TSFTextService deactivated");
    return S_OK;
}

// ITfKeyEventSink 接口实现

STDMETHODIMP TSFTextService::OnSetFocus(BOOL fForeground)
{
    Q_UNUSED(fForeground);
    return S_OK;
}

STDMETHODIMP TSFTextService::OnTestKeyDown(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten)
{
    Q_UNUSED(pic);
    Q_UNUSED(lParam);

    if (pfEaten == nullptr) {
        return E_INVALIDARG;
    }

    *pfEaten = FALSE;

    // 只处理字母键和一些控制键
    if (wParam >= 'A' && wParam <= 'Z') {
        *pfEaten = TRUE;
    } else if (wParam == VK_SPACE || wParam == VK_RETURN ||
               wParam == VK_BACK || wParam == VK_ESCAPE ||
               wParam == VK_LEFT || wParam == VK_RIGHT ||
               wParam == VK_UP || wParam == VK_DOWN ||
               (wParam >= '1' && wParam <= '9')) {
        if (m_isComposing) {
            *pfEaten = TRUE;
        }
    }

    return S_OK;
}

STDMETHODIMP TSFTextService::OnKeyDown(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten)
{
    if (pfEaten == nullptr) {
        return E_INVALIDARG;
    }

    *pfEaten = FALSE;

    if (!m_pinyinEngine) {
        return S_OK;
    }

    m_currentContext = pic;

    bool handled = false;

    // 字母键
    if (wParam >= 'A' && wParam <= 'Z') {
        // 如果没有在组合，开始组合
        if (!m_isComposing) {
            if (!startComposition(pic)) {
                return S_OK;
            }
        }

        QChar key = QChar(static_cast<wchar_t>(wParam)).toLower();
        handled = m_pinyinEngine->processKey(key);
    }
    // 退格键
    else if (wParam == VK_BACK) {
        if (m_isComposing) {
            handled = m_pinyinEngine->processBackspace();
            if (!m_pinyinEngine->hasComposition()) {
                endComposition();
            }
        }
    }
    // 空格键（提交第一个候选）
    else if (wParam == VK_SPACE) {
        if (m_isComposing) {
            handled = m_pinyinEngine->processSpace();
        }
    }
    // 回车键（提交原始拼音）
    else if (wParam == VK_RETURN) {
        if (m_isComposing) {
            handled = m_pinyinEngine->processEnter();
        }
    }
    // ESC 键（清除输入）
    else if (wParam == VK_ESCAPE) {
        if (m_isComposing) {
            handled = m_pinyinEngine->processEscape();
            endComposition();
        }
    }
    // 数字键（选择候选）
    else if (wParam >= '1' && wParam <= '9') {
        if (m_isComposing) {
            int number = wParam - '0';
            handled = m_pinyinEngine->processNumber(number);
        }
    }
    // 翻页键
    else if (wParam == VK_DOWN || wParam == VK_RIGHT) {
        if (m_isComposing) {
            handled = m_pinyinEngine->processPage(true);
        }
    }
    else if (wParam == VK_UP || wParam == VK_LEFT) {
        if (m_isComposing) {
            handled = m_pinyinEngine->processPage(false);
        }
    }
    // Shift 键（中英文切换）
    else if (wParam == VK_SHIFT) {
        // 只在 Shift 释放时处理，这里先不处理
    }

    if (handled) {
        *pfEaten = TRUE;
        updateComposition();
    }

    return S_OK;
}

STDMETHODIMP TSFTextService::OnTestKeyUp(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten)
{
    Q_UNUSED(pic);
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);

    if (pfEaten) {
        *pfEaten = FALSE;
    }
    return S_OK;
}

STDMETHODIMP TSFTextService::OnKeyUp(ITfContext* pic, WPARAM wParam, LPARAM lParam, BOOL* pfEaten)
{
    Q_UNUSED(pic);
    Q_UNUSED(wParam);
    Q_UNUSED(lParam);

    if (pfEaten) {
        *pfEaten = FALSE;
    }
    return S_OK;
}

STDMETHODIMP TSFTextService::OnGuideline(ITfContext* pic, const GUID* pguid)
{
    Q_UNUSED(pic);
    Q_UNUSED(pguid);
    return S_OK;
}

// ITfCompositionSink 接口实现

STDMETHODIMP TSFTextService::OnCompositionTerminated(TfEditCookie ecWrite, ITfComposition* pComposition)
{
    Q_UNUSED(ecWrite);
    Q_UNUSED(pComposition);

    UIM_LOG_DEBUG("Composition terminated");

    m_isComposing = false;
    m_composition = nullptr;

    if (m_candidateWindow) {
        m_candidateWindow->hideWindow();
    }

    if (m_pinyinEngine) {
        m_pinyinEngine->clear();
    }

    return S_OK;
}

// ITfThreadMgrEventSink 接口实现

STDMETHODIMP TSFTextService::OnInitDocumentMgr(ITfDocumentMgr* pdm)
{
    Q_UNUSED(pdm);
    return S_OK;
}

STDMETHODIMP TSFTextService::OnUninitDocumentMgr(ITfDocumentMgr* pdm)
{
    Q_UNUSED(pdm);
    return S_OK;
}

STDMETHODIMP TSFTextService::OnSetFocus(ITfDocumentMgr* pudmFocus, ITfDocumentMgr* pudmPrev)
{
    Q_UNUSED(pudmFocus);
    Q_UNUSED(pudmPrev);

    // 失去焦点时隐藏候选窗口
    if (pudmFocus == nullptr && m_candidateWindow) {
        m_candidateWindow->hideWindow();
    }

    return S_OK;
}

STDMETHODIMP TSFTextService::OnPushContext(ITfContext* pic)
{
    Q_UNUSED(pic);
    return S_OK;
}

STDMETHODIMP TSFTextService::OnPopContext(ITfContext* pic)
{
    Q_UNUSED(pic);
    return S_OK;
}

// 私有方法

bool TSFTextService::initPinyinEngine()
{
    m_pinyinEngine = new PinyinEngine(this);

    connect(m_pinyinEngine, &PinyinEngine::commitText,
            this, &TSFTextService::onCommitText);
    connect(m_pinyinEngine, &PinyinEngine::candidatesChanged,
            this, &TSFTextService::onCandidatesChanged);
    connect(m_pinyinEngine, &PinyinEngine::compositionChanged,
            this, &TSFTextService::onCompositionChanged);

    return m_pinyinEngine->initialize();
}

bool TSFTextService::initCandidateWindow()
{
    m_candidateWindow = new CandidateWindow();
    m_candidateWindow->setPinyinEngine(m_pinyinEngine);

    return true;
}

bool TSFTextService::startComposition(ITfContext* context)
{
    if (context == nullptr || m_composition != nullptr) {
        return false;
    }

    TfEditCookie ec;
    ITfEditSession* pEditSession = nullptr;

    // 开始组合
    HRESULT hr = context->RequestEditSession(m_clientId, nullptr,
                                             TF_ES_SYNC | TF_ES_READWRITE, &ec);
    if (FAILED(hr)) {
        return false;
    }

    ITfRange* pRange = nullptr;
    hr = context->GetSelection(ec, TF_DEFAULT_SELECTION, 1, nullptr, nullptr);
    if (FAILED(hr)) {
        return false;
    }

    hr = context->StartComposition(ec, nullptr, this, &m_composition);
    if (FAILED(hr)) {
        return false;
    }

    m_isComposing = true;
    m_currentContext = context;

    UIM_LOG_DEBUG("Composition started");
    return true;
}

void TSFTextService::endComposition()
{
    if (m_composition && m_currentContext) {
        TfEditCookie ec;
        if (SUCCEEDED(m_currentContext->RequestEditSession(m_clientId, nullptr,
                                                           TF_ES_SYNC | TF_ES_READWRITE, &ec))) {
            m_composition->EndComposition(ec);
        }
        m_composition->Release();
        m_composition = nullptr;
    }

    m_isComposing = false;

    if (m_candidateWindow) {
        m_candidateWindow->hideWindow();
    }

    UIM_LOG_DEBUG("Composition ended");
}

void TSFTextService::updateComposition()
{
    if (!m_composition || !m_currentContext || !m_pinyinEngine) {
        return;
    }

    TfEditCookie ec;
    if (FAILED(m_currentContext->RequestEditSession(m_clientId, nullptr,
                                                    TF_ES_SYNC | TF_ES_READWRITE, &ec))) {
        return;
    }

    // 获取组合范围
    ITfRange* pRange = nullptr;
    if (SUCCEEDED(m_composition->GetRange(&pRange))) {
        // 设置组合文本
        QString compositionText = m_pinyinEngine->compositionText();
        if (!compositionText.isEmpty()) {
            pRange->SetText(ec, 0, compositionText.toStdWstring().c_str(),
                            compositionText.length());
        }

        pRange->Release();
    }

    // 更新候选词窗口
    if (m_pinyinEngine->hasComposition()) {
        showCandidateWindow();
        updateCandidateWindowPosition();
    } else {
        hideCandidateWindow();
    }
}

void TSFTextService::commitString(const QString& text)
{
    if (!m_currentContext) {
        return;
    }

    TfEditCookie ec;
    if (FAILED(m_currentContext->RequestEditSession(m_clientId, nullptr,
                                                    TF_ES_SYNC | TF_ES_READWRITE, &ec))) {
        return;
    }

    // 如果在组合中，先结束组合
    if (m_composition) {
        ITfRange* pRange = nullptr;
        if (SUCCEEDED(m_composition->GetRange(&pRange))) {
            // 用提交的文本替换组合文本
            pRange->SetText(ec, 0, text.toStdWstring().c_str(), text.length());
            pRange->Release();
        }

        m_composition->EndComposition(ec);
        m_composition->Release();
        m_composition = nullptr;
        m_isComposing = false;
    } else {
        // 直接插入文本
        ITfRange* pRange = nullptr;
        if (SUCCEEDED(m_currentContext->GetSelection(ec, TF_DEFAULT_SELECTION, 1, nullptr, nullptr))) {
            // 这里简化处理，实际需要正确获取和设置选择范围
        }
    }

    hideCandidateWindow();
}

void TSFTextService::updateCandidateWindowPosition()
{
    if (!m_candidateWindow || !m_currentContext) {
        return;
    }

    // 获取光标位置
    QRect caretRect = WinUtils::getCaretRect();

    // 在光标下方显示候选窗口
    int x = caretRect.left();
    int y = caretRect.bottom() + 2;

    m_candidateWindow->showAt(x, y);
}

void TSFTextService::showCandidateWindow()
{
    if (m_candidateWindow) {
        m_candidateWindow->updateCandidates();
        m_candidateWindow->updatePinyin();
        m_candidateWindow->show();
    }
}

void TSFTextService::hideCandidateWindow()
{
    if (m_candidateWindow) {
        m_candidateWindow->hideWindow();
    }
}

bool TSFTextService::registerKeyEventSink()
{
    if (!m_threadMgr) {
        return false;
    }

    ITfKeystrokeMgr* pKeystrokeMgr = nullptr;
    HRESULT hr = m_threadMgr->QueryInterface(IID_ITfKeystrokeMgr,
                                              reinterpret_cast<void**>(&pKeystrokeMgr));
    if (FAILED(hr)) {
        return false;
    }

    hr = pKeystrokeMgr->AdviseKeyEventSink(m_clientId, this, TRUE);
    if (FAILED(hr)) {
        pKeystrokeMgr->Release();
        return false;
    }

    m_keyEventSinkCookie = 1;  // 简化处理
    pKeystrokeMgr->Release();

    UIM_LOG_DEBUG("Key event sink registered");
    return true;
}

void TSFTextService::unregisterKeyEventSink()
{
    if (!m_threadMgr || m_keyEventSinkCookie == TF_INVALID_COOKIE) {
        return;
    }

    ITfKeystrokeMgr* pKeystrokeMgr = nullptr;
    if (SUCCEEDED(m_threadMgr->QueryInterface(IID_ITfKeystrokeMgr,
                                               reinterpret_cast<void**>(&pKeystrokeMgr)))) {
        pKeystrokeMgr->UnadviseKeyEventSink(m_clientId);
        pKeystrokeMgr->Release();
    }

    m_keyEventSinkCookie = TF_INVALID_COOKIE;
}

bool TSFTextService::registerThreadMgrEventSink()
{
    if (!m_threadMgr) {
        return false;
    }

    ITfSource* pSource = nullptr;
    HRESULT hr = m_threadMgr->QueryInterface(IID_ITfSource,
                                              reinterpret_cast<void**>(&pSource));
    if (FAILED(hr)) {
        return false;
    }

    hr = pSource->AdviseSink(IID_ITfThreadMgrEventSink,
                             static_cast<ITfThreadMgrEventSink*>(this),
                             &m_threadMgrEventCookie);
    if (FAILED(hr)) {
        pSource->Release();
        return false;
    }

    pSource->Release();

    UIM_LOG_DEBUG("Thread manager event sink registered");
    return true;
}

void TSFTextService::unregisterThreadMgrEventSink()
{
    if (!m_threadMgr || m_threadMgrEventCookie == TF_INVALID_COOKIE) {
        return;
    }

    ITfSource* pSource = nullptr;
    if (SUCCEEDED(m_threadMgr->QueryInterface(IID_ITfSource,
                                               reinterpret_cast<void**>(&pSource)))) {
        pSource->UnadviseSink(m_threadMgrEventCookie);
        pSource->Release();
    }

    m_threadMgrEventCookie = TF_INVALID_COOKIE;
}

void TSFTextService::onCommitText(const QString& text)
{
    commitString(text);
}

void TSFTextService::onCandidatesChanged()
{
    if (m_candidateWindow && m_isComposing) {
        m_candidateWindow->updateCandidates();
    }
}

void TSFTextService::onCompositionChanged()
{
    if (m_candidateWindow && m_isComposing) {
        m_candidateWindow->updatePinyin();
    }
}

} // namespace uim
