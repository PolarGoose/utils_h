#include "utils_h/logging/log_stream.h"

using namespace utils_h;
using namespace utils_h::logging;

struct log_stream_test {
  void log_function(const std::string& message) { m_logMessage = message; }

  std::string m_logMessage;
};

TEST_CASE_METHOD(log_stream_test, "Works correctly") {
  log_stream{[&](const std::string& str) { log_function(str); }}.get_stream()
      << "a"
      << "b" << 10;

  REQUIRE(m_logMessage == "ab10");
}
