# Clang

find_package(Clang REQUIRED)

## We need a header that was not exported properly

set(VERSION_REGEX "[0-9]+\\.[0-9]+\\.[0-9]+")
if(EXISTS "${CLANG_INCLUDE_DIRS}/clang/Basic/Version.inc")
    file(READ "${CLANG_INCLUDE_DIRS}/clang/Basic/Version.inc" CLANG_VERSION_DATA)
    string(REGEX REPLACE ";" "\\\\;" CLANG_VERSION_DATA ${CLANG_VERSION_DATA})
    string(REGEX REPLACE "\n" ";" CLANG_VERSION_DATA ${CLANG_VERSION_DATA})
    foreach(line ${CLANG_VERSION_DATA})
        string(REGEX REPLACE "^#define CLANG_VERSION (${VERSION_REGEX})$" "\\1" CLANG_VERSION_STRING ${line})
        if("${CLANG_VERSION_STRING}" MATCHES "^${VERSION_REGEX}$")
            break()
        else()
            unset(CLANG_VERSION_STRING)
        endif()
    endforeach()
endif()
if(NOT DEFINED CLANG_VERSION_STRING)
    message(FATAL_ERROR "Unable to parse ClangVersion from ${CLANG_INCLUDE_DIRS}/clang/Basic/Version.inc: ")
endif()

set(CLANG_INCLUDE_DIRS_2 "${Clang_DIR}")
get_filename_component(CLANG_INCLUDE_DIRS_2 "${CLANG_INCLUDE_DIRS_2}" PATH)
get_filename_component(CLANG_INCLUDE_DIRS_2 "${CLANG_INCLUDE_DIRS_2}" PATH)
set(CLANG_INCLUDE_DIRS_2 "${CLANG_INCLUDE_DIRS_2}/clang/${CLANG_VERSION_STRING}/include")

# Library

add_library(autotest
    "${CMAKE_CURRENT_SOURCE_DIR}/args.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/engine.cpp"
)

add_library(autotest::autotest ALIAS autotest)

target_compile_features(autotest PUBLIC cxx_std_14)
target_compile_options(autotest PRIVATE
    $<$<CONFIG:Debug>:-O0 -g>
    $<$<CONFIG:Release>:-O1>
)

target_include_directories(autotest
PUBLIC
    $<BUILD_INTERFACE:${LLVM_INCLUDE_DIRS}>
    $<BUILD_INTERFACE:${CLANG_INCLUDE_DIRS}>
    $<BUILD_INTERFACE:${CLANG_INCLUDE_DIRS_2}>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(autotest PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/autotest)

install(TARGETS autotest
    EXPORT autotest-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${LLVM_INCLUDE_DIRS}
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/autotest
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT autotest-targets
    FILE autotest-targets.cmake
    NAMESPACE autotest::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/autotest
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/cmake/autotest-config-version.cmake
    VERSION ${AUTOTEST_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/autotest-config.cmake.in
    ${CMAKE_BINARY_DIR}/cmake/autotest-config.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

install(
    FILES
        ${CMAKE_BINARY_DIR}/cmake/autotest-config.cmake
        ${CMAKE_BINARY_DIR}/cmake/autotest-config-version.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

export(EXPORT autotest-targets
    FILE ${CMAKE_BINARY_DIR}/cmake/autotest-targets.cmake
    NAMESPACE autotest::
)

export(PACKAGE autotest)