@echo off
REM ========================================
REM 统一输入法 - Windows 编译脚本
REM ========================================

setlocal enabledelayedexpansion

echo ========================================
echo   统一输入法 - Windows 编译脚本
echo ========================================
echo.

REM 设置 Qt 路径（请根据实际情况修改）
set QT_PATH=C:\Qt\6.5.0\msvc2019_64

REM 设置 Visual Studio 版本
set VS_VERSION=2022
set VS_EDITION=Community

REM 检查 Qt 是否存在
if not exist "%QT_PATH%\bin\qmake.exe" (
    echo [错误] 未找到 Qt，请修改脚本中的 QT_PATH 变量
    echo 当前路径: %QT_PATH%
    echo.
    echo 常见 Qt 路径:
    echo   C:\Qt\6.5.0\msvc2019_64
    echo   C:\Qt\6.6.0\msvc2019_64
    echo   C:\Qt\6.7.0\msvc2019_64
    pause
    exit /b 1
)

echo [信息] Qt 路径: %QT_PATH%
echo.

REM 检查 CMake
where cmake >nul 2>&1
if errorlevel 1 (
    echo [错误] 未找到 CMake，请先安装 CMake 并添加到 PATH
    pause
    exit /b 1
)

echo [信息] CMake 版本:
cmake --version | findstr "version"
echo.

REM 设置 Visual Studio 环境
echo [信息] 检测 Visual Studio...

set VS_PATH=
if exist "C:\Program Files\Microsoft Visual Studio\%VS_VERSION%\%VS_EDITION%\VC\Auxiliary\Build\vcvars64.bat" (
    set VS_PATH=C:\Program Files\Microsoft Visual Studio\%VS_VERSION%\%VS_EDITION%\VC\Auxiliary\Build
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\%VS_VERSION%\%VS_EDITION%\VC\Auxiliary\Build\vcvars64.bat" (
    set VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\%VS_VERSION%\%VS_EDITION%\VC\Auxiliary\Build
)

if "%VS_PATH%"=="" (
    echo [警告] 未找到 Visual Studio %VS_VERSION% %VS_EDITION%
    echo 请确保已安装 Visual Studio 2019 或 2022
    echo.
    echo 如果已安装，请修改脚本中的 VS_VERSION 和 VS_EDITION 变量
    pause
    exit /b 1
)

echo [信息] Visual Studio 路径: %VS_PATH%
echo.

REM 调用 vcvars64.bat 设置环境
call "%VS_PATH%\vcvars64.bat" >nul

echo ========================================
echo   开始编译
echo ========================================
echo.

REM 创建构建目录
if not exist build mkdir build
cd build

REM 配置 CMake
echo [1/3] 配置 CMake...
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="%QT_PATH%" -DBUILD_TESTS=ON
if errorlevel 1 (
    echo.
    echo [错误] CMake 配置失败
    cd ..
    pause
    exit /b 1
)

echo.
echo [2/3] 编译 Release 版本...
cmake --build . --config Release -j %NUMBER_OF_PROCESSORS%
if errorlevel 1 (
    echo.
    echo [错误] 编译失败
    cd ..
    pause
    exit /b 1
)

echo.
echo [3/3] 运行单元测试...
ctest -C Release --output-on-failure
if errorlevel 1 (
    echo.
    echo [警告] 部分测试失败，但不影响主程序运行
)

cd ..

echo.
echo ========================================
echo   编译完成！
echo ========================================
echo.
echo 输出文件位置:
echo   主程序: build\src\platform\windows\Release\UnifiedInputMethod.exe
echo   TSF DLL: build\src\platform\windows\Release\UnifiedInputMethodTSF.dll
echo   测试程序: build\tests\Release\test_pinyin.exe
echo.

pause
