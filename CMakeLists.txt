cmake_minimum_required(VERSION 3.14)

project(scran_qc
    VERSION 1.0.0
    DESCRIPTION "Simple per-cell quality control statistics"
    LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Library
add_library(scran_qc INTERFACE)
add_library(libscran::scran_qc ALIAS scran_qc)

target_include_directories(scran_qc INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/scran_qc>)
target_compile_features(scran_qc INTERFACE cxx_std_17)

# Dependencies
option(SCRAN_QC_FETCH_EXTERN "Automatically fetch scran_qc's external dependencies." ON)
if(SCRAN_QC_FETCH_EXTERN)
    add_subdirectory(extern)
else()
    find_package(tatami_tatami 3.0.0 CONFIG REQUIRED)
    find_package(tatami_tatami_stats 1.0.0 CONFIG REQUIRED)
endif()

target_link_libraries(scran_qc INTERFACE tatami::tatami tatami::tatami_stats)

# Tests
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    option(SCRAN_QC_TESTS "Build scran_qc's test suite." ON)
else()
    option(SCRAN_QC_TESTS "Build scran_qc's test suite." OFF)
endif()

if(SCRAN_QC_TESTS)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif() 
endif()

# Install
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/scran_qc)

install(TARGETS scran_qc
    EXPORT scran_qcTargets)

install(EXPORT scran_qcTargets
    FILE libscran_scran_qcTargets.cmake
    NAMESPACE libscran::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libscran_scran_qc)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/libscran_scran_qcConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libscran_scran_qc)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/libscran_scran_qcConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libscran_scran_qcConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/libscran_scran_qcConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libscran_scran_qc)
