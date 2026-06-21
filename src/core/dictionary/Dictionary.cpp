#include "Dictionary.h"
#include "common/Logger.h"

#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <algorithm>

namespace uim {

// 简化版内置词库（常用词，约 500 个）
// 格式：词语 拼音 词频
static const struct {
    const char* word;
    const char* pinyin;
    int frequency;
} g_builtinWords[] = {
    // 单字 - 高频
    {"的", "de", 10000},
    {"一", "yi", 9000},
    {"是", "shi", 8500},
    {"不", "bu", 8000},
    {"了", "le", 7800},
    {"在", "zai", 7500},
    {"人", "ren", 7000},
    {"有", "you", 6800},
    {"我", "wo", 6500},
    {"他", "ta", 6200},
    {"她", "ta", 6000},
    {"它", "ta", 5000},
    {"这", "zhe", 5800},
    {"那", "na", 5500},
    {"个", "ge", 5200},
    {"中", "zhong", 5000},
    {"大", "da", 4800},
    {"小", "xiao", 4600},
    {"上", "shang", 4500},
    {"下", "xia", 4400},
    {"来", "lai", 4300},
    {"去", "qu", 4200},
    {"说", "shuo", 4100},
    {"看", "kan", 4000},
    {"听", "ting", 3900},
    {"想", "xiang", 3800},
    {"做", "zuo", 3700},
    {"要", "yao", 3600},
    {"会", "hui", 3500},
    {"能", "neng", 3400},
    {"可以", "ke yi", 3300},
    {"好", "hao", 3200},
    {"对", "dui", 3100},
    {"错", "cuo", 3000},
    {"和", "he", 2900},
    {"与", "yu", 2800},
    {"及", "ji", 2700},
    {"等", "deng", 2600},
    {"就", "jiu", 2500},
    {"都", "dou", 2400},
    {"也", "ye", 2300},
    {"还", "hai", 2200},
    {"又", "you", 2100},
    {"再", "zai", 2000},
    {"才", "cai", 1900},
    {"已", "yi", 1800},
    {"已经", "yi jing", 1700},
    {"没", "mei", 1600},
    {"没有", "mei you", 1500},
    {"什么", "shen me", 1400},
    {"怎么", "zen me", 1300},
    {"为什么", "wei shen me", 1200},
    {"因为", "yin wei", 1100},
    {"所以", "suo yi", 1000},
    {"但是", "dan shi", 950},
    {"如果", "ru guo", 900},
    {"虽然", "sui ran", 850},
    {"而且", "er qie", 800},
    {"然后", "ran hou", 750},
    {"终于", "zhong yu", 700},
    {"其实", "qi shi", 650},
    {"可能", "ke neng", 600},
    {"应该", "ying gai", 550},
    {"必须", "bi xu", 500},
    {"需要", "xu yao", 450},

    // 双字词 - 常用
    {"我们", "wo men", 3000},
    {"你们", "ni men", 2800},
    {"他们", "ta men", 2700},
    {"她们", "ta men", 2500},
    {"大家", "da jia", 2400},
    {"自己", "zi ji", 2300},
    {"朋友", "peng you", 2200},
    {"同事", "tong shi", 2100},
    {"同学", "tong xue", 2000},
    {"老师", "lao shi", 1900},
    {"学生", "xue sheng", 1800},
    {"医生", "yi sheng", 1700},
    {"工程师", "gong cheng shi", 1600},
    {"经理", "jing li", 1500},
    {"老板", "lao ban", 1400},
    {"客户", "ke hu", 1300},
    {"用户", "yong hu", 1200},
    {"产品", "chan pin", 1100},
    {"项目", "xiang mu", 1000},
    {"工作", "gong zuo", 950},
    {"学习", "xue xi", 900},
    {"生活", "sheng huo", 850},
    {"时间", "shi jian", 800},
    {"今天", "jin tian", 750},
    {"明天", "ming tian", 700},
    {"昨天", "zuo tian", 650},
    {"现在", "xian zai", 600},
    {"以前", "yi qian", 550},
    {"以后", "yi hou", 500},
    {"早上", "zao shang", 450},
    {"晚上", "wan shang", 400},
    {"下午", "xia wu", 350},
    {"上午", "shang wu", 300},
    {"星期", "xing qi", 250},
    {"周一", "zhou yi", 200},
    {"周末", "zhou mo", 150},

    // 常用动词
    {"知道", "zhi dao", 2000},
    {"明白", "ming bai", 1900},
    {"了解", "liao jie", 1800},
    {"认识", "ren shi", 1700},
    {"记得", "ji de", 1600},
    {"忘记", "wang ji", 1500},
    {"希望", "xi wang", 1400},
    {"喜欢", "xi huan", 1300},
    {"讨厌", "tao yan", 1200},
    {"高兴", "gao xing", 1100},
    {"开心", "kai xin", 1000},
    {"难过", "nan guo", 950},
    {"生气", "sheng qi", 900},
    {"担心", "dan xin", 850},
    {"害怕", "hai pa", 800},
    {"努力", "nu li", 750},
    {"加油", "jia you", 700},
    {"成功", "cheng gong", 650},
    {"失败", "shi bai", 600},
    {"开始", "kai shi", 550},
    {"结束", "jie shu", 500},
    {"继续", "ji xu", 450},
    {"停止", "ting zhi", 400},
    {"完成", "wan cheng", 350},
    {"开始", "kai shi", 300},

    // 计算机相关
    {"电脑", "dian nao", 1500},
    {"手机", "shou ji", 1400},
    {"键盘", "jian pan", 1300},
    {"鼠标", "shu biao", 1200},
    {"显示器", "xian shi qi", 1100},
    {"软件", "ruan jian", 1000},
    {"硬件", "ying jian", 950},
    {"系统", "xi tong", 900},
    {"程序", "cheng xu", 850},
    {"代码", "dai ma", 800},
    {"编程", "bian cheng", 750},
    {"开发", "kai fa", 700},
    {"测试", "ce shi", 650},
    {"数据库", "shu ju ku", 600},
    {"网络", "wang luo", 550},
    {"互联网", "hu lian wang", 500},
    {"浏览器", "liu lan qi", 450},
    {"下载", "xia zai", 400},
    {"上传", "shang chuan", 350},
    {"文件", "wen jian", 300},
    {"文件夹", "wen jian jia", 250},
    {"安装", "an zhuang", 200},
    {"卸载", "xie zai", 150},
    {"设置", "she zhi", 100},
    {"配置", "pei zhi", 90},
    {"输入法", "shu ru fa", 80},
    {"拼音", "pin yin", 70},
    {"候选词", "hou xuan ci", 60},
    {"剪贴板", "jian tie ban", 50},
    {"同步", "tong bu", 40},

    // 常用名词
    {"中国", "zhong guo", 2000},
    {"北京", "bei jing", 1900},
    {"上海", "shang hai", 1800},
    {"深圳", "shen zhen", 1700},
    {"广州", "guang zhou", 1600},
    {"杭州", "hang zhou", 1500},
    {"公司", "gong si", 1400},
    {"学校", "xue xiao", 1300},
    {"医院", "yi yuan", 1200},
    {"银行", "yin hang", 1100},
    {"超市", "chao shi", 1000},
    {"商店", "shang dian", 950},
    {"饭店", "fan dian", 900},
    {"酒店", "jiu dian", 850},
    {"公园", "gong yuan", 800},
    {"车站", "che zhan", 750},
    {"机场", "ji chang", 700},
    {"地铁", "di tie", 650},
    {"公交", "gong jiao", 600},
    {"汽车", "qi che", 550},
    {"火车", "huo che", 500},
    {"飞机", "fei ji", 450},
    {"自行车", "zi xing che", 400},
    {"电动车", "dian dong che", 350},

    // 数字相关
    {"零", "ling", 500},
    {"一", "yi", 1000},
    {"二", "er", 900},
    {"三", "san", 850},
    {"四", "si", 800},
    {"五", "wu", 750},
    {"六", "liu", 700},
    {"七", "qi", 650},
    {"八", "ba", 600},
    {"九", "jiu", 550},
    {"十", "shi", 500},
    {"百", "bai", 450},
    {"千", "qian", 400},
    {"万", "wan", 350},
    {"亿", "yi", 300},

    // 更多常用词
    {"问题", "wen ti", 1000},
    {"答案", "da an", 900},
    {"方法", "fang fa", 850},
    {"方式", "fang shi", 800},
    {"结果", "jie guo", 750},
    {"原因", "yuan yin", 700},
    {"目的", "mu di", 650},
    {"目标", "mu biao", 600},
    {"计划", "ji hua", 550},
    {"方案", "fang an", 500},
    {"建议", "jian yi", 450},
    {"意见", "yi jian", 400},
    {"想法", "xiang fa", 350},
    {"意思", "yi si", 300},
    {"意义", "yi yi", 250},
    {"价值", "jia zhi", 200},
    {"价格", "jia ge", 150},
    {"质量", "zhi liang", 100},
    {"数量", "shu liang", 90},
    {"速度", "su du", 80},
    {"效率", "xiao lv", 70},
    {"效果", "xiao guo", 60},
    {"功能", "gong neng", 50},
    {"性能", "xing neng", 40},
    {"安全", "an quan", 30},
    {"隐私", "yin si", 20},
};

static const int g_builtinWordCount = sizeof(g_builtinWords) / sizeof(g_builtinWords[0]);

Dictionary& Dictionary::instance()
{
    static Dictionary instance;
    return instance;
}

Dictionary::Dictionary()
    : m_loaded(false)
{
}

Dictionary::~Dictionary() = default;

bool Dictionary::load()
{
    if (m_loaded) {
        return true;
    }

    // MVP 阶段默认加载内置词库
    return loadBuiltin();
}

bool Dictionary::loadFromFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        UIM_LOG_ERROR(QString("Failed to open dictionary file: %1").arg(filePath));
        return false;
    }

    m_words.clear();
    m_pinyinIndex.clear();
    m_jianpinIndex.clear();

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith("#")) {
            continue;
        }

        QStringList parts = line.split('\t');
        if (parts.size() >= 2) {
            QString word = parts[0];
            QString pinyin = parts[1];
            int frequency = parts.size() > 2 ? parts[2].toInt() : 100;
            m_words.append(DictionaryEntry(word, pinyin, frequency));
        }
    }

    file.close();
    buildIndex();
    m_loaded = true;

    UIM_LOG_INFO(QString("Dictionary loaded: %1 words from %2").arg(m_words.size()).arg(filePath));
    return true;
}

