#include "utils_h/windows/scoped_handle.h"
#include <doctest.h>

TEST_CASE("scoped_handle tests") {
  SUBCASE("Create handle") {
    const auto& handle = utils_h::windows::scoped_handle{INVALID_HANDLE_VALUE};
    REQUIRE(handle.get() == INVALID_HANDLE_VALUE);
  }
}
