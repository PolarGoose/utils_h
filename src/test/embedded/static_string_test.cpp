#include "utils_h/embedded/static_string.h"
#include <doctest.h>

using namespace utils_h::embedded;

TEST_CASE("StaticString tests") {
  SUBCASE("Construction with an empty string") {
    static_string<300> s("");
    REQUIRE(s.str() == "");
    REQUIRE(s.str().size() == 0);
    REQUIRE(s.str().data()[0] == '\0');
  }

  SUBCASE("Construction with a short string") {
    std::string input = "Hello";
    static_string<300> s(input);
    REQUIRE(s.str() == input);
    REQUIRE(s.str().size() == 5);
    REQUIRE(s.str().data()[5] == '\0');
  }

  SUBCASE("Construct with a string exactly 300 characters long") {
    std::string input(299, 'x');
    static_string<300> s(input);
    REQUIRE(s.str() == input);
    REQUIRE(s.str().data()[299] == '\0');
  }

  SUBCASE("Construct with a string longer than 300 characters") {
    std::string input(350, 'y');
    static_string<300> s(input);
    REQUIRE(s.str() == std::string_view(input.data(), 299));
    REQUIRE(s.str().data()[299] == '\0');
  }
}
