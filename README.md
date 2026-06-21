# 统一输入法（Unified Input Method）

一个跨平台的开源桌面输入法，支持拼音输入、剪贴板同步、语音输入等功能。

> **当前版本**：0.1.0 (MVP)
> **开发阶段**：第一阶段 - 核心输入

## 功能特性

### 已实现（MVP）
- ✅ 基础拼音输入（全拼 + 简拼）
- ✅ 智能候选词排序（基于词频）
- ✅ 中英文切换
- ✅ 候选词翻页
- ✅ Windows TSF 输入法框架集成
- ✅ Qt 候选词窗口（浅色/深色主题）
- ✅ 基础设置界面
- ✅ 剪贴板历史记录框架
- ✅ 剪贴板同步框架（局域网 P2P）
- ✅ 系统托盘图标

### 计划中
- 🔄 完整词库（Rime 八股文词库，约 10 万词）
- 🔄 剪贴板同步（端到端加密）
- 🔄 本地语音输入（Vosk / whisper.cpp）
- 🔄 macOS 平台适配
- 🔄 Linux 平台适配
- 🔄 智能输入优化（词频学习、上下文联想、智能纠错）
- 🔄 配置同步

## 技术栈

| 类别 | 技术 |
|------|------|
| 语言 | C++17 |
| UI 框架 | Qt 6.5+ |
| 构建系统 | CMake 3.20+ |
| 平台 | Windows（MSVC）/ macOS / Linux |
| 输入法框架 | Windows TSF / macOS IMK / Linux IBus |
| 加密 | OpenSSL（后续集成） |
| 语音识别 | Vosk / whisper.cpp（后续集成） |

## 项目结构

```
UnifiedInputMethod/
├── CMakeLists.txt              # 主 CMake 文件
├── README.md                   # 项目说明
├── src/
│   ├── core/                   # 核心引擎模块
│   │   ├── common/             # 通用工具
│   │   │   ├── Types.h/cpp     # 通用类型定义
│   │   │   └── Logger.h/cpp    # 日志工具
│   │   ├── pinyin/             # 拼音引擎
│   │   │   ├── PinyinEngine.h/cpp   # 拼音引擎主类
│   │   │   └── PinyinParser.h/cpp   # 拼音解析器
│   │   └── dictionary/         # 词库
│   │       └── Dictionary.h/cpp     # 词库管理类
│   ├── ui/                     # UI 模块（Qt）
│   │   ├── candidate/          # 候选词窗口
│   │   │   ├── CandidateWindow.h/cpp     # 候选词窗口
│   │   │   └── CandidateListModel.h/cpp  # 候选词列表模型
│   │   ├── settings/           # 设置窗口
│   │   │   ├── SettingsDialog.h/cpp           # 设置对话框
│   │   │   ├── GeneralSettingsPage.h/cpp      # 通用设置页
│   │   │   └── AppearanceSettingsPage.h/cpp   # 外观设置页
│   │   └── resources/
│   │       └── resources.qrc   # Qt 资源文件
│   ├── platform/               # 平台相关
│   │   ├── windows/            # Windows 平台
│   │   │   ├── main.cpp        # 主程序入口
│   │   │   ├── tsf/            # TSF 输入法
│   │   │   │   ├── TSFModule.h/cpp        # TSF 模块管理
│   │   │   │   ├── TSFTextService.h/cpp   # TSF 文本服务
│   │   │   │   ├── TSFLanguageBar.h/cpp   # TSF 语言栏
│   │   │   │   └── TSFPlugin.cpp          # TSF DLL 入口
│   │   │   └── utils/          # Windows 工具
│   │   │       └── WinUtils.h/cpp
│   │   ├── macos/              # macOS 平台（预留）
│   │   └── linux/              # Linux 平台（预留）
│   ├── clipboard/              # 剪贴板同步
│   │   ├── ClipboardTypes.h    # 类型定义
│   │   ├── ClipboardManager.h/cpp  # 剪贴板管理器
│   │   └── sync/
│   │       └── SyncManager.h/cpp     # 同步管理器
│   └── config/                 # 配置管理
│       └── ConfigManager.h/cpp
├── resources/                  # 资源文件
│   ├── icons/
│   └── themes/
│       └── default.json
├── tests/                      # 测试
│   ├── CMakeLists.txt
│   └── test_pinyin.cpp
└── third_party/                # 第三方依赖（预留）
```

## 编译说明

### 环境要求

- **操作系统**：Windows 10/11
- **编译器**：Visual Studio 2019/2022 (MSVC)
- **Qt**：6.5 或更高版本
- **CMake**：3.20 或更高版本
- **Windows SDK**：包含 TSF 相关头文件和库

### 编译步骤

1. **克隆项目**
```bash
git clone <repository-url>
cd UnifiedInputMethod
```

2. **创建构建目录**
```bash
mkdir build
cd build
```

3. **配置 CMake**
```bash
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.5.0/msvc2019_64"
```

或者使用 Visual Studio 打开：
```bash
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:/Qt/6.5.0/msvc2019_64"
```

