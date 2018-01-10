#include "utils_h/logging/scoped_log.h"
#include <doctest.h>

using namespace utils_h;
using namespace utils_h::logging;
using namespace doctest;

TEST_CASE("scoped_log tests") {
  std::string log_message;
  const auto& log_function = [&](const std::string& msg) { log_message = msg; };

  SUBCASE("Should write correct information to the log") {
    {
      scoped_log scopedLog("LineInfo ", log_function);
      REQUIRE(log_message == "LineInfo Enter");
    }
    REQUIRE(log_message.c_str() == Contains("LineInfo Exit"));
  }
}
