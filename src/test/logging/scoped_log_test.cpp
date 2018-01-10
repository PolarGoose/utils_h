#include "utils_h/logging/scoped_log.h"

using namespace utils_h;
using namespace utils_h::logging;
using namespace Catch::Matchers;

class scoped_log_test {
protected:
  void log_function(const std::string& message) { _log_message = message; }

  std::string _log_message;
};

TEST_CASE_METHOD(scoped_log_test,
                 "Should write correct information to the log") {
  {
    scoped_log scopedLog{"LineInfo ",
                         [&](const std::string& msg) { log_function(msg); }};
    REQUIRE(_log_message == "LineInfo Enter");
  }
  REQUIRE_THAT(_log_message, Matches(R"(LineInfo Exit\(\d+ms\))"));
}

TEST_CASE_METHOD(scoped_log_test, "Should detect exceptions") {
  try {
    scoped_log scopedLog{"LineInfo ",
                         [&](const std::string& msg) { log_function(msg); }};
    REQUIRE(_log_message == "LineInfo Enter");
    throw std::runtime_error("error");
  } catch (const std::runtime_error&) {
    REQUIRE_THAT(_log_message, Matches(R"(LineInfo Exception\(\d+ms\))"));
  }
}