4. **编译**
```bash
cmake --build . --config Release
```

或者在 Visual Studio 中打开 `UnifiedInputMethod.sln` 进行编译。

### 可选参数

- `-DBUILD_TESTS=ON`：编译单元测试（默认 OFF）
- `-DCMAKE_BUILD_TYPE=Debug`：Debug 模式

## 运行说明

### 主程序

编译完成后，运行 `UnifiedInputMethod.exe`：

- 系统托盘会显示输入法图标
- 双击托盘图标打开设置界面
- 右键托盘图标显示菜单

### 注册 TSF 输入法

> **注意**：需要管理员权限

1. 找到编译生成的 `UnifiedInputMethodTSF.dll`
2. 以管理员身份运行命令提示符
3. 执行注册命令：
```cmd
regsvr32 UnifiedInputMethodTSF.dll
```

4. 注销命令：
```cmd
regsvr32 /u UnifiedInputMethodTSF.dll
```

### 使用输入法

1. 注册成功后，在 Windows 语言设置中添加"统一输入法"
2. 按 `Win + 空格` 切换到统一输入法
3. 输入拼音即可看到候选词窗口
4. 按数字键 1-9 选择候选词
5. 按 `+/-` 翻页
6. 按 `Shift` 切换中英文

### 运行测试

```bash
cd build
ctest -C Release
```

或者直接运行测试程序：
```bash
./tests/Release/test_pinyin.exe
```

## 代码风格

- **类名**：大驼峰（PascalCase）
- **函数名**：小驼峰（camelCase）
- **变量名**：小驼峰
- **成员变量**：`m_` 前缀
- **命名空间**：按模块划分（根命名空间 `uim`）
- **注释**：Doxygen 风格
- **错误处理**：返回 `bool` 或 `std::expected`，少用异常

## 开发路线图

### 第一阶段：核心输入（当前）
- ✅ 拼音引擎 + Windows TSF 适配 + 候选窗口
- ✅ 简化词库，能跑通流程

### 第二阶段：剪贴板同步
- 历史记录 + 加密同步 + 设备配对
- 局域网同步优先

### 第三阶段：语音输入
- Vosk 集成 + 实时识别 + 悬浮窗
- 完全离线运行

### 第四阶段：多平台适配与优化
- macOS 适配（IMK）
- Linux 适配（IBus/Fcitx5）
- 性能优化（启动 < 100ms，内存 < 100MB）
- 主题系统完善
- 智能输入优化
- 配置同步
- 安装与更新
- 测试与发布

## 模块说明

### 核心模块（core）
- **PinyinEngine**：拼音引擎主类，处理按键输入、候选词生成、文本提交
- **PinyinParser**：拼音解析器，音节切分（正向最大匹配）
- **Dictionary**：词库管理，内置简化词库，支持全拼/简拼查询
- **Logger**：日志工具，支持控制台和文件输出
- **Types**：通用类型定义

### UI 模块（ui）
- **CandidateWindow**：候选词窗口，无边框、置顶、半透明
- **CandidateListModel**：候选词列表模型
- **SettingsDialog**：设置对话框，左侧导航 + 右侧内容
- **GeneralSettingsPage**：通用设置页（输入、剪贴板、常规）
- **AppearanceSettingsPage**：外观设置页（主题、字体、窗口）

### 配置模块（config）
- **ConfigManager**：配置管理器，基于 QSettings（INI 格式）

### 平台模块（platform）
- **Windows TSF**：
  - TSFTextService：TSF 文本服务，实现多个 COM 接口
  - TSFLanguageBar：TSF 语言栏
  - TSFModule：TSF 模块管理
  - TSFPlugin：TSF DLL 入口
  - WinUtils：Windows 工具函数

### 剪贴板模块（clipboard）
- **ClipboardManager**：剪贴板管理器，监听系统剪贴板、历史记录管理
- **SyncManager**：同步管理器，设备发现、数据同步（框架）

## 常见问题

### Q: 为什么选择 TSF 而不是 IMM？
A: TSF 是微软推荐的新一代输入法框架，功能更强大，支持更丰富的文本服务。IMM 是旧框架，已逐渐被淘汰。

### Q: TSF DLL 和主程序是什么关系？
A: TSF 插件是独立的 DLL，由 Windows 文本服务框架加载。主程序是独立的可执行文件，负责设置界面、系统托盘等功能。二者可以通过命名管道、共享内存等方式通信。

### Q: 词库为什么用简化版？
A: MVP 阶段优先保证流程跑通，简化词库体积小、加载快。后续会替换为完整的 Rime 八股文词库。

### Q: 剪贴板同步安全吗？
A: 设计上采用端到端加密（AES-256-GCM），中继服务器只转发加密数据，不解密、不存储。MVP 阶段暂未实现加密，后续会补充。

## 许可证

MIT License

## 贡献

欢迎提交 Issue 和 Pull Request！

## 联系方式

- 项目地址：[GitHub]
- 问题反馈：[Issues]
