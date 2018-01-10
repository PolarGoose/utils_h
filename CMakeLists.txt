cmake_minimum_required (VERSION 3.28)
project(UtilsHLibrary LANGUAGES CXX)

# Download Doctest
file(DOWNLOAD
  https://github.com/doctest/doctest/releases/download/v2.4.11/doctest.h
  ${CMAKE_BINARY_DIR}/doctest/doctest.h
  EXPECTED_MD5 0b7fbd89a158063beecba78eb8400fad)

file(GLOB_RECURSE src_files CONFIGURE_DEPENDS "src/*.h" "src/*.cpp")
add_executable(utils_h_test ${src_files})
target_include_directories(utils_h_test PRIVATE "src" ${CMAKE_BINARY_DIR}/doctest)
target_compile_features(utils_h_test PRIVATE cxx_std_23)
target_compile_options(utils_h_test PRIVATE /utf-8 /external:anglebrackets /external:W0 /WX /W4 /analyze)
target_compile_definitions(utils_h_test PRIVATE _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
target_precompile_headers(utils_h_test PRIVATE src/test/precompiled_header.h)

# Download and configure clang-format
file(DOWNLOAD
  https://github.com/muttleyxd/clang-tools-static-binaries/releases/download/master-f7f02c1d/clang-format-17_windows-amd64.exe
  ${CMAKE_BINARY_DIR}/clang-format.exe
  EXPECTED_MD5 459e1bec4b16540b098ac7bd893d5781)

add_custom_target(clangformat
  COMMAND
    ${CMAKE_BINARY_DIR}/clang-format.exe -style=file -i ${src_files}
  WORKING_DIRECTORY
    ${CMAKE_SOURCE_DIR}
  COMMENT
    "Formatting source files with clang-format")
add_dependencies(utils_h_test clangformat)

