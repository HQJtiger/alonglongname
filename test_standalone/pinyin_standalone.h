/**
 * @file pinyin_standalone.h
 * @brief 纯 C++ 拼音引擎（不依赖 Qt）
 *
 * 用于验证核心逻辑，可在 Linux 下编译运行。
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cctype>

namespace uim {

// 候选词结构
struct Candidate {
    std::wstring text;       // 候选词文本
    std::string pinyin;      // 拼音
    int frequency;           // 词频
    int index;               // 序号
};

using CandidateList = std::vector<Candidate>;

// 拼音解析结果
struct PinyinParseResult {
    std::string rawInput;           // 原始输入
    std::vector<std::string> syllables; // 音节列表
    bool isJianpin;                 // 是否是简拼
    bool isValid;                   // 是否有效
    int completedSyllables;         // 已完成音节数
};

/**
 * @brief 拼音解析器
 *
 * 负责将输入的拼音字符串切分成音节。
 */
class PinyinParser {
public:
    static PinyinParser& instance();

    /**
     * @brief 解析拼音输入
     * @param input 拼音字符串
     * @return 解析结果
     */
    PinyinParseResult parse(const std::string& input) const;

    /**
     * @brief 判断是否是简拼输入
     */
    bool isJianpinInput(const std::string& input) const;

    /**
     * @brief 判断是否是完整音节
     */
    bool isCompleteSyllable(const std::string& syllable) const;

private:
    PinyinParser();
    void initSyllables();

    // 音节列表（约 400+ 个完整音节）
    std::vector<std::string> m_allSyllables;

    // 声母列表
    std::vector<std::string> m_initials;

    // 快速查找
    bool isSyllable(const std::string& s) const;
};

/**
 * @brief 词库管理
 *
 * 内置简化词库，支持全拼和简拼查询。
 */
class Dictionary {
public:
    static Dictionary& instance();

    /**
     * @brief 初始化词库
     */
    void initialize();

    /**
     * @brief 综合查询（全拼 + 简拼）
     */
    CandidateList lookup(const std::string& pinyin) const;

    /**
     * @brief 全拼精确查询
     */
    CandidateList lookupExact(const std::string& pinyin) const;

    /**
     * @brief 简拼查询
     */
    CandidateList lookupJianpin(const std::string& jianpin) const;

    /**
     * @brief 增加词频
     */
    void incrementFrequency(const std::wstring& text);

    /**
     * @brief 添加用户词
     */
    void addUserWord(const std::wstring& text, const std::string& pinyin, int frequency);

private:
    Dictionary();
    void buildIndex();

    // 词库数据: 拼音 -> 候选词列表
    std::unordered_map<std::string, CandidateList> m_pinyinIndex;

    // 简拼索引: 简拼 -> 候选词列表
    std::unordered_map<std::string, CandidateList> m_jianpinIndex;

    bool m_initialized;
};

/**
 * @brief 拼音引擎
 *
 * 核心输入处理逻辑。
 */
class PinyinEngine {
public:
    PinyinEngine();

    /**
     * @brief 处理字母输入
     * @return true 如果处理成功
     */
    bool processKey(char key);

    /**
     * @brief 处理退格
     */
    bool processBackspace();

    /**
     * @brief 处理空格（提交第一个候选）
     * @return 提交的文本，空字符串表示失败
     */
    std::wstring processSpace();

    /**
     * @brief 处理回车（提交原始拼音）
     */
    std::string processEnter();

    /**
     * @brief 处理数字键选择候选（1-9）
     * @return 选中的候选词文本
     */
    std::wstring processNumber(int num);

    /**
     * @brief 翻页
     * @param forward true=下一页，false=上一页
     * @return true 如果翻页成功
     */
    bool processPage(bool forward);

    /**
     * @brief ESC 清除输入
     */
    void processEscape();

    /**
     * @brief 获取当前输入
     */
    const std::string& currentInput() const { return m_input; }

    /**
     * @brief 获取当前页候选词
     */
    const CandidateList& currentCandidates() const { return m_currentPage; }

    /**
     * @brief 获取当前页码
     */
    int currentPage() const { return m_currentPageNum; }

    /**
     * @brief 获取总页数
     */
    int totalPages() const { return m_totalPages; }

    /**
     * @brief 是否有输入
     */
    bool hasInput() const { return !m_input.empty(); }

    /**
     * @brief 每页候选词数量
     */
    static constexpr int candidatesPerPage = 9;

private:
    void updateCandidates();
    void updatePage();

    std::string m_input;
    CandidateList m_allCandidates;
    CandidateList m_currentPage;

    int m_currentPageNum;
    int m_totalPages;
};

} // namespace uim
