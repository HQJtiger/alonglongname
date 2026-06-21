# 测试模块
find_package(Qt6 REQUIRED COMPONENTS Test)

# 拼音引擎测试
add_executable(test_pinyin test_pinyin.cpp)
target_link_libraries(test_pinyin
    PRIVATE
    uim_core
    Qt6::Core
    Qt6::Test
)
add_test(NAME test_pinyin COMMAND test_pinyin)
