# 配置管理模块
set(CONFIG_SOURCES
    ConfigManager.cpp
)

set(CONFIG_HEADERS
    ConfigManager.h
)

add_library(uim_config STATIC ${CONFIG_SOURCES} ${CONFIG_HEADERS})

target_link_libraries(uim_config
    PUBLIC Qt6::Core
)

target_include_directories(uim_config
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
