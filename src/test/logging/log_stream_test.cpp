#include "utils_h/logging/log_stream.h"
#include <doctest.h>

using namespace utils_h;
using namespace utils_h::logging;

TEST_CASE("log_stream tests") {
  SUBCASE("Should log the message correctly") {
    std::string logMsg;
    const auto& logFunction = [&](const std::string& msg) { logMsg = msg; };
    log_stream(logFunction).get_stream() << "a"
                                         << "b" << 10;
    REQUIRE(logMsg == "ab10");
  }
}
