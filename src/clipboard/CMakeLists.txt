# 剪贴板同步模块
set(CLIPBOARD_SOURCES
    ClipboardManager.cpp
    ClipboardHistory.cpp
    sync/SyncManager.cpp
)

set(CLIPBOARD_HEADERS
    ClipboardManager.h
    ClipboardHistory.h
    sync/SyncManager.h
)

add_library(uim_clipboard STATIC ${CLIPBOARD_SOURCES} ${CLIPBOARD_HEADERS})

target_link_libraries(uim_clipboard
    PUBLIC Qt6::Core Qt6::Gui Qt6::Network
    PRIVATE uim_config
)

target_include_directories(uim_clipboard
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
