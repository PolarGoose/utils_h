file(DOWNLOAD
  https://github.com/doctest/doctest/releases/download/v2.4.11/doctest.h
  ${CMAKE_BINARY_DIR}/doctest/doctest.h
  EXPECTED_MD5 0b7fbd89a158063beecba78eb8400fad)

function(add_doctest_lib target)
  target_include_directories(${target} PRIVATE ${CMAKE_BINARY_DIR}/doctest)
endfunction()
