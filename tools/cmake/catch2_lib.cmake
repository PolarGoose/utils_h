file(DOWNLOAD
  https://github.com/catchorg/Catch2/releases/download/v3.2.1/catch_amalgamated.hpp
  ${CMAKE_BINARY_DIR}/catch2/catch_amalgamated.hpp
  EXPECTED_MD5 9fcaf5b0150a144543bf5d8177a48ebd)
file(DOWNLOAD
  https://github.com/catchorg/Catch2/releases/download/v3.2.1/catch_amalgamated.cpp
  ${CMAKE_BINARY_DIR}/catch2/catch_amalgamated.cpp
  EXPECTED_MD5 aebd1ac63e23b0aeba85c6e3f0ce90a2)

add_library(catch2
  ${CMAKE_BINARY_DIR}/catch2/catch_amalgamated.hpp
  ${CMAKE_BINARY_DIR}/catch2/catch_amalgamated.cpp)
add_compiler_options_with_warnings_disabled(catch2)
target_include_directories(catch2 INTERFACE ${CMAKE_BINARY_DIR}/catch2)

function(add_catch2_lib target)
  target_link_libraries(${target} catch2)
endfunction()
