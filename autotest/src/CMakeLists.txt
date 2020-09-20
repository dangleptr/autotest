add_library(autotest)

set_target_properties(autotest PROPERTIES LINKER_LANGUAGE CXX)

add_library(autotest::autotest ALIAS autotest)

target_compile_features(autotest PUBLIC cxx_std_17)
target_compile_options(autotest PRIVATE
    $<$<CONFIG:Debug>:-Og -g>
    $<$<CONFIG:Release>:-O2>
)

target_include_directories(autotest
PUBLIC
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
