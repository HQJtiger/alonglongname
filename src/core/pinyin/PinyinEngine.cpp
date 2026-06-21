#include "PinyinEngine.h"
#include "common/Logger.h"

#include <QDebug>

namespace uim {

PinyinEngine::PinyinEngine(QObject* parent)
    : QObject(parent)
    , m_state(EngineState::Idle)
    , m_currentPage(0)
    , m_totalPages(0)
    , m_candidatesPerPage(9)
    , m_chineseMode(true)
    , m_parser(nullptr)
    , m_dictionary(nullptr)
{
}

PinyinEngine::~PinyinEngine() = default;

bool PinyinEngine::initialize()
{
    UIM_LOG_INFO("Initializing PinyinEngine...");

    // 获取词库实例并加载
    m_dictionary = &Dictionary::instance();
    if (!m_dictionary->load()) {
        UIM_LOG_ERROR("Failed to load dictionary");
        return false;
    }

    // 获取拼音解析器
    m_parser = &PinyinParser::instance();

    UIM_LOG_INFO(QString("PinyinEngine initialized. Dictionary words: %1")
                 .arg(m_dictionary->totalWords()));

    return true;
}

bool PinyinEngine::processKey(QChar key)
{
    if (!m_chineseMode) {
        // 英文模式下，直接提交字符
        emit commitText(key);
        return true;
    }

    // 只处理字母
    if (!key.isLetter()) {
        return false;
    }

    // 添加到输入缓冲区
    m_inputBuffer += key.toLower();

    // 更新状态
    m_state = EngineState::Inputting;

    // 更新候选词
    updateCandidates();

    emit compositionChanged();
    emit candidatesChanged();

    return true;
}

bool PinyinEngine::processBackspace()
{
    if (m_inputBuffer.isEmpty()) {
        return false;
    }

    // 删除最后一个字符
    m_inputBuffer.chop(1);

    if (m_inputBuffer.isEmpty()) {
        m_state = EngineState::Idle;
        m_candidates.clear();
        m_currentPage = 0;
        m_totalPages = 0;
    } else {
        updateCandidates();
    }

    emit compositionChanged();
    emit candidatesChanged();

    return true;
}

bool PinyinEngine::processEnter()
{
    if (m_inputBuffer.isEmpty()) {
        return false;
    }

    // 回车提交原始拼音（英文）
    emit commitText(m_inputBuffer);
    clear();

    return true;
}

bool PinyinEngine::processSpace()
{
    if (m_inputBuffer.isEmpty()) {
        return false;
    }

    // 空格提交第一个候选
    if (!m_candidates.isEmpty()) {
        commitCandidate(0);
    } else {
        // 没有候选，提交原始拼音
        emit commitText(m_inputBuffer);
        clear();
    }

    return true;
}

bool PinyinEngine::processNumber(int number)
{
    if (number < 1 || number > 9) {
        return false;
    }

    if (m_candidates.isEmpty()) {
        return false;
    }

    // 计算实际索引
    int index = (m_currentPage * m_candidatesPerPage) + (number - 1);
    if (index >= m_candidates.size()) {
        return false;
    }

    commitCandidate(index);

    return true;
}

bool PinyinEngine::processPage(bool next)
{
    if (m_totalPages <= 1) {
        return false;
    }

    if (next) {
        if (m_currentPage < m_totalPages - 1) {
            m_currentPage++;
            emit candidatesChanged();
            return true;
        }
    } else {
        if (m_currentPage > 0) {
            m_currentPage--;
            emit candidatesChanged();
            return true;
        }
    }

    return false;
}

bool PinyinEngine::processEscape()
{
    if (m_inputBuffer.isEmpty()) {
        return false;
    }

    clear();
    return true;
}

QString PinyinEngine::currentPinyin() const
{
    return m_inputBuffer;
}

CandidateList PinyinEngine::candidates() const
{
    // 返回当前页的候选词
    int start = m_currentPage * m_candidatesPerPage;
    int end = qMin(start + m_candidatesPerPage, m_candidates.size());

    CandidateList pageCandidates;
    for (int i = start; i < end; ++i) {
        Candidate c = m_candidates[i];
        c.index = i - start + 1;  // 重新编号为 1-9
        pageCandidates.append(c);
    }

    return pageCandidates;
}

int PinyinEngine::currentPage() const
{
    return m_currentPage;
}

int PinyinEngine::totalPages() const
{
    return m_totalPages;
}

EngineState PinyinEngine::state() const
{
    return m_state;
}

bool PinyinEngine::hasComposition() const
{
    return !m_inputBuffer.isEmpty();
}

QString PinyinEngine::compositionText() const
{
    return m_inputBuffer;
}

void PinyinEngine::clear()
{
    m_inputBuffer.clear();
    m_candidates.clear();
    m_currentPage = 0;
    m_totalPages = 0;
    m_state = EngineState::Idle;

    emit compositionChanged();
    emit candidatesChanged();
}

void PinyinEngine::toggleChineseEnglish()
{
    m_chineseMode = !m_chineseMode;
    emit inputModeChanged(m_chineseMode);

    if (!m_chineseMode && !m_inputBuffer.isEmpty()) {
        // 切换到英文时，提交当前输入
        emit commitText(m_inputBuffer);
        clear();
    }
}

bool PinyinEngine::isChineseMode() const
{
    return m_chineseMode;
}

void PinyinEngine::setChineseMode(bool chinese)
{
    if (m_chineseMode != chinese) {
        m_chineseMode = chinese;
        emit inputModeChanged(m_chineseMode);
    }
}

void PinyinEngine::updateCandidates()
{
    if (m_inputBuffer.isEmpty()) {
        m_candidates.clear();
        m_currentPage = 0;
        m_totalPages = 0;
        return;
    }

    // 查询词库
    m_candidates = m_dictionary->lookup(m_inputBuffer, 50);

    // 如果没有找到候选，添加原始拼音作为候选
    if (m_candidates.isEmpty()) {
        Candidate c;
        c.text = m_inputBuffer;
        c.pinyin = m_inputBuffer;
        c.frequency = 0;
        c.index = 1;
        m_candidates.append(c);
    }

    // 计算页数
    calculatePages();

    // 重置到第一页
    m_currentPage = 0;
}

void PinyinEngine::commitCandidate(int index)
{
    if (index < 0 || index >= m_candidates.size()) {
        return;
    }

    const Candidate& candidate = m_candidates[index];

    // 提交文本
    emit commitText(candidate.text);

    // 更新词频（用户学习）
    m_dictionary->incrementFrequency(candidate.text);

    // 清空输入
    clear();
}

void PinyinEngine::calculatePages()
{
    if (m_candidates.isEmpty()) {
        m_totalPages = 0;
    } else {
        m_totalPages = (m_candidates.size() + m_candidatesPerPage - 1) / m_candidatesPerPage;
    }
}

} // namespace uim
