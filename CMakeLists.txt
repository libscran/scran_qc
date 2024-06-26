cmake_minimum_required(VERSION 3.14)

project(scran_quality_control
    VERSION 1.0.0
    DESCRIPTION "Simple per-cell quality control statistics"
    LANGUAGES CXX)

add_library(scran_quality_control INTERFACE)
target_compile_features(scran_quality_control INTERFACE cxx_std_17)
target_include_directories(scran_quality_control INTERFACE include/scran)

add_subdirectory(extern)
target_link_libraries(scran_quality_control INTERFACE tatami::tatami)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif()
endif()
