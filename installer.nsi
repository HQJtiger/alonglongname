; ========================================
; 统一输入法 - NSIS 安装包脚本
; ========================================
; 使用方法:
;   1. 安装 NSIS (https://nsis.sourceforge.io/)
;   2. 右键此文件 -> "Compile NSIS Script"
;   3. 或者命令行: makensis installer.nsi
; ========================================

; 安装包基本信息
!define APP_NAME "统一输入法"
!define APP_VERSION "0.1.0"
!define APP_PUBLISHER "UnifiedInputMethod"
!define APP_WEBSITE "https://github.com/example/unified-input-method"

!define EXE_NAME "UnifiedInputMethod.exe"
!define DLL_NAME "UnifiedInputMethodTSF.dll"

; 默认安装路径
!define INSTALL_DIR "$PROGRAMFILES\UnifiedInputMethod"

; 输出文件名
OutFile "统一输入法 v${APP_VERSION} 安装包.exe"

; 安装包界面设置
InstallDir "${INSTALL_DIR}"
InstallDirRegKey HKLM "Software\${APP_PUBLISHER}\${APP_NAME}" "InstallLocation"

; 请求管理员权限（注册输入法需要）
RequestExecutionLevel admin

; 安装包压缩方式
SetCompressor lzma
SetCompressor /SOLID lzma

; 界面设置
!include "MUI2.nsh"

!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; 欢迎页面
!insertmacro MUI_PAGE_WELCOME
; 许可协议页面（可选，需要准备 license.txt）
; !insertmacro MUI_PAGE_LICENSE "license.txt"
; 组件选择页面
!insertmacro MUI_PAGE_COMPONENTS
; 安装目录选择页面
!insertmacro MUI_PAGE_DIRECTORY
; 安装进度页面
!insertmacro MUI_PAGE_INSTFILES
; 完成页面
!define MUI_FINISHPAGE_RUN "$INSTDIR\${EXE_NAME}"
!define MUI_FINISHPAGE_RUN_TEXT "运行统一输入法"
!define MUI_FINISHPAGE_SHOWREADME ""
!define MUI_FINISHPAGE_SHOWREADME_TEXT "查看使用说明"
!insertmacro MUI_PAGE_FINISH

; 卸载页面
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

; 语言设置（简体中文）
!insertmacro MUI_LANGUAGE "SimpChinese"

; ========================================
; 安装区段
; ========================================

Section "主程序" SecMain
    SectionIn RO

    SetOutPath $INSTDIR

    ; 主程序
    File "build\src\platform\windows\Release\${EXE_NAME}"

    ; TSF 输入法 DLL
    File "build\src\platform\windows\Release\${DLL_NAME}"

    ; Qt 运行库
    File "build\src\platform\windows\Release\Qt6Core.dll"
    File "build\src\platform\windows\Release\Qt6Gui.dll"
    File "build\src\platform\windows\Release\Qt6Widgets.dll"
    File "build\src\platform\windows\Release\Qt6Network.dll"

    ; Qt 平台插件
    SetOutPath $INSTDIR\platforms
    File "build\src\platform\windows\Release\platforms\qwindows.dll"

    SetOutPath $INSTDIR

    ; 资源文件
    ; File /r "resources\"

    ; README
    File "README.md"

    ; 创建开始菜单快捷方式
    CreateDirectory "$SMPROGRAMS\${APP_NAME}"
    CreateShortCut "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk" "$INSTDIR\${EXE_NAME}"
    CreateShortCut "$SMPROGRAMS\${APP_NAME}\卸载 ${APP_NAME}.lnk" "$INSTDIR\uninstall.exe"
    CreateShortCut "$SMPROGRAMS\${APP_NAME}\使用说明.lnk" "$INSTDIR\README.md"

    ; 写入注册表
    WriteRegStr HKLM "Software\${APP_PUBLISHER}\${APP_NAME}" "InstallLocation" "$INSTDIR"
    WriteRegStr HKLM "Software\${APP_PUBLISHER}\${APP_NAME}" "Version" "${APP_VERSION}"

    ; 注册卸载信息
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "DisplayName" "${APP_NAME}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "DisplayVersion" "${APP_VERSION}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "Publisher" "${APP_PUBLISHER}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "InstallLocation" "$INSTDIR"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "UninstallString" "$INSTDIR\uninstall.exe"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "QuietUninstallString" "$INSTDIR\uninstall.exe /S"
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "NoModify" 1
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "NoRepair" 1

    ; 创建卸载程序
    WriteUninstaller "$INSTDIR\uninstall.exe"

SectionEnd

Section "注册输入法" SecRegisterTSF
    SectionIn RO

    ; 注册 TSF 输入法 DLL
    DetailPrint "正在注册输入法..."
    ExecWait 'regsvr32 /s "$INSTDIR\${DLL_NAME}"'

    ; 检查注册是否成功
    IfErrors 0 +2
    MessageBox MB_ICONEXCLAMATION "输入法注册失败，请手动运行 regsvr32 $INSTDIR\${DLL_NAME}"

SectionEnd

Section "桌面快捷方式" SecDesktopShortcut
    CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${EXE_NAME}"
SectionEnd

; 区段描述
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecMain} "统一输入法主程序和核心组件"
    !insertmacro MUI_DESCRIPTION_TEXT ${SecRegisterTSF} "注册为 Windows 系统输入法（需要管理员权限）"
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDesktopShortcut} "在桌面创建快捷方式"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

; ========================================
; 卸载区段
; ========================================

Section "uninstall"

    ; 停止程序
    DetailPrint "正在关闭程序..."
    ExecWait 'taskkill /F /IM ${EXE_NAME}'

    ; 注销输入法
    DetailPrint "正在注销输入法..."
    ExecWait 'regsvr32 /s /u "$INSTDIR\${DLL_NAME}"'

    ; 删除安装目录
    RMDir /r "$INSTDIR"

    ; 删除开始菜单
    RMDir /r "$SMPROGRAMS\${APP_NAME}"

    ; 删除桌面快捷方式
    Delete "$DESKTOP\${APP_NAME}.lnk"

    ; 删除注册表
    DeleteRegKey HKLM "Software\${APP_PUBLISHER}\${APP_NAME}"
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"

SectionEnd

; ========================================
; 安装完成后函数
; ========================================

Function .onInstSuccess
    MessageBox MB_OK|MB_ICONINFORMATION "${APP_NAME} 安装完成！$\r$\n$\r$\n输入法已注册，请按 Win+空格 切换使用。"
FunctionEnd
