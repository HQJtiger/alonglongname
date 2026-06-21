/**
 * @file pinyin_standalone.cpp
 * @brief 纯 C++ 拼音引擎实现
 */

#include "pinyin_standalone.h"

#include <sstream>
#include <cstring>
#include <cwchar>
#include <codecvt>
#include <locale>

namespace uim {

// ========================================
// 工具函数
// ========================================

static std::string toLower(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// UTF-8 转 wstring
static std::wstring utf8ToWstring(const std::string& utf8) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(utf8);
}

// wstring 转 UTF-8
static std::string wstringToUtf8(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

// ========================================
// PinyinParser 实现
// ========================================

PinyinParser::PinyinParser() {
    initSyllables();
}

PinyinParser& PinyinParser::instance() {
    static PinyinParser parser;
    return parser;
}

void PinyinParser::initSyllables() {
    // 声母列表
    m_initials = {
        "b", "p", "m", "f", "d", "t", "n", "l",
        "g", "k", "h", "j", "q", "x", "zh", "ch",
        "sh", "r", "z", "c", "s", "y", "w"
    };

    // 完整音节列表（常用的约 400 个）
    const char* syllables[] = {
        "a", "ai", "an", "ang", "ao",
        "ba", "bai", "ban", "bang", "bao", "bei", "ben", "beng", "bi", "bian", "biao", "bie", "bin", "bing", "bo", "bu",
        "ca", "cai", "can", "cang", "cao", "ce", "cen", "ceng", "cha", "chai", "chan", "chang", "chao", "che", "chen", "cheng",
        "chi", "chong", "chou", "chu", "chua", "chuai", "chuan", "chuang", "chui", "chun", "chuo", "ci", "cong", "cou", "cu",
        "cuan", "cui", "cun", "cuo",
        "da", "dai", "dan", "dang", "dao", "de", "deng", "di", "dian", "diao", "die", "ding", "diu", "dong", "dou", "du",
        "duan", "dui", "dun", "duo",
        "e", "en", "er",
        "fa", "fan", "fang", "fei", "fen", "feng", "fo", "fou", "fu",
        "ga", "gai", "gan", "gang", "gao", "ge", "gei", "gen", "geng", "gong", "gou", "gu", "gua", "guai", "guan", "guang",
        "gui", "gun", "guo",
        "ha", "hai", "han", "hang", "hao", "he", "hei", "hen", "heng", "hong", "hou", "hu", "hua", "huai", "huan", "huang",
        "hui", "hun", "huo",
        "ji", "jia", "jian", "jiang", "jiao", "jie", "jin", "jing", "jiong", "jiu", "ju", "juan", "jue", "jun",
        "ka", "kai", "kan", "kang", "kao", "ke", "ken", "keng", "kong", "kou", "ku", "kua", "kuai", "kuan", "kuang",
        "kui", "kun", "kuo",
        "la", "lai", "lan", "lang", "lao", "le", "lei", "leng", "li", "lia", "lian", "liang", "liao", "lie", "lin", "ling",
        "liu", "long", "lou", "lu", "lv", "luan", "lve", "lun", "luo",
        "ma", "mai", "man", "mang", "mao", "me", "mei", "men", "meng", "mi", "mian", "miao", "mie", "min", "ming", "miu",
        "mo", "mou", "mu",
        "na", "nai", "nan", "nang", "nao", "ne", "nei", "nen", "neng", "ni", "nian", "niang", "niao", "nie", "nin", "ning",
        "niu", "nong", "nu", "nv", "nuan", "nve", "nuo",
        "o", "ou",
        "pa", "pai", "pan", "pang", "pao", "pei", "pen", "peng", "pi", "pian", "piao", "pie", "pin", "ping", "po", "pou", "pu",
        "qi", "qia", "qian", "qiang", "qiao", "qie", "qin", "qing", "qiong", "qiu", "qu", "quan", "que", "qun",
        "ran", "rang", "rao", "re", "ren", "reng", "ri", "rong", "rou", "ru", "ruan", "rui", "run", "ruo",
        "sa", "sai", "san", "sang", "sao", "se", "sen", "seng", "sha", "shai", "shan", "shang", "shao", "she", "shei", "shen",
        "sheng", "shi", "shou", "shu", "shua", "shuai", "shuan", "shuang", "shui", "shun", "shuo", "si", "song", "sou", "su",
        "suan", "sui", "sun", "suo",
        "ta", "tai", "tan", "tang", "tao", "te", "teng", "ti", "tian", "tiao", "tie", "ting", "tong", "tou", "tu",
        "tuan", "tui", "tun", "tuo",
        "wa", "wai", "wan", "wang", "wei", "wen", "weng", "wo", "wu",
        "xi", "xia", "xian", "xiang", "xiao", "xie", "xin", "xing", "xiong", "xiu", "xu", "xuan", "xue", "xun",
        "ya", "yan", "yang", "yao", "ye", "yi", "yin", "ying", "yo", "yong", "you", "yu", "yuan", "yue", "yun",
        "za", "zai", "zan", "zang", "zao", "ze", "zei", "zen", "zeng", "zha", "zhai", "zhan", "zhang", "zhao", "zhe", "zhei",
        "zhen", "zheng", "zhi", "zhong", "zhou", "zhu", "zhua", "zhuai", "zhuan", "zhuang", "zhui", "zhun", "zhuo", "zi",
        "zong", "zou", "zu", "zuan", "zui", "zun", "zuo",
        nullptr
    };

    for (int i = 0; syllables[i] != nullptr; i++) {
        m_allSyllables.push_back(syllables[i]);
    }

    // 按长度排序，长的在前（用于最大匹配）
    std::sort(m_allSyllables.begin(), m_allSyllables.end(),
              [](const std::string& a, const std::string& b) {
                  return a.length() > b.length();
              });
}

bool PinyinParser::isSyllable(const std::string& s) const {
    for (const auto& syllable : m_allSyllables) {
        if (syllable == s) {
            return true;
        }
    }
    return false;
}

bool PinyinParser::isCompleteSyllable(const std::string& syllable) const {
    return isSyllable(syllable);
}

bool PinyinParser::isJianpinInput(const std::string& input) const {
    if (input.empty()) return false;

    // 简拼：所有字符都是声母（没有元音字母）
    std::string vowels = "aeiouv";
    for (char c : input) {
        if (vowels.find(c) != std::string::npos) {
            return false;
        }
    }
    return true;
}

PinyinParseResult PinyinParser::parse(const std::string& input) const {
    PinyinParseResult result;
    result.rawInput = input;
    result.isValid = false;
    result.isJianpin = false;
    result.completedSyllables = 0;

    if (input.empty()) {
        return result;
    }

    std::string lowerInput = toLower(input);

    // 检查是否是简拼
    if (isJianpinInput(lowerInput)) {
        result.isJianpin = true;
        result.isValid = true;
        for (char c : lowerInput) {
            result.syllables.push_back(std::string(1, c));
        }
        result.completedSyllables = result.syllables.size();
        return result;
    }

    // 正向最大匹配算法
    int pos = 0;
    int len = lowerInput.length();

    while (pos < len) {
        bool found = false;

        // 从最长的可能音节开始尝试
        for (int i = std::min(6, len - pos); i >= 1; i--) {
            std::string sub = lowerInput.substr(pos, i);
            if (isSyllable(sub)) {
                result.syllables.push_back(sub);
                pos += i;
                result.completedSyllables++;
                found = true;
                break;
            }
        }

        if (!found) {
            // 剩下的字符不构成完整音节
            if (pos < len) {
                std::string remaining = lowerInput.substr(pos);
                // 检查是否是某个音节的前缀
                bool isPrefix = false;
                for (const auto& syllable : m_allSyllables) {
                    if (syllable.length() > remaining.length() &&
                        syllable.substr(0, remaining.length()) == remaining) {
                        isPrefix = true;
                        break;
                    }
                }
                if (isPrefix) {
                    result.syllables.push_back(remaining);
                    result.isValid = true;
                }
            }
            break;
        }
    }

    if (!result.syllables.empty()) {
        result.isValid = true;
    }

    return result;
}

// ========================================
// Dictionary 实现
// ========================================

Dictionary::Dictionary() : m_initialized(false) {
}

Dictionary& Dictionary::instance() {
    static Dictionary dict;
    return dict;
}

void Dictionary::initialize() {
    if (m_initialized) return;

    // 内置简化词库
    // 格式: { 拼音, 词, 词频 }
    struct WordEntry {
        const char* pinyin;
        const char* word;
        int frequency;
    };

    WordEntry words[] = {
        // 高频单字
        {"de", "的", 10000},
        {"yi", "一", 9000},
        {"shi", "是", 8500},
        {"bu", "不", 8000},
        {"le", "了", 7500},
        {"zai", "在", 7000},
        {"ren", "人", 6500},
        {"you", "有", 6000},
        {"wo", "我", 5500},
        {"ni", "你", 5400},
        {"ta", "他", 5000},
        {"ta", "她", 4900},
        {"ta", "它", 4800},
        {"zhe", "这", 4700},
        {"na", "那", 4600},
        {"he", "和", 4500},
        {"jiu", "就", 4400},
        {"ye", "也", 4300},
        {"dao", "到", 4200},
        {"shuo", "说", 4100},
        {"yao", "要", 4000},
        {"ke", "可", 3900},
        {"qu", "去", 3800},
        {"lai", "来", 3700},
        {"yi", "以", 3600},
        {"hui", "会", 3500},
        {"zuo", "做", 3400},
        {"xiang", "想", 3300},
        {"kan", "看", 3200},
        {"ting", "听", 3100},
        {"shuo", "说", 3000},
        {"zhi", "知", 2900},
        {"dao", "道", 2800},
        {"nian", "年", 2700},
        {"yue", "月", 2600},
        {"ri", "日", 2500},
        {"shi", "时", 2400},
        {"fen", "分", 2300},
        {"zhong", "中", 2200},
        {"guo", "国", 2100},
        {"jia", "家", 2000},
        {"xue", "学", 1900},
        {"gong", "工", 1800},
        {"gong", "公", 1700},
        {"hao", "好", 1600},
        {"wei", "为", 1500},
        {"zi", "子", 1400},
        {"you", "又", 1300},
        {"dan", "但", 1200},
        {"er", "而", 1100},
        {"qi", "其", 1000},
        {"huo", "或", 900},
        {"wu", "无", 800},
        {"yu", "与", 700},
        {"ji", "及", 600},
        {"deng", "等", 500},
        {"ba", "把", 490},
        {"jiang", "将", 480},
        {"dui", "对", 470},
        {"sheng", "生", 460},
        {"fa", "发", 450},
        {"cheng", "成", 440},
        {"jian", "见", 430},
        {"wen", "问", 420},
        {"chu", "出", 410},
        {"li", "里", 400},
        {"mian", "面", 390},
        {"shou", "手", 380},
        {"shui", "水", 370},
        {"huo", "火", 360},
        {"shan", "山", 350},
        {"shi", "石", 340},
        {"tian", "天", 330},
        {"di", "地", 320},
        {"ren", "认", 310},
        {"wei", "位", 300},
        {"ming", "名", 290},
        {"fa", "法", 280},
        {"shu", "书", 270},
        {"hua", "话", 260},
        {"ci", "次", 250},
        {"zheng", "正", 240},
        {"xin", "心", 230},
        {"qing", "情", 220},
        {"peng", "朋", 210},
        {"you", "友", 200},
        {"ai", "爱", 190},
        {"hen", "恨", 180},
        {"gao", "高", 170},
        {"di", "低", 160},
        {"da", "大", 150},
        {"xiao", "小", 140},
        {"duo", "多", 130},
        {"shao", "少", 120},
        {"chang", "长", 110},
        {"duan", "短", 100},
        {"kuai", "快", 90},
        {"man", "慢", 80},
        {"xin", "新", 70},
        {"jiu", "旧", 60},
        {"mei", "美", 50},
        {"zhen", "真", 40},
        {"jia", "假", 30},
        {"cuo", "错", 20},
        {"neng", "能", 1000},
        {"keyi", "可以", 800},
        {"buxing", "不行", 700},
        {"meiyou", "没有", 600},
        {"haode", "好的", 500},
        {"xiexie", "谢谢", 400},
        {"bukeqi", "不客气", 300},
        {"duibuqi", "对不起", 200},
        {"meiguanxi", "没关系", 100},
        {"zaijian", "再见", 50},
        {"nihao", "你好", 500},

        // 常用双字词
        {"women", "我们", 5000},
        {"nimen", "你们", 4000},
        {"tamen", "他们", 3900},
        {"tamen", "她们", 3800},
        {"dajia", "大家", 3700},
        {"zij", "自己", 3600},
        {"pengyou", "朋友", 3500},
        {"tongshi", "同事", 3400},
        {"tongxue", "同学", 3300},
        {"laoshi", "老师", 3200},
        {"xuesheng", "学生", 3100},
        {"yisheng", "医生", 3000},
        {"gognsi", "公司", 2900},
        {"xuexiao", "学校", 2800},
        {"yiyuan", "医院", 2700},
        {"shouji", "手机", 2600},
        {"diannao", "电脑", 2500},
        {"jianpan", "键盘", 2400},
        {"shubiao", "鼠标", 2300},
        {"ruanjian", "软件", 2200},
        {"yingjian", "硬件", 2100},
        {"xitong", "系统", 2000},
        {"chengxu", "程序", 1900},
        {"daima", "代码", 1800},
        {"wangluo", "网络", 1700},
        {"hulianwang", "互联网", 1600},
        {"zhidao", "知道", 1500},
        {"mingbai", "明白", 1400},
        {"liaojie", "了解", 1300},
        {"renshi", "认识", 1200},
        {"jide", "记得", 1100},
        {"wangji", "忘记", 1000},
        {"xiwang", "希望", 900},
        {"xihuan", "喜欢", 800},
        {"ai", "爱", 700},
        {"hen", "恨", 600},
        {"gaoxing", "高兴", 500},
        {"nanguo", "难过", 490},
        {"shengqi", "生气", 480},
        {"haipa", "害怕", 470},
        {"jindong", "激动", 460},
        {"pingjing", "平静", 450},

        // 时间相关
        {"jintian", "今天", 4000},
        {"zuotian", "昨天", 3900},
        {"mingtian", "明天", 3800},
        {"shangwu", "上午", 3700},
        {"xiawu", "下午", 3600},
        {"wanshang", "晚上", 3500},
        {"zaoshang", "早上", 3400},
        {"zhongwu", "中午", 3300},
        {"xianzai", "现在", 3200},
        {"yiqian", "以前", 3100},
        {"yihou", "以后", 3000},
        {"gangcai", "刚才", 2900},
        {"mashang", "马上", 2800},
        {"dengyi", "等一", 2700},

        // 地点
        {"zhongguo", "中国", 5000},
        {"beijing", "北京", 4000},
        {"shanghai", "上海", 3900},
        {"shenzhen", "深圳", 3800},
        {"guangzhou", "广州", 3700},
        {"hangzhou", "杭州", 3600},
        {"chengdu", "成都", 3500},
        {"nanjing", "南京", 3400},
        {"wuhan", "武汉", 3300},
        {"xian", "西安", 3200},

        // 计算机相关
        {"ruanjian", "软件", 2000},
        {"yingjian", "硬件", 1500},
        {"xitong", "系统", 2500},
        {"chengxu", "程序", 2200},
        {"daima", "代码", 2000},
        {"shurufa", "输入法", 1800},
        {"pinyin", "拼音", 1700},
        {"hanzi", "汉字", 1600},
        {"wenjian", "文件", 1500},
        {"wenjianjia", "文件夹", 1400},
        {"chuangkou", "窗口", 1300},
        {"caidan", "菜单", 1200},
        {"anniu", "按钮", 1100},
        {"tubiao", "图标", 1000},
        {"zhuti", "主题", 900},
        {"shezhi", "设置", 800},
        {"peizhi", "配置", 700},
        {"ceshi", "测试", 600},
        {"bianyi", "编译", 500},
        {"yunxing", "运行", 490},
        {"tiaoshi", "调试", 480},

        // 其他常用词
        {"wenti", "问题", 3000},
        {"daan", "答案", 2900},
        {"fangfa", "方法", 2800},
        {"fangshi", "方式", 2700},
        {"jieguo", "结果", 2600},
        {"yuanyin", "原因", 2500},
        {"mu", "目", 2400},
        {"biao", "标", 2300},
        {"jihua", "计划", 2200},
        {"anpai", "安排", 2100},
        {"gongzuo", "工作", 2000},
        {"shenghuo", "生活", 1900},
        {"xuexi", "学习", 1800},
        {"lianxi", "联系", 1700},
        {"bangzhu", "帮助", 1600},
        {"xiexie", "谢谢", 1500},
        {"bukeqi", "不客气", 1400},
        {"duibuqi", "对不起", 1300},
        {"meiguanxi", "没关系", 1200},
        {"zaijian", "再见", 1100},
        {"nihao", "你好", 1000},
        {"zaoshanghao", "早上好", 900},
        {"wanshanghao", "晚上好", 800},
        {"chifanlema", "吃饭了吗", 700},
        {"huanying", "欢迎", 600},
        {"qing", "请", 500},
        {"hao", "好", 4000},
        {"huai", "坏", 300},
        {"da", "大", 3000},
        {"xiao", "小", 2900},
        {"duo", "多", 2800},
        {"shao", "少", 2700},
        {"chang", "长", 2600},
        {"duan", "短", 2500},
        {"gao", "高", 2400},
        {"di", "低", 2300},
        {"kuai", "快", 2200},
        {"man", "慢", 2100},
        {"xin", "新", 2000},
        {"jiu", "旧", 1900},
        {"mei", "美", 1800},
        {"chou", "丑", 1700},
        {"zhen", "真", 1600},
        {"jia", "假", 1500},
        {"dui", "对", 1400},
        {"cuo", "错", 1300},
        {"shi", "是", 1200},
        {"fou", "否", 1100},
        {"neng", "能", 1000},
        {"buneng", "不能", 900},
        {"keyi", "可以", 800},
        {"buxing", "不行", 700},
        {"haode", "好的", 600},
        {"meiyou", "没有", 500},
        {"youde", "有的", 490},
        {"suoyou", "所有", 480},
        {"meige", "每个", 470},
        {"yixie", "一些", 460},
        {"henduo", "很多", 450},
        {"yidian", "一点", 440},
        {"feichang", "非常", 430},
        {"bijiao", "比较", 420},
        {"tai", "太", 410},
        {"zui", "最", 400},
        {"geng", "更", 390},
        {"xiangdang", "相当", 380},
        {"tebie", "特别", 370},
        {"yiban", "一般", 360},
        {"putong", "普通", 350},
        {"jianyi", "建议", 340},
        {"yijian", "意见", 330},
        {"xiangfa", "想法", 320},
        {"guandian", "观点", 310},
        {"kanfa", "看法", 300},
        {nullptr, nullptr, 0}
    };

    // 构建索引
    for (int i = 0; words[i].pinyin != nullptr; i++) {
        Candidate c;
        c.text = utf8ToWstring(words[i].word);
        c.pinyin = words[i].pinyin;
        c.frequency = words[i].frequency;
        c.index = 0;

        m_pinyinIndex[words[i].pinyin].push_back(c);

        // 构建简拼索引
        std::string jianpin;
        bool first = true;
        PinyinParseResult result = PinyinParser::instance().parse(words[i].pinyin);
        for (const auto& syl : result.syllables) {
            if (!syl.empty()) {
                jianpin += syl[0];
            }
        }
        if (!jianpin.empty() && jianpin.length() > 1) {
            m_jianpinIndex[jianpin].push_back(c);
        }
    }

    // 按词频排序
    for (auto& pair : m_pinyinIndex) {
        std::sort(pair.second.begin(), pair.second.end(),
                  [](const Candidate& a, const Candidate& b) {
                      return a.frequency > b.frequency;
                  });
    }

    for (auto& pair : m_jianpinIndex) {
        std::sort(pair.second.begin(), pair.second.end(),
                  [](const Candidate& a, const Candidate& b) {
                      return a.frequency > b.frequency;
                  });
    }

    m_initialized = true;
}

CandidateList Dictionary::lookup(const std::string& pinyin) const {
    CandidateList result;

    // 先查全拼
    auto exact = lookupExact(pinyin);
    result.insert(result.end(), exact.begin(), exact.end());

    // 再查简拼
    if (PinyinParser::instance().isJianpinInput(pinyin)) {
        auto jianpin = lookupJianpin(pinyin);
        result.insert(result.end(), jianpin.begin(), jianpin.end());
    }

    return result;
}

CandidateList Dictionary::lookupExact(const std::string& pinyin) const {
    auto it = m_pinyinIndex.find(pinyin);
    if (it != m_pinyinIndex.end()) {
        return it->second;
    }
    return {};
}

CandidateList Dictionary::lookupJianpin(const std::string& jianpin) const {
    auto it = m_jianpinIndex.find(jianpin);
    if (it != m_jianpinIndex.end()) {
        return it->second;
    }
    return {};
}

void Dictionary::incrementFrequency(const std::wstring& text) {
    // 简化实现：实际应该遍历所有索引
    // MVP 版本暂不实现完整的词频学习
}

void Dictionary::addUserWord(const std::wstring& text, const std::string& pinyin, int frequency) {
    Candidate c;
    c.text = text;
    c.pinyin = pinyin;
    c.frequency = frequency;
    c.index = 0;

    m_pinyinIndex[pinyin].push_back(c);

    // 重新排序
    std::sort(m_pinyinIndex[pinyin].begin(), m_pinyinIndex[pinyin].end(),
              [](const Candidate& a, const Candidate& b) {
                  return a.frequency > b.frequency;
              });
}

// ========================================
// PinyinEngine 实现
// ========================================

PinyinEngine::PinyinEngine()
    : m_currentPageNum(0)
    , m_totalPages(0)
{
    Dictionary::instance().initialize();
}

bool PinyinEngine::processKey(char key) {
    if (!isalpha(key)) {
        return false;
    }

    m_input += tolower(key);
    updateCandidates();
    return true;
}

bool PinyinEngine::processBackspace() {
    if (m_input.empty()) {
        return false;
    }

    m_input.pop_back();
    updateCandidates();
    return true;
}

std::wstring PinyinEngine::processSpace() {
    if (m_allCandidates.empty()) {
        return L"";
    }

    std::wstring result = m_allCandidates[0].text;
    Dictionary::instance().incrementFrequency(result);
    m_input.clear();
    m_allCandidates.clear();
    m_currentPage.clear();
    m_currentPageNum = 0;
    m_totalPages = 0;

    return result;
}

std::string PinyinEngine::processEnter() {
    std::string result = m_input;
    m_input.clear();
    m_allCandidates.clear();
    m_currentPage.clear();
    m_currentPageNum = 0;
    m_totalPages = 0;
    return result;
}

std::wstring PinyinEngine::processNumber(int num) {
    if (num < 1 || num > 9) {
        return L"";
    }

    int index = m_currentPageNum * candidatesPerPage + (num - 1);
    if (index >= (int)m_allCandidates.size()) {
        return L"";
    }

    std::wstring result = m_allCandidates[index].text;
    Dictionary::instance().incrementFrequency(result);
    m_input.clear();
    m_allCandidates.clear();
    m_currentPage.clear();
    m_currentPageNum = 0;
    m_totalPages = 0;

    return result;
}

bool PinyinEngine::processPage(bool forward) {
    if (m_totalPages <= 1) {
        return false;
    }

    if (forward) {
        if (m_currentPageNum < m_totalPages - 1) {
            m_currentPageNum++;
            updatePage();
            return true;
        }
    } else {
        if (m_currentPageNum > 0) {
            m_currentPageNum--;
            updatePage();
            return true;
        }
    }

    return false;
}

void PinyinEngine::processEscape() {
    m_input.clear();
    m_allCandidates.clear();
    m_currentPage.clear();
    m_currentPageNum = 0;
    m_totalPages = 0;
}

void PinyinEngine::updateCandidates() {
    m_allCandidates.clear();

    if (m_input.empty()) {
        m_currentPage.clear();
        m_currentPageNum = 0;
        m_totalPages = 0;
        return;
    }

    // 查询词库
    m_allCandidates = Dictionary::instance().lookup(m_input);

    // 计算页数
    if (m_allCandidates.empty()) {
        m_totalPages = 0;
    } else {
        m_totalPages = (m_allCandidates.size() + candidatesPerPage - 1) / candidatesPerPage;
    }

    m_currentPageNum = 0;
    updatePage();
}

void PinyinEngine::updatePage() {
    m_currentPage.clear();

    if (m_allCandidates.empty()) {
        return;
    }

    int start = m_currentPageNum * candidatesPerPage;
    int end = std::min(start + candidatesPerPage, (int)m_allCandidates.size());

    for (int i = start; i < end; i++) {
        Candidate c = m_allCandidates[i];
        c.index = i + 1;
        m_currentPage.push_back(c);
    }
}

} // namespace uim
