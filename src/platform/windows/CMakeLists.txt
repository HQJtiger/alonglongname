# Windows 平台适配模块
# 包含 TSF 输入法框架集成

set(WINDOWS_SOURCES
    utils/WinUtils.cpp
    tsf/TSFModule.cpp
    tsf/TSFTextService.cpp
    tsf/TSFLanguageBar.cpp
)

set(WINDOWS_HEADERS
    utils/WinUtils.h
    tsf/TSFModule.h
    tsf/TSFTextService.h
    tsf/TSFLanguageBar.h
)

# TSF 相关的 IDL 文件（可选，用于生成 COM 接口）
# set(WINDOWS_IDLS
#     tsf/TextService.idl
# )

add_library(uim_platform_windows STATIC ${WINDOWS_SOURCES} ${WINDOWS_HEADERS})

target_link_libraries(uim_platform_windows
    PUBLIC uim_core uim_ui
    PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets
)

# Windows 特定库
target_link_libraries(uim_platform_windows
    PRIVATE
    shlwapi
    comctl32
    msctf      # TSF 库
    uuid
    ole32
    oleaut32
)

target_include_directories(uim_platform_windows
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

# TSF 输入法 DLL（独立的 COM DLL，用于注册到系统）
add_library(uim_tsf_plugin SHARED
    tsf/TSFPlugin.cpp
    tsf/TSFTextService.cpp
    tsf/TSFLanguageBar.cpp
    tsf/TSFModule.cpp
    utils/WinUtils.cpp
)

target_link_libraries(uim_tsf_plugin
    PRIVATE
    uim_core
    uim_ui
    uim_config
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    shlwapi
    comctl32
    msctf
    uuid
    ole32
    oleaut32
)

set_target_properties(uim_tsf_plugin PROPERTIES
    OUTPUT_NAME "UnifiedInputMethodTSF"
    PREFIX ""
)

# 主程序入口（设置界面、托盘等）
add_executable(UnifiedInputMethod WIN32
    main.cpp
)

target_link_libraries(UnifiedInputMethod
    PRIVATE
    uim_core
    uim_ui
    uim_config
    uim_clipboard
    uim_platform_windows
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)
