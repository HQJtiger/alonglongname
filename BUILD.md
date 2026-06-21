# 编译与打包指南

本文档详细说明如何在 Windows 上编译统一输入法并打包成安装程序。

## 目录

- [环境准备](#环境准备)
- [快速开始](#快速开始)
- [手动编译](#手动编译)
- [部署 Qt 运行库](#部署-qt-运行库)
- [打包安装程序](#打包安装程序)
- [常见问题](#常见问题)

## 环境准备

### 必需软件

| 软件 | 最低版本 | 说明 |
|------|----------|------|
| Visual Studio | 2019 / 2022 | C++ 桌面开发工作负载 |
| Qt | 6.5+ | MSVC 64位版本 |
| CMake | 3.20+ | 构建系统 |
| NSIS | 3.0+ | 打包安装程序（可选） |

### 1. 安装 Visual Studio

1. 下载 [Visual Studio Community](https://visualstudio.microsoft.com/zh-hans/vs/community/)（免费）
2. 安装时勾选 **"使用 C++ 的桌面开发"** 工作负载
3. 确保安装了 **Windows 10/11 SDK**（默认会安装）

### 2. 安装 Qt

1. 下载 [Qt 在线安装器](https://www.qt.io/download)
2. 安装时选择：
   - Qt 6.5.0（或更高版本）
   - **MSVC 2019 64-bit** 组件
   - Qt 5 Compatibility Module（可选）

3. 记住安装路径，例如：`C:\Qt\6.5.0\msvc2019_64`

### 3. 安装 CMake

- 方式一：Visual Studio 自带 CMake（推荐）
- 方式二：从 [cmake.org](https://cmake.org/download/) 下载安装
- 方式三：使用 winget：`winget install Kitware.CMake`

### 4. 安装 NSIS（可选，用于打包）

1. 下载 [NSIS](https://nsis.sourceforge.io/Download)
2. 默认安装路径：`C:\Program Files (x86)\NSIS`

## 快速开始

### 一键编译打包

最简单的方式是使用一键脚本：

1. 打开项目目录
2. 编辑 `build_and_package.bat`，修改 Qt 路径：
   ```bat
   set QT_PATH=C:\Qt\6.5.0\msvc2019_64
   ```
3. 双击运行 `build_and_package.bat`

脚本会自动完成：
- ✅ 配置 CMake
- ✅ 编译 Release 版本
- ✅ 部署 Qt 运行库
- ✅ 打包安装程序（如果安装了 NSIS）

### 分步编译

如果想分步执行，可以使用以下脚本：

1. **编译**：`build_windows.bat`
2. **部署 Qt**：`deploy_qt.bat`
3. **打包**：右键 `installer.nsi` → "Compile NSIS Script"

## 手动编译

### 1. 打开命令行

**方式一：使用 Visual Studio 命令提示符**（推荐）
- 开始菜单 → "x64 Native Tools Command Prompt for VS 2022"

**方式二：普通命令提示符 + 调用 vcvars64.bat**
```cmd
"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
```

### 2. 配置 CMake

```cmd
cd 项目目录
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.5.0\msvc2019_64"
```

参数说明：
- `-G "Visual Studio 17 2022"`：使用 VS 2022（VS 2019 用 "Visual Studio 16 2019"）
- `-A x64`：64 位
- `-DCMAKE_PREFIX_PATH`：Qt 安装路径
- `-DBUILD_TESTS=ON`：编译单元测试（可选）

### 3. 编译

```cmd
cmake --build . --config Release -j 8
```

参数说明：
- `--config Release`：Release 版本（Debug 用 Debug）
- `-j 8`：并行编译任务数，建议设置为 CPU 核心数

### 4. 输出文件

编译成功后，输出文件在：
```
build\src\platform\windows\Release\
├── UnifiedInputMethod.exe      # 主程序
├── UnifiedInputMethodTSF.dll   # TSF 输入法 DLL
└── ...
```

测试程序在：
```
build\tests\Release\
└── test_pinyin.exe             # 单元测试
```

## 部署 Qt 运行库

编译出来的 exe 不能直接运行，需要 Qt 运行库。

### 使用 windeployqt

```cmd
C:\Qt\6.5.0\msvc2019_64\bin\windeployqt.exe --release build\src\platform\windows\Release\UnifiedInputMethod.exe
```

常用参数：
- `--release`：部署 Release 版本运行库
- `--debug`：部署 Debug 版本运行库
- `--no-translations`：不包含翻译文件（减小体积）
- `--no-system-d3d-compiler`：不包含 D3D 编译器
- `--no-opengl-sw`：不包含软件 OpenGL 渲染器

### 验证部署

部署完成后，双击 `UnifiedInputMethod.exe` 应该能直接运行。

如果提示缺少 DLL，说明部署不完整，重新运行 windeployqt。

## 打包安装程序

### 使用 NSIS

1. 确保已编译并部署 Qt 运行库
2. 右键 `installer.nsi` → "Compile NSIS Script"
3. 或者命令行：
   ```cmd
   "C:\Program Files (x86)\NSIS\makensis.exe" installer.nsi
   ```

### 安装包内容

生成的安装包包含：
- ✅ 主程序 UnifiedInputMethod.exe
- ✅ TSF 输入法 DLL
- ✅ Qt 运行库
- ✅ 开始菜单快捷方式
- ✅ 卸载程序
- ✅ 自动注册输入法

### 安装测试

1. 运行生成的安装包 exe
2. 按提示完成安装
3. 安装完成后，按 `Win + 空格` 切换输入法
4. 应该能看到"统一输入法"

## 常见问题

### Q: CMake 提示找不到 Qt

**原因**：`CMAKE_PREFIX_PATH` 设置不正确

**解决**：
1. 确认 Qt 安装路径
2. 路径中使用正斜杠 `/` 或双反斜杠 `\\`
3. 确保路径指向 `msvc2019_64` 目录（不是 `6.5.0` 根目录）

### Q: 编译报错找不到 msctf.h

**原因**：Windows SDK 未安装或版本太旧

**解决**：
1. 打开 Visual Studio Installer
2. 修改安装，确保安装了 Windows 10/11 SDK
3. 重新编译

### Q: TSF DLL 注册失败

**原因**：
1. 没有管理员权限
2. DLL 缺少依赖
3. 位数不匹配

**解决**：
1. 以管理员身份运行命令提示符
2. 用 Dependencies 工具检查 DLL 依赖
3. 确保是 64 位 DLL 在 64 位系统上注册

### Q: 输入法注册了但不显示

**原因**：可能是语言配置问题

**解决**：
1. 打开设置 → 时间和语言 → 语言和区域
2. 点击"中文"→"语言选项"
3. 检查"键盘"列表中是否有"统一输入法"
4. 如果没有，点击"添加键盘"添加

### Q: 候选词窗口不显示

**原因**：TSF 文本服务与 Qt 窗口集成问题

**解决**：
这是 MVP 版本的已知问题，需要进一步完善 TSF 集成。当前版本可以：
1. 先运行主程序测试设置界面
2. TSF 集成需要后续迭代完善

### Q: 如何调试 TSF DLL

**方法**：
1. 编译 Debug 版本
2. 在 Visual Studio 中设置附加到进程
3. 附加到 `ctfmon.exe` 或目标应用程序
4. 在代码中设置断点

## 开发调试建议

### Debug 编译

```cmd
cmake --build . --config Debug
```

### 运行单元测试

```cmd
cd build
ctest -C Release --output-on-failure
```

### 查看日志

日志文件位置：
```
%APPDATA%\UnifiedInputMethod\logs\
```

### 配置文件位置

```
%APPDATA%\UnifiedInputMethod\config.ini
```
