#pragma once

#include <QString>
#include <QStringList>
#include <QHash>
#include <QVector>
#include <QPair>

#include "common/Types.h"

namespace uim {

/**
 * @brief 词库条目
 */
struct DictionaryEntry {
    QString word;       ///< 词语
    QString pinyin;     ///< 完整拼音（带空格分隔）
    int frequency;      ///< 词频

    DictionaryEntry() : frequency(0) {}
    DictionaryEntry(const QString& w, const QString& p, int freq = 0)
        : word(w), pinyin(p), frequency(freq) {}
};

/**
 * @brief 词库类
 *
 * 负责管理拼音词库，支持按拼音查询候选词。
 * MVP 阶段使用简化版内置词库，后续支持加载外部词库。
 */
class Dictionary {
public:
    /**
     * @brief 获取单例实例
     */
    static Dictionary& instance();

    /**
     * @brief 加载词库
     * @return 是否成功
     */
    bool load();

    /**
     * @brief 从文件加载词库
     * @param filePath 词库文件路径
     * @return 是否成功
     */
    bool loadFromFile(const QString& filePath);

    /**
     * @brief 加载内置简化词库
     * @return 是否成功
     */
    bool loadBuiltin();

    /**
     * @brief 查询候选词
     * @param pinyin 拼音（支持简拼和全拼）
     * @param maxCount 最大返回数量
     * @return 候选词列表
     */
    CandidateList lookup(const QString& pinyin, int maxCount = 10) const;

    /**
     * @brief 查询完整拼音匹配的词
     * @param pinyin 完整拼音
     * @param maxCount 最大返回数量
     * @return 候选词列表
     */
    CandidateList lookupExact(const QString& pinyin, int maxCount = 10) const;

    /**
     * @brief 查询简拼匹配的词
     * @param jianpin 简拼
     * @param maxCount 最大返回数量
     * @return 候选词列表
     */
    CandidateList lookupJianpin(const QString& jianpin, int maxCount = 10) const;

    /**
     * @brief 获取词库总词数
     */
    int totalWords() const;

    /**
     * @brief 是否已经加载
     */
    bool isLoaded() const;

    /**
     * @brief 添加用户词
     * @param word 词语
     * @param pinyin 拼音
     * @param frequency 词频
     */
    void addUserWord(const QString& word, const QString& pinyin, int frequency = 100);

    /**
     * @brief 增加词频
     * @param word 词语
     * @param delta 增加量
     */
    void incrementFrequency(const QString& word, int delta = 1);

private:
    Dictionary();
    ~Dictionary();
    Dictionary(const Dictionary&) = delete;
    Dictionary& operator=(const Dictionary&) = delete;

    /**
     * @brief 构建拼音索引
     */
    void buildIndex();

    /**
     * @brief 获取简拼（每个字的首字母）
     */
    QString getJianpin(const QString& pinyin) const;

    bool m_loaded;
    QVector<DictionaryEntry> m_words;           ///< 所有词条
    QHash<QString, QVector<int>> m_pinyinIndex; ///< 拼音 -> 词条索引列表
    QHash<QString, QVector<int>> m_jianpinIndex;///< 简拼 -> 词条索引列表
    QHash<QString, int> m_wordFrequency;        ///< 词频（用户学习用）
};

} // namespace uim
