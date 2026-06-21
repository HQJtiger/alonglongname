#include "PinyinParser.h"
#include "common/Logger.h"

#include <QSet>
#include <algorithm>

namespace uim {

PinyinParser& PinyinParser::instance()
{
    static PinyinParser instance;
    return instance;
}

PinyinParser::PinyinParser()
{
    initPinyinData();
}

PinyinParser::~PinyinParser() = default;

void PinyinParser::initPinyinData()
{
    // 声母列表
    m_initials = {
        "b", "p", "m", "f", "d", "t", "n", "l",
        "g", "k", "h", "j", "q", "x",
        "zh", "ch", "sh", "r", "z", "c", "s",
        "y", "w"
    };

    // 韵母列表
    m_finals = {
        "a", "o", "e", "i", "u", "v", "ü",
        "ai", "ei", "ui", "ao", "ou", "iu", "ie", "üe", "ve", "er",
        "an", "en", "in", "un", "ün", "vn",
        "ang", "eng", "ing", "ong",
        "ia", "ua", "uo", "uai", "uei", "iao", "iou", "ian", "uan", "üan", "van",
        "iang", "uang", "ueng", "iong", "ong"
    };

    // 常用完整音节列表（简化版）
    m_completeSyllables = {
        "a", "ai", "an", "ang", "ao",
        "ba", "bai", "ban", "bang", "bao", "bei", "ben", "beng", "bi", "bian", "biao",
        "bie", "bin", "bing", "bo", "bu",
        "ca", "cai", "can", "cang", "cao", "ce", "cen", "ceng", "cha", "chai",
        "chan", "chang", "chao", "che", "chen", "cheng", "chi", "chong", "chou",
        "chu", "chuan", "chuang", "chui", "chun", "chuo", "ci", "cong", "cou",
        "cu", "cuan", "cui", "cun", "cuo",
        "da", "dai", "dan", "dang", "dao", "de", "deng", "di", "dian", "diao",
        "die", "ding", "diu", "dong", "dou", "du", "duan", "dui", "dun", "duo",
        "e", "en", "er",
        "fa", "fan", "fang", "fei", "fen", "feng", "fo", "fou", "fu",
        "ga", "gai", "gan", "gang", "gao", "ge", "gei", "gen", "geng", "gong",
        "gou", "gu", "gua", "guai", "guan", "guang", "gui", "gun", "guo",
        "ha", "hai", "han", "hang", "hao", "he", "hei", "hen", "heng", "hong",
        "hou", "hu", "hua", "huai", "huan", "huang", "hui", "hun", "huo",
        "ji", "jia", "jian", "jiang", "jiao", "jie", "jin", "jing", "jiong",
        "jiu", "ju", "juan", "jue", "jun",
        "ka", "kai", "kan", "kang", "kao", "ke", "ken", "keng", "kong", "kou",
        "ku", "kua", "kuai", "kuan", "kuang", "kui", "kun", "kuo",
        "la", "lai", "lan", "lang", "lao", "le", "lei", "leng", "li", "lian",
        "liang", "liao", "lie", "lin", "ling", "liu", "long", "lou", "lu",
        "lv", "luan", "lue", "lun", "luo",
        "ma", "mai", "man", "mang", "mao", "me", "mei", "men", "meng", "mi",
        "mian", "miao", "mie", "min", "ming", "miu", "mo", "mou", "mu",
        "na", "nai", "nan", "nang", "nao", "ne", "nei", "nen", "neng", "ni",
        "nian", "niang", "niao", "nie", "nin", "ning", "niu", "nong", "nou",
        "nu", "nv", "nuan", "nue", "nuo",
        "o", "ou",
        "pa", "pai", "pan", "pang", "pao", "pei", "pen", "peng", "pi", "pian",
        "piao", "pie", "pin", "ping", "po", "pou", "pu",
        "qi", "qia", "qian", "qiang", "qiao", "qie", "qin", "qing", "qiong",
        "qiu", "qu", "quan", "que", "qun",
        "ran", "rang", "rao", "re", "ren", "reng", "ri", "rong", "rou", "ru",
        "ruan", "rui", "run", "ruo",
        "sa", "sai", "san", "sang", "sao", "se", "sen", "seng", "sha", "shai",
        "shan", "shang", "shao", "she", "shei", "shen", "sheng", "shi", "shou",
        "shu", "shua", "shuai", "shuan", "shuang", "shui", "shun", "shuo", "si",
        "song", "sou", "su", "suan", "sui", "sun", "suo",
        "ta", "tai", "tan", "tang", "tao", "te", "teng", "ti", "tian", "tiao",
        "tie", "ting", "tong", "tou", "tu", "tuan", "tui", "tun", "tuo",
        "wa", "wai", "wan", "wang", "wei", "wen", "weng", "wo", "wu",
        "xi", "xia", "xian", "xiang", "xiao", "xie", "xin", "xing", "xiong",
        "xiu", "xu", "xuan", "xue", "xun",
        "ya", "yan", "yang", "yao", "ye", "yi", "yin", "ying", "yo", "yong",
        "you", "yu", "yuan", "yue", "yun",
        "za", "zai", "zan", "zang", "zao", "ze", "zei", "zen", "zeng", "zha",
        "zhai", "zhan", "zhang", "zhao", "zhe", "zhei", "zhen", "zheng", "zhi",
        "zhong", "zhou", "zhu", "zhua", "zhuai", "zhuan", "zhuang", "zhui",
        "zhun", "zhuo", "zi", "zong", "zou", "zu", "zuan", "zui", "zun", "zuo"
    };

    // 按长度降序排序，用于最大匹配
    std::sort(m_completeSyllables.begin(), m_completeSyllables.end(),
              [](const QString& a, const QString& b) {
                  return a.length() > b.length();
              });
}

PinyinParseResult PinyinParser::parse(const QString& input) const
{
    PinyinParseResult result;
    result.originalInput = input;

    if (input.isEmpty()) {
        result.isValid = false;
        return result;
    }

    // 检查是否全是字母
    for (QChar c : input) {
        if (!c.isLetter()) {
            result.isValid = false;
            return result;
        }
    }

    QString lowerInput = input.toLower();

    // 尝试切分音节
    result.syllables = splitSyllables(lowerInput);

    // 计算简拼
    result.jianpin = getJianpin(result.syllables);

    // 计算已完成的音节数
    result.completedSyllableCount = 0;
    for (const QString& s : result.syllables) {
        if (isCompleteSyllable(s)) {
            result.completedSyllableCount++;
        }
    }

    result.isValid = !result.syllables.isEmpty();

    return result;
}

QStringList PinyinParser::splitSyllables(const QString& input) const
{
    // MVP 版本使用简单的正向最大匹配
    return forwardMaxMatch(input);
}

bool PinyinParser::isCompleteSyllable(const QString& syllable) const
{
    return m_completeSyllables.contains(syllable);
}

QString PinyinParser::getJianpin(const QStringList& syllables) const
{
    QString jianpin;
    for (const QString& s : syllables) {
        if (!s.isEmpty()) {
            jianpin += s[0];
        }
    }
    return jianpin;
}

bool PinyinParser::isJianpinInput(const QString& input) const
{
    if (input.isEmpty()) {
        return false;
    }

    // 如果输入中没有元音字母，可能是简拼
    static const QSet<QChar> vowels = {'a', 'e', 'i', 'o', 'u', 'v'};
    for (QChar c : input.toLower()) {
        if (vowels.contains(c)) {
            return false;
        }
    }

    // 并且长度大于1
    return input.length() > 1;
}

QString PinyinParser::getFirstLetter(const QString& pinyin) const
{
    if (pinyin.isEmpty()) {
        return QString();
    }
    return pinyin.left(1).toUpper();
}

QStringList PinyinParser::forwardMaxMatch(const QString& input) const
{
    QStringList result;
    int pos = 0;
    int len = input.length();

    while (pos < len) {
        bool matched = false;

        // 从最长的可能音节开始尝试
        for (int syllableLen = qMin(6, len - pos); syllableLen >= 1; --syllableLen) {
            QString candidate = input.mid(pos, syllableLen);

            if (isCompleteSyllable(candidate)) {
                result.append(candidate);
                pos += syllableLen;
                matched = true;
                break;
            }
        }

        if (!matched) {
            // 没有匹配到完整音节，把剩下的作为一个不完整音节
            result.append(input.mid(pos));
            break;
        }
    }

    return result;
}

QStringList PinyinParser::backwardMaxMatch(const QString& input) const
{
    QStringList result;
    int pos = input.length();

    while (pos > 0) {
        bool matched = false;

        for (int syllableLen = qMin(6, pos); syllableLen >= 1; --syllableLen) {
            QString candidate = input.mid(pos - syllableLen, syllableLen);

            if (isCompleteSyllable(candidate)) {
                result.prepend(candidate);
                pos -= syllableLen;
                matched = true;
                break;
            }
        }

        if (!matched) {
            // 没有匹配到，把剩下的放前面
            result.prepend(input.left(pos));
            break;
        }
    }

    return result;
}

} // namespace uim
