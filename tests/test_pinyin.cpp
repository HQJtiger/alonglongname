/**
 * @file test_pinyin.cpp
 * @brief 拼音引擎单元测试
 *
 * 使用 Qt Test 框架进行单元测试。
 */

#include <QTest>
#include <QObject>

#include "core/pinyin/PinyinEngine.h"
#include "core/pinyin/PinyinParser.h"
#include "core/dictionary/Dictionary.h"
#include "core/common/Logger.h"

using namespace uim;

class TestPinyin : public QObject {
    Q_OBJECT

private slots:
    // 初始化
    void initTestCase();
    void cleanupTestCase();

    // 拼音解析器测试
    void testParserSingleSyllable();
    void testParserMultipleSyllables();
    void testParserJianpin();
    void testParserInvalidInput();

    // 词库测试
    void testDictionaryLookup();
    void testDictionaryLookupExact();
    void testDictionaryLookupJianpin();
    void testDictionaryAddWord();

    // 拼音引擎测试
    void testEngineInput();
    void testEngineBackspace();
    void testEngineSpaceCommit();
    void testEngineNumberSelect();
    void testEngineEscape();
    void testEngineToggleChineseEnglish();

private:
    PinyinEngine* m_engine;
};

void TestPinyin::initTestCase()
{
    // 初始化日志
    Logger::instance().setLogLevel(LogLevel::Warning);

    // 初始化词库
    Dictionary::instance().initialize();

    // 创建拼音引擎
    m_engine = new PinyinEngine();
}

void TestPinyin::cleanupTestCase()
{
    delete m_engine;
}

// 拼音解析器测试

void TestPinyin::testParserSingleSyllable()
{
    PinyinParser& parser = PinyinParser::instance();

    auto result = parser.parse("ni");
    QVERIFY(result.isValid);
    QCOMPARE(result.syllables.size(), 1);
    QCOMPARE(result.syllables[0], QString("ni"));
    QCOMPARE(result.completedSyllables, 1);
}

void TestPinyin::testParserMultipleSyllables()
{
    PinyinParser& parser = PinyinParser::instance();

    auto result = parser.parse("nihao");
    QVERIFY(result.isValid);
    QCOMPARE(result.syllables.size(), 2);
    QCOMPARE(result.syllables[0], QString("ni"));
    QCOMPARE(result.syllables[1], QString("hao"));
}

void TestPinyin::testParserJianpin()
{
    PinyinParser& parser = PinyinParser::instance();

    QVERIFY(parser.isJianpinInput("nh"));
    QVERIFY(!parser.isJianpinInput("nihao"));
}

void TestPinyin::testParserInvalidInput()
{
    PinyinParser& parser = PinyinParser::instance();

    auto result = parser.parse("");
    QVERIFY(!result.isValid);
    QCOMPARE(result.syllables.size(), 0);
}

// 词库测试

void TestPinyin::testDictionaryLookup()
{
    Dictionary& dict = Dictionary::instance();

    auto candidates = dict.lookup("ni");
    QVERIFY(candidates.size() > 0);
}

void TestPinyin::testDictionaryLookupExact()
{
    Dictionary& dict = Dictionary::instance();

    auto candidates = dict.lookupExact("ni");
    QVERIFY(candidates.size() > 0);
}

void TestPinyin::testDictionaryLookupJianpin()
{
    Dictionary& dict = Dictionary::instance();

    auto candidates = dict.lookupJianpin("n");
    QVERIFY(candidates.size() > 0);
}

void TestPinyin::testDictionaryAddWord()
{
    Dictionary& dict = Dictionary::instance();

    int beforeCount = dict.lookup("ceshi").size();
    dict.addUserWord("测试", "ceshi", 100);
    int afterCount = dict.lookup("ceshi").size();

    QCOMPARE(afterCount, beforeCount + 1);
}

// 拼音引擎测试

void TestPinyin::testEngineInput()
{
    m_engine->clear();

    m_engine->processKey('n');
    m_engine->processKey('i');

    QCOMPARE(m_engine->currentInput(), QString("ni"));
    QVERIFY(m_engine->candidates().size() > 0);
}

void TestPinyin::testEngineBackspace()
{
    m_engine->clear();

    m_engine->processKey('n');
    m_engine->processKey('i');
    m_engine->processBackspace();

    QCOMPARE(m_engine->currentInput(), QString("n"));
}

void TestPinyin::testEngineSpaceCommit()
{
    m_engine->clear();

    m_engine->processKey('n');
    m_engine->processKey('i');

    QSignalSpy spy(m_engine, &PinyinEngine::commitText);
    m_engine->processSpace();

    QCOMPARE(spy.count(), 1);
    QVERIFY(m_engine->currentInput().isEmpty());
}

void TestPinyin::testEngineNumberSelect()
{
    m_engine->clear();

    m_engine->processKey('n');
    m_engine->processKey('i');

    int candidateCount = m_engine->candidates().size();
    QVERIFY(candidateCount >= 1);

    QSignalSpy spy(m_engine, &PinyinEngine::commitText);
    m_engine->processNumber(1);

    QCOMPARE(spy.count(), 1);
}

void TestPinyin::testEngineEscape()
{
    m_engine->clear();

    m_engine->processKey('n');
    m_engine->processKey('i');
    m_engine->processEscape();

    QVERIFY(m_engine->currentInput().isEmpty());
    QCOMPARE(m_engine->candidates().size(), 0);
}

void TestPinyin::testEngineToggleChineseEnglish()
{
    InputMode initialMode = m_engine->inputMode();

    m_engine->toggleChineseEnglish();
    QVERIFY(m_engine->inputMode() != initialMode);

    m_engine->toggleChineseEnglish();
    QCOMPARE(m_engine->inputMode(), initialMode);
}

QTEST_MAIN(TestPinyin)
#include "test_pinyin.moc"
