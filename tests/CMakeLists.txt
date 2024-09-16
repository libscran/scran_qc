include(FetchContent)
FetchContent_Declare(
  scran_tests 
  GIT_REPOSITORY https://github.com/libscran/scran_tests
  GIT_TAG master
)
FetchContent_MakeAvailable(scran_tests)

option(CODE_COVERAGE "Enable coverage testing" OFF)
set(DO_CODE_COVERAGE OFF)
if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(DO_CODE_COVERAGE ON)
endif()

include(GoogleTest)

macro(decorate_executable target)
    target_link_libraries(
        ${target}
        scran_qc
        scran_tests
    )

    target_compile_options(${target} PRIVATE -Wall -Werror -Wpedantic -Wextra)

    if(DO_CODE_COVERAGE)
        target_compile_options(${target} PRIVATE -O0 -g --coverage)
        target_link_options(${target} PRIVATE --coverage)
    endif()

    gtest_discover_tests(${target})
endmacro()

add_executable(
    libtest 
    src/per_cell_qc_metrics.cpp
    src/find_median_mad.cpp
    src/choose_filter_thresholds.cpp
    src/rna_quality_control.cpp
    src/adt_quality_control.cpp
    src/crispr_quality_control.cpp
    src/format_filters.cpp
)
decorate_executable(libtest)

add_executable(
    cuspartest 
    src/per_cell_qc_metrics.cpp
)
decorate_executable(cuspartest)
target_compile_definitions(cuspartest PRIVATE SUBPAR_CUSTOM_PARALLELIZE_RANGE=::subpar::test_parallelize_range)
