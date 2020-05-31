cmake_minimum_required(VERSION 3.10)

include_directories(
    "include"
)

find_package(autotest REQUIRED)

include(../tools/tests.cmake)

create_test_target(block-array "src/block-array-test.cpp")

create_test_target(robin-flat-map "src/robin-hood-test.cpp")