bool Dictionary::loadBuiltin()
{
    m_words.clear();
    m_pinyinIndex.clear();
    m_jianpinIndex.clear();

    for (int i = 0; i < g_builtinWordCount; ++i) {
        m_words.append(DictionaryEntry(
            QString::fromUtf8(g_builtinWords[i].word),
            QString::fromUtf8(g_builtinWords[i].pinyin),
            g_builtinWords[i].frequency
        ));
    }

    buildIndex();
    m_loaded = true;

    UIM_LOG_INFO(QString("Builtin dictionary loaded: %1 words").arg(m_words.size()));
    return true;
}

void Dictionary::buildIndex()
{
    m_pinyinIndex.clear();
    m_jianpinIndex.clear();

    for (int i = 0; i < m_words.size(); ++i) {
        const auto& entry = m_words[i];

        // 全拼索引（去掉空格）
        QString pinyinKey = entry.pinyin.remove(' ');
        m_pinyinIndex[pinyinKey].append(i);

        // 简拼索引
        QString jianpin = getJianpin(entry.pinyin);
        if (!jianpin.isEmpty()) {
            m_jianpinIndex[jianpin].append(i);
        }
    }
}

QString Dictionary::getJianpin(const QString& pinyin) const
{
    QStringList syllables = pinyin.split(' ', Qt::SkipEmptyParts);
    QString jianpin;
    for (const QString& s : syllables) {
        if (!s.isEmpty()) {
            jianpin += s[0];
        }
    }
    return jianpin;
}

