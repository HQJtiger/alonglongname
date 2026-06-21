#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

#include "common/Types.h"
#include "PinyinParser.h"
#include "dictionary/Dictionary.h"

namespace uim {

/**
 * @brief 拼音引擎状态
 */
enum class EngineState {
    Idle,           ///< 空闲状态
    Inputting,      ///< 输入中
    Committing,     ///< 提交中
    Selecting       ///< 候选选择中
};

/**
 * @brief 拼音引擎
 *
 * 拼音输入法的核心引擎，负责管理输入状态、处理按键、
 * 生成候选词、提交文本等。
 */
class PinyinEngine : public QObject {
    Q_OBJECT

public:
    explicit PinyinEngine(QObject* parent = nullptr);
    ~PinyinEngine() override;

    /**
     * @brief 初始化引擎
     * @return 是否成功
     */
    bool initialize();

    /**
     * @brief 处理按键输入
     * @param key 按键字符
     * @return 是否处理了该按键
     */
    bool processKey(QChar key);

    /**
     * @brief 处理退格键
     * @return 是否处理
     */
    bool processBackspace();

    /**
     * @brief 处理回车键（提交第一个候选）
     * @return 是否处理
     */
    bool processEnter();

    /**
     * @brief 处理空格键（提交第一个候选）
     * @return 是否处理
     */
    bool processSpace();

    /**
     * @brief 处理数字键（选择候选）
     * @param number 数字（1-9）
     * @return 是否处理
     */
    bool processNumber(int number);

    /**
     * @brief 处理翻页
     * @param next true=下一页，false=上一页
     * @return 是否处理
     */
    bool processPage(bool next);

    /**
     * @brief 处理 ESC 键（清除输入）
     * @return 是否处理
     */
    bool processEscape();

    /**
     * @brief 获取当前输入的拼音
     */
    QString currentPinyin() const;

    /**
     * @brief 获取当前候选词列表
     */
    CandidateList candidates() const;

    /**
     * @brief 获取当前页码
     */
    int currentPage() const;

    /**
     * @brief 获取总页数
     */
    int totalPages() const;

    /**
     * @brief 获取引擎状态
     */
    EngineState state() const;

    /**
     * @brief 是否有未提交的输入
     */
    bool hasComposition() const;

    /**
     * @brief 获取预编辑文本（组合串）
     */
    QString compositionText() const;

    /**
     * @brief 清空所有输入
     */
    void clear();

    /**
     * @brief 切换中英文模式
     */
    void toggleChineseEnglish();

    /**
     * @brief 是否是中文模式
     */
    bool isChineseMode() const;

    /**
     * @brief 设置中文模式
     */
    void setChineseMode(bool chinese);

signals:
    /**
     * @brief 候选词更新信号
     */
    void candidatesChanged();

    /**
     * @brief 提交文本信号
     * @param text 要提交的文本
     */
    void commitText(const QString& text);

    /**
     * @brief 预编辑文本变化信号
     */
    void compositionChanged();

    /**
     * @brief 输入模式变化信号
     */
    void inputModeChanged(bool isChinese);

private:
    /**
     * @brief 更新候选词
     */
    void updateCandidates();

    /**
     * @brief 提交候选词
     * @param index 候选索引（0-based）
     */
    void commitCandidate(int index);

    /**
     * @brief 计算总页数
     */
    void calculatePages();

    EngineState m_state;          ///< 引擎状态
    QString m_inputBuffer;        ///< 输入缓冲区（原始拼音）
    CandidateList m_candidates;   ///< 当前候选词列表
    int m_currentPage;            ///< 当前页码
    int m_totalPages;             ///< 总页数
    int m_candidatesPerPage;      ///< 每页候选词数量
    bool m_chineseMode;           ///< 是否中文模式

    PinyinParser* m_parser;       ///< 拼音解析器
    Dictionary* m_dictionary;     ///< 词库
};

} // namespace uim
