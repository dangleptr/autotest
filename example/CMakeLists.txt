cmake_minimum_required(VERSION 3.10)

include_directories(
    "include"
)

find_package(autotest REQUIRED)

include(../tests/tests.cmake)

create_test_target(block-array "src/main.cpp")