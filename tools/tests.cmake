set(FUZZER_FLAGS "-fsanitize=fuzzer,address -fsanitize-coverage=edge,pc-table,inline-8bit-counters")
set(COV_FLAGS "-fprofile-instr-generate -fcoverage-mapping")

find_package(autotest REQUIRED)

add_library(standalone-main STATIC "../../tools/StandaloneFuzzTargetMain.c")

function(create_test_target target_name source )

    add_executable("${target_name}-fuzzer" "${source}")

    set_target_properties("${target_name}-fuzzer" PROPERTIES COMPILE_FLAGS "-O0 -fno-omit-frame-pointer ${FUZZER_FLAGS}")
    set_target_properties("${target_name}-fuzzer" PROPERTIES LINK_FLAGS " -fno-omit-frame-pointer ${FUZZER_FLAGS}")

    target_compile_features("${target_name}-fuzzer" INTERFACE cxx_std_14)
    target_link_libraries("${target_name}-fuzzer" PRIVATE autotest::autotest ${ARGN})

    add_executable("${target_name}-coverage" "${source}")

    set_target_properties("${target_name}-coverage" PROPERTIES COMPILE_FLAGS "-O0 -fno-omit-frame-pointer ${COV_FLAGS}")
    set_target_properties("${target_name}-coverage" PROPERTIES LINK_FLAGS " -fno-omit-frame-pointer ${COV_FLAGS} ")

    target_compile_features("${target_name}-coverage" INTERFACE cxx_std_17)
    target_link_libraries("${target_name}-coverage" PRIVATE autotest::autotest standalone-main ${ARGN})

endfunction()