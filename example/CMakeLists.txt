cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

include_directories(
    "include"
    "../autotest/include"
)

#find_package(autotest REQUIRED)

include(../tools/tests.cmake)

create_test_target(block-array "src/block-array-test.cpp")

create_test_target(robin-flat-map "src/robin-hood-test.cpp")
