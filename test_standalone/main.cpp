/**
 * @file main.cpp
 * @brief 拼音输入法命令行测试程序
 *
 * 纯 C++ 实现，不依赖 Qt，可在 Linux 下编译运行。
 * 用于验证拼音引擎核心逻辑。
 */

#include <iostream>
#include <string>
#include <vector>
#include <codecvt>
#include <locale>

// Windows 用 conio.h
#ifdef _WIN32
#include <conio.h>
#endif

// Linux 下用 termios.h
#ifdef __linux__
#include <termios.h>
#include <unistd.h>
#endif

#include "pinyin_standalone.h"

using namespace uim;

// Linux 下的 getch 实现
#ifdef __linux__
int getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif

// wstring 转 UTF-8 字符串（用于输出）
static std::string wstringToUtf8(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

void printHelp() {
    std::cout << "========================================" << std::endl;
    std::cout << "  统一输入法 - 命令行测试版" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    std::cout << "操作说明:" << std::endl;
    std::cout << "  字母键   - 输入拼音" << std::endl;
    std::cout << "  退格键   - 删除一个拼音" << std::endl;
    std::cout << "  空格键   - 提交第一个候选词" << std::endl;
    std::cout << "  数字键1-9 - 选择对应候选词" << std::endl;
    std::cout << "  +/-      - 翻页" << std::endl;
    std::cout << "  ESC      - 清除输入" << std::endl;
    std::cout << "  Enter    - 提交原始拼音" << std::endl;
    std::cout << "  q        - 退出程序" << std::endl;
    std::cout << std::endl;
    std::cout << "示例: 输入 nihao，按空格提交" << std::endl;
    std::cout << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << std::endl;
}

void printStatus(PinyinEngine& engine, const std::wstring& committed) {
    // 清屏（简单方式：输出多行空行）
    std::cout << "\033[2J\033[H";  // ANSI 转义码清屏

    printHelp();

    // 显示已提交的文本
    std::cout << "已输入: " << wstringToUtf8(committed) << std::endl;
    std::cout << std::endl;

    // 显示当前拼音
    std::cout << "拼音: " << engine.currentInput() << std::endl;
    std::cout << std::endl;

    // 显示候选词
    const auto& candidates = engine.currentCandidates();
    if (candidates.empty()) {
        std::cout << "(无候选词)" << std::endl;
    } else {
        std::cout << "候选词 (第 " << engine.currentPage() + 1 << "/" << engine.totalPages() << " 页):" << std::endl;
        std::cout << std::endl;
        for (size_t i = 0; i < candidates.size(); i++) {
            std::cout << "  " << candidates[i].index << ". " << wstringToUtf8(candidates[i].text);
            std::cout << "  [" << candidates[i].pinyin << "]";
            std::cout << "  (词频: " << candidates[i].frequency << ")";
            std::cout << std::endl;
        }
    }

    std::cout << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "请输入 (按 q 退出): ";
    std::cout.flush();
}

int main() {
    PinyinEngine engine;
    std::wstring committedText;

    printStatus(engine, committedText);

    while (true) {
        int key = getch();

        // 退出
        if (key == 'q' || key == 'Q') {
            break;
        }

        // ESC
        if (key == 27) {
            engine.processEscape();
            printStatus(engine, committedText);
            continue;
        }

        // 退格
        if (key == 127 || key == '\b') {
            engine.processBackspace();
            printStatus(engine, committedText);
            continue;
        }

        // 空格
        if (key == ' ') {
            std::wstring result = engine.processSpace();
            if (!result.empty()) {
                committedText += result;
            }
            printStatus(engine, committedText);
            continue;
        }

        // 回车
        if (key == '\n' || key == '\r') {
            std::string result = engine.processEnter();
            if (!result.empty()) {
                committedText += std::wstring(result.begin(), result.end());
            }
            printStatus(engine, committedText);
            continue;
        }

        // 翻页
        if (key == '+' || key == '=') {
            engine.processPage(true);
            printStatus(engine, committedText);
            continue;
        }
        if (key == '-') {
            engine.processPage(false);
            printStatus(engine, committedText);
            continue;
        }

        // 数字键选择
        if (key >= '1' && key <= '9') {
            int num = key - '0';
            std::wstring result = engine.processNumber(num);
            if (!result.empty()) {
                committedText += result;
            }
            printStatus(engine, committedText);
            continue;
        }

        // 字母输入
        if (isalpha(key)) {
            engine.processKey(key);
            printStatus(engine, committedText);
            continue;
        }

        // 其他键忽略
    }

    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "  测试结束" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    std::cout << "最终输入结果: " << wstringToUtf8(committedText) << std::endl;
    std::cout << std::endl;

    return 0;
}
