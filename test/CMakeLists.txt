include(GoogleTest)

add_executable(lazycpp_units
    tests.cpp
)
target_link_libraries(lazycpp_units PRIVATE gtest_main lazycpp)

gtest_discover_tests(lazycpp_units)