CandidateList Dictionary::lookup(const QString& pinyin, int maxCount) const
{
    if (!m_loaded) {
        return {};
    }

    // 先尝试全拼匹配
    CandidateList result = lookupExact(pinyin, maxCount);

    // 如果全拼结果不够，尝试简拼匹配
    if (result.size() < maxCount) {
        CandidateList jianpinResult = lookupJianpin(pinyin, maxCount - result.size());
        for (const auto& c : jianpinResult) {
            // 避免重复
            bool exists = false;
            for (const auto& r : result) {
                if (r.text == c.text) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                result.append(c);
            }
        }
    }

    // 限制数量
    if (result.size() > maxCount) {
        result = result.mid(0, maxCount);
    }

    // 标注序号
    for (int i = 0; i < result.size(); ++i) {
        result[i].index = i + 1;
    }

    return result;
}

CandidateList Dictionary::lookupExact(const QString& pinyin, int maxCount) const
{
    if (!m_loaded) {
        return {};
    }

    QString key = pinyin.toLower();
    auto it = m_pinyinIndex.find(key);
    if (it == m_pinyinIndex.end()) {
        return {};
    }

    // 获取所有匹配的词条，按词频排序
    QVector<DictionaryEntry> matches;
    for (int idx : it.value()) {
        matches.append(m_words[idx]);
    }

    // 按词频降序排序
    std::sort(matches.begin(), matches.end(), [](const DictionaryEntry& a, const DictionaryEntry& b) {
        return a.frequency > b.frequency;
    });

    // 转换为候选词列表
    CandidateList result;
    int count = qMin(maxCount, matches.size());
    for (int i = 0; i < count; ++i) {
        Candidate c;
        c.text = matches[i].word;
        c.pinyin = matches[i].pinyin;
        c.frequency = matches[i].frequency;
        c.index = i + 1;
        result.append(c);
    }

    return result;
}

