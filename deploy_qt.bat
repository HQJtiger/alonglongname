@echo off
REM ========================================
REM 统一输入法 - 部署 Qt 运行库
REM ========================================
REM 功能: 自动收集 Qt 运行库到输出目录
REM ========================================

setlocal enabledelayedexpansion

echo ========================================
echo   统一输入法 - Qt 运行库部署
echo ========================================
echo.

REM 设置 Qt 路径
set QT_PATH=C:\Qt\6.5.0\msvc2019_64

REM 检查 Qt 是否存在
if not exist "%QT_PATH%\bin\windeployqt.exe" (
    echo [错误] 未找到 windeployqt.exe
    echo 请修改脚本中的 QT_PATH 变量
    echo 当前路径: %QT_PATH%
    pause
    exit /b 1
)

REM 输出目录
set OUTPUT_DIR=build\src\platform\windows\Release

if not exist "%OUTPUT_DIR%\UnifiedInputMethod.exe" (
    echo [错误] 未找到编译输出，请先运行 build_windows.bat 编译
    echo 期望路径: %OUTPUT_DIR%\UnifiedInputMethod.exe
    pause
    exit /b 1
)

echo [信息] 正在部署 Qt 运行库...
echo.

"%QT_PATH%\bin\windeployqt.exe" --release --no-translations --no-system-d3d-compiler --no-opengl-sw "%OUTPUT_DIR%\UnifiedInputMethod.exe"

if errorlevel 1 (
    echo.
    echo [错误] 部署失败
    pause
    exit /b 1
)

echo.
echo ========================================
echo   部署完成！
echo ========================================
echo.
echo 运行库已部署到: %OUTPUT_DIR%
echo.
echo 接下来可以:
echo   1. 运行 installer.nsi 打包安装程序
echo   2. 或者直接运行 %OUTPUT_DIR%\UnifiedInputMethod.exe 测试
echo.

pause
