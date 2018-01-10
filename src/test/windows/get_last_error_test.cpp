#include "utils_h/string_conversion.h"
#include "utils_h/windows/get_last_error.h"
#include <doctest.h>

using namespace utils_h::windows;

TEST_CASE("get_last_error test") {
  SUBCASE("Testing get_last_error with a known last error") {
    SetLastError(ERROR_ACCESS_DENIED);
    REQUIRE(get_last_error() == L"ErrorCode=5(0x5). ErrorMessage='Access is denied. '");
  }

  SUBCASE("Testing get_last_error with no last error") {
    SetLastError(0);
    REQUIRE(get_last_error() == L"No last error information");
  }
}
