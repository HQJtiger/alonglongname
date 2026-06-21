# UI 模块：候选词窗口、设置窗口
set(UI_SOURCES
    candidate/CandidateWindow.cpp
    candidate/CandidateListModel.cpp
    settings/SettingsDialog.cpp
    settings/GeneralSettingsPage.cpp
    settings/AppearanceSettingsPage.cpp
)

set(UI_HEADERS
    candidate/CandidateWindow.h
    candidate/CandidateListModel.h
    settings/SettingsDialog.h
    settings/GeneralSettingsPage.h
    settings/AppearanceSettingsPage.h
)

set(UI_RESOURCES
    resources/resources.qrc
)

add_library(uim_ui STATIC ${UI_SOURCES} ${UI_HEADERS} ${UI_RESOURCES})

target_link_libraries(uim_ui
    PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets
    PRIVATE uim_core uim_config
)

target_include_directories(uim_ui
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
