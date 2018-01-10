#include "utils_h/concurrency/observable_atomic_variable.h"
#include "utils_h/concurrency/watchdog.h"

using namespace utils_h;
using namespace utils_h::concurrency;
using namespace std::chrono_literals;

class watchdog_test {
protected:
  watchdog_test()
      : _watchdog{std::in_place, _watchdog_timeout,
                  [&] { _is_watchdog_timed_out.set(true); }} {}

  void reset_watchdog_for_some_time() {
    for (auto i = 0; i < 5; i++) {
      std::this_thread::sleep_for(_watchdog_timeout / 2);
      _watchdog->Reset();
    }
  }

  void wait_and_check_that_watchdog_triggered() {
    return _is_watchdog_timed_out.wait_for_value(true, _watchdog_timeout * 2);
  }

  void wait_and_check_that_watchdog_did_not_trigger() {
    REQUIRE_THROWS_AS(wait_and_check_that_watchdog_triggered(),
                      timed_out_exception);
  }

  const std::chrono::milliseconds _watchdog_timeout{300};
  observable_atomic_variable<bool> _is_watchdog_timed_out{false};
  std::optional<watchdog> _watchdog;
};

TEST_CASE_METHOD(
    watchdog_test,
    "Watchdog starts when it is constructed and timeouts If not reset") {
  wait_and_check_that_watchdog_triggered();
}

TEST_CASE_METHOD(watchdog_test, "Watchdog can be reset and fires timed out "
                                "event when stopped being reset") {
  reset_watchdog_for_some_time();

  REQUIRE(_is_watchdog_timed_out.get() == false);

  wait_and_check_that_watchdog_triggered();
}

TEST_CASE_METHOD(watchdog_test, "Watchdog should be stopped on destruction") {
  _watchdog.reset();
  wait_and_check_that_watchdog_did_not_trigger();
}
