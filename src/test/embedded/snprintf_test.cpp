#include "utils_h/embedded/snprintf.h"
#include <doctest.h>

TEST_CASE("snprintf tests") {
  SUBCASE("Should work with normal input") {
    auto result = utils_h::embedded::snprintf<20>("Test %d", 123);
    REQUIRE(doctest::String(result->str().data()) == "Test 123");
    REQUIRE(result->str().size() == 8);
  }

  SUBCASE("Should return a null terminated string") {
    auto result = utils_h::embedded::snprintf<20>("Test %d", 123);
    REQUIRE(result->str().data()[8] == '\0');
  }

  SUBCASE("Should return truncated string in case the buffer is not large enough") {
    auto result = utils_h::embedded::snprintf<10>("Longer than %d chars", 10);
    REQUIRE(result->str().size() == 10);
    REQUIRE(doctest::String(result->str().data()) == "Longer tha");
  }

  SUBCASE("Should handle the string of the buffer size") {
    auto result = utils_h::embedded::snprintf<10>("10 chars..");
    REQUIRE(result->str().size() == 10);
    REQUIRE(doctest::String(result->str().data()) == "10 chars..");
  }

  SUBCASE("Should work with an empty string") {
    auto result = utils_h::embedded::snprintf<20>("");
    REQUIRE(doctest::String(result->str().data()) == "");
  }
}
