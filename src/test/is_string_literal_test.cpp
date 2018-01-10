#include "utils_h/type_traits.h"

using namespace utils_h;

TEST_CASE("is_string_literal - Should detect non string literals") {
  const char* msg1 = "some string";
  const char msg2[] = "some string";
  const char msg3[]{'s', 'o', 'm', 'e', '\0'};
  auto msg4 = "some string";
  const std::string msg5 = "some string";

  static_assert(!is_string_literal<decltype(msg1)>::value);
  static_assert(!is_string_literal<decltype(msg2)>::value);
  static_assert(!is_string_literal<decltype(msg3)>::value);
  static_assert(!is_string_literal<decltype(msg4)>::value);
  static_assert(!is_string_literal<decltype(msg5)>::value);
}

TEST_CASE("is_string_literal - Should detect string literals") {
  const auto& msg1 = "some string";

  static_assert(is_string_literal<decltype(msg1)>::value);
  static_assert(is_string_literal<decltype("some string")>::value);
}
