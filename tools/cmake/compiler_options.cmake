if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  message(FATAL_ERROR "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

function(_add_compiler_options target)
  target_compile_features(${target} PRIVATE cxx_std_20)
  target_compile_options(${target} PRIVATE /utf-8 /external:anglebrackets /external:W0)
  target_compile_definitions(${target} PRIVATE _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
endfunction()

function(add_compiler_options_with_warnings target)
  _add_compiler_options(${target})
  target_compile_options(${target} PRIVATE /WX /W4)
endfunction()

function(add_compiler_options_with_warnings_disabled target)
  _add_compiler_options(${target})
  target_compile_options(${target} PRIVATE /W0)
endfunction()
