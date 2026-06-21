#pragma once

#include <QString>
#include <QStringList>
#include <QVector>

namespace uim {

/**
 * @brief 拼音音节信息
 */
struct PinyinSyllable {
    QString syllable;     ///< 完整音节
    QString initial;      ///< 声母
    QString final;        ///< 韵母
    bool isComplete;      ///< 是否完整音节

    PinyinSyllable() : isComplete(false) {}
};

/**
 * @brief 拼音解析结果
 */
struct PinyinParseResult {
    QString originalInput;        ///< 原始输入
    QStringList syllables;        ///< 切分后的音节列表
    QString jianpin;              ///< 简拼（首字母）
    bool isValid;                 ///< 是否有效
    int completedSyllableCount;   ///< 已完成的音节数

    PinyinParseResult() : isValid(false), completedSyllableCount(0) {}
};

/**
 * @brief 拼音解析器
 *
 * 负责解析用户输入的拼音字符串，进行音节切分、
 * 简拼识别、模糊音处理等。
 */
class PinyinParser {
public:
    /**
     * @brief 获取单例实例
     */
    static PinyinParser& instance();

    /**
     * @brief 解析拼音字符串
     * @param input 用户输入的拼音
     * @return 解析结果
     */
    PinyinParseResult parse(const QString& input) const;

    /**
     * @brief 尝试切分音节
     * @param input 输入字符串
     * @return 切分后的音节列表
     */
    QStringList splitSyllables(const QString& input) const;

    /**
     * @brief 判断是否是完整的拼音音节
     * @param syllable 音节
     * @return 是否完整
     */
    bool isCompleteSyllable(const QString& syllable) const;

    /**
     * @brief 获取简拼（每个音节首字母）
     * @param syllables 音节列表
     * @return 简拼字符串
     */
    QString getJianpin(const QStringList& syllables) const;

    /**
     * @brief 是否是简拼输入
     *
     * 简拼判断规则：所有字符都是辅音字母，且没有完整音节
     */
    bool isJianpinInput(const QString& input) const;

    /**
     * @brief 获取拼音的首字母
     */
    QString getFirstLetter(const QString& pinyin) const;

private:
    PinyinParser();
    ~PinyinParser();
    PinyinParser(const PinyinParser&) = delete;
    PinyinParser& operator=(const PinyinParser&) = delete;

    /**
     * @brief 初始化声母和韵母列表
     */
    void initPinyinData();

    /**
     * @brief 正向最大匹配切分
     */
    QStringList forwardMaxMatch(const QString& input) const;

    /**
     * @brief 反向最大匹配切分
     */
    QStringList backwardMaxMatch(const QString& input) const;

    QVector<QString> m_initials;    ///< 声母列表
    QVector<QString> m_finals;      ///< 韵母列表
    QVector<QString> m_completeSyllables; ///< 完整音节列表
};

} // namespace uim
