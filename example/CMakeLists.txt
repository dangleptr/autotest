cmake_minimum_required(VERSION 3.10)

include_directories(
    "include"
)

find_package(autotest REQUIRED)

add_executable(example "src/main.cpp")
set_target_properties(example PROPERTIES LINK_FLAGS " -fno-omit-frame-pointer -fsanitize=fuzzer,address -fsanitize-coverage=edge,pc-table,inline-8bit-counters")
set_target_properties(example PROPERTIES COMPILE_FLAGS "-O0 -fno-omit-frame-pointer -fsanitize=address,fuzzer -fsanitize-coverage=edge,pc-table,inline-8bit-counters"
)
target_compile_features(example INTERFACE cxx_std_14)
target_link_libraries(example PRIVATE autotest::autotest)