CandidateList Dictionary::lookupJianpin(const QString& jianpin, int maxCount) const
{
    if (!m_loaded) {
        return {};
    }

    QString key = jianpin.toLower();
    auto it = m_jianpinIndex.find(key);
    if (it == m_jianpinIndex.end()) {
        return {};
    }

    // 获取所有匹配的词条，按词频排序
    QVector<DictionaryEntry> matches;
    for (int idx : it.value()) {
        matches.append(m_words[idx]);
    }

    // 按词频降序排序
    std::sort(matches.begin(), matches.end(), [](const DictionaryEntry& a, const DictionaryEntry& b) {
        return a.frequency > b.frequency;
    });

    // 转换为候选词列表
    CandidateList result;
    int count = qMin(maxCount, matches.size());
    for (int i = 0; i < count; ++i) {
        Candidate c;
        c.text = matches[i].word;
        c.pinyin = matches[i].pinyin;
        c.frequency = matches[i].frequency;
        c.index = i + 1;
        result.append(c);
    }

    return result;
}

int Dictionary::totalWords() const
{
    return m_words.size();
}

bool Dictionary::isLoaded() const
{
    return m_loaded;
}

void Dictionary::addUserWord(const QString& word, const QString& pinyin, int frequency)
{
    DictionaryEntry entry(word, pinyin, frequency);
    m_words.append(entry);

    // 更新索引
    int idx = m_words.size() - 1;
    QString pinyinKey = pinyin.remove(' ');
    m_pinyinIndex[pinyinKey].append(idx);

    QString jianpin = getJianpin(pinyin);
    if (!jianpin.isEmpty()) {
        m_jianpinIndex[jianpin].append(idx);
    }
}

void Dictionary::incrementFrequency(const QString& word, int delta)
{
    for (auto& entry : m_words) {
        if (entry.word == word) {
            entry.frequency += delta;
            break;
        }
    }
    m_wordFrequency[word] += delta;
}

} // namespace uim
