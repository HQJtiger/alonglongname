@echo off
REM ========================================
REM 统一输入法 - 一键编译打包脚本
REM ========================================
REM 功能: 编译 -> 部署 Qt 运行库 -> 打包安装程序
REM ========================================

setlocal enabledelayedexpansion

echo ========================================
echo   统一输入法 - 一键编译打包
echo ========================================
echo.

REM 设置 Qt 路径（请根据实际情况修改）
set QT_PATH=C:\Qt\6.5.0\msvc2019_64

REM 设置 NSIS 路径
set NSIS_PATH=C:\Program Files (x86)\NSIS

REM 检查 Qt
if not exist "%QT_PATH%\bin\qmake.exe" (
    echo [错误] 未找到 Qt，请修改脚本中的 QT_PATH 变量
    pause
    exit /b 1
)

echo [信息] Qt 路径: %QT_PATH%

REM 检查 CMake
where cmake >nul 2>&1
if errorlevel 1 (
    echo [错误] 未找到 CMake
    pause
    exit /b 1
)

echo [信息] CMake 已就绪

REM 检查 NSIS
if not exist "%NSIS_PATH%\makensis.exe" (
    echo [警告] 未找到 NSIS，将跳过安装包打包步骤
    echo 安装 NSIS 后可以生成安装包: https://nsis.sourceforge.io/
    set SKIP_INSTALLER=1
) else (
    echo [信息] NSIS 已就绪
    set SKIP_INSTALLER=0
)

echo.
echo ========================================
echo   步骤 1/3: 编译项目
echo ========================================
echo.

REM 创建构建目录
if not exist build mkdir build
cd build

REM 配置 CMake
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="%QT_PATH%" -DBUILD_TESTS=ON
if errorlevel 1 (
    echo.
    echo [错误] CMake 配置失败
    cd ..
    pause
    exit /b 1
)

REM 编译
cmake --build . --config Release -j %NUMBER_OF_PROCESSORS%
if errorlevel 1 (
    echo.
    echo [错误] 编译失败
    cd ..
    pause
    exit /b 1
)

cd ..

echo.
echo [完成] 编译成功
echo.

echo ========================================
echo   步骤 2/3: 部署 Qt 运行库
echo ========================================
echo.

set OUTPUT_DIR=build\src\platform\windows\Release

"%QT_PATH%\bin\windeployqt.exe" --release --no-translations --no-system-d3d-compiler --no-opengl-sw "%OUTPUT_DIR%\UnifiedInputMethod.exe"

if errorlevel 1 (
    echo.
    echo [警告] Qt 部署可能有问题，但继续执行
)

echo.
echo [完成] Qt 运行库已部署
echo.

if "%SKIP_INSTALLER%"=="1" (
    echo ========================================
    echo   跳过安装包打包
    echo ========================================
    echo.
    echo 编译输出目录: %OUTPUT_DIR%
    echo 可以直接运行 UnifiedInputMethod.exe 测试
    echo.
    pause
    exit /b 0
)

echo ========================================
echo   步骤 3/3: 打包安装程序
echo ========================================
echo.

"%NSIS_PATH%\makensis.exe" installer.nsi

if errorlevel 1 (
    echo.
    echo [错误] 安装包打包失败
    pause
    exit /b 1
)

echo.
echo ========================================
echo   全部完成！
echo ========================================
echo.
echo 安装包位置: 统一输入法 v0.1.0 安装包.exe
echo 编译输出: %OUTPUT_DIR%
echo.
echo 接下来:
echo   1. 运行安装包进行安装测试
echo   2. 或者直接运行 %OUTPUT_DIR%\UnifiedInputMethod.exe
echo.

pause
