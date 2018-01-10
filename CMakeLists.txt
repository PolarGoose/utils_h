cmake_minimum_required (VERSION 3.20)
project(UtilsHLibrary LANGUAGES CXX)

include(tools/cmake/compiler_options.cmake)
include(tools/cmake/clang_format.cmake)
include(tools/cmake/doctest_lib.cmake)

file(GLOB_RECURSE src_files CONFIGURE_DEPENDS "src/*.h" "src/*.cpp")
add_executable(utils_h_test ${src_files})
target_include_directories(utils_h_test PRIVATE "src")
target_precompile_headers(utils_h_test PRIVATE src/test/precompiled_header.h)
add_clang_format(utils_h_test)
add_doctest_lib(utils_h_test)
add_compiler_options_with_warnings(utils_h_test)
