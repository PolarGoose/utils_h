#include "utils_h/string_conversion.h"
#include <doctest.h>

using namespace utils_h;

TEST_CASE("String conversion tests") {
  SUBCASE("Should convert utf8 to utf16 and back") {
    std::string utf8_str = "車B1234 こんにちは";
    std::wstring utf16_str = L"車B1234 こんにちは";

    REQUIRE(to_utf16(utf8_str) == utf16_str);
    REQUIRE(to_utf8(utf16_str) == utf8_str);
  }
}
