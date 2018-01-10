#include "utils_h/app_exception.h"
#include <doctest.h>

using namespace utils_h;

TEST_CASE("app_exception tests") {
  SUBCASE("Message formatting should work. Case #1") {
    try {
      app_exception ex;
      throw app_exception() << "Error occurred: " << 123;

    } catch (const app_exception& ex) {
      REQUIRE(doctest::String(ex.what()) == "Error occurred: 123");
    }
  }

  SUBCASE("Message formatting should work. Case #2") {
    try {
      throw app_exception("Error occurred: ") << 123;
    } catch (const app_exception& ex) {
      REQUIRE(doctest::String(ex.what()) == "Error occurred: 123");
    }
  }
}

TEST_CASE("app_wide_exception tests") {
  SUBCASE("Message formatting should work. Case #1") {
    try {
      throw app_wide_exception() << L"Error occurred: " << 123;
    } catch (const app_wide_exception& ex) {
      REQUIRE(doctest::String(ex.what()) == "Error occurred: 123");
      REQUIRE(std::wstring(ex.wide_what()) == L"Error occurred: 123");
    }
  }

  SUBCASE("Message formatting should work 2. Case #2") {
    try {
      throw app_wide_exception(L"Error occurred: ") << 123;
    } catch (const app_wide_exception& ex) {
      REQUIRE(doctest::String(ex.what()) == "Error occurred: 123");
      REQUIRE(std::wstring(ex.wide_what()) == L"Error occurred: 123");
    }
  }
}
