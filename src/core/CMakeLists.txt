# 核心模块：拼音引擎、词库、通用工具
set(CORE_SOURCES
    common/Logger.cpp
    common/Types.cpp
    pinyin/PinyinEngine.cpp
    pinyin/PinyinParser.cpp
    dictionary/Dictionary.cpp
)

set(CORE_HEADERS
    common/Logger.h
    common/Types.h
    pinyin/PinyinEngine.h
    pinyin/PinyinParser.h
    dictionary/Dictionary.h
)

add_library(uim_core STATIC ${CORE_SOURCES} ${CORE_HEADERS})

target_link_libraries(uim_core
    PUBLIC Qt6::Core
)

target_include_directories(uim_core
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
