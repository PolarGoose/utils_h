#include "utils_h/concurrency/observable_atomic_variable.h"
#include "utils_h/concurrency/watchdog.h"
#include <doctest.h>

using namespace utils_h;
using namespace utils_h::concurrency;
using namespace std::chrono_literals;

TEST_CASE("watchdog tests") {
  const auto& timeout = 300ms;
  observable_atomic_variable<bool> watchdog_timed_out(false);
  std::optional<watchdog> watchdog(std::in_place, timeout, [&] { watchdog_timed_out.set(true); });

  const auto& reset_watchdog_for_some_time = [&] {
    for (auto i = 0; i < 5; i++) {
      std::this_thread::sleep_for(timeout / 2);
      watchdog->Reset();
    }
  };

  const auto& wait_and_check_that_watchdog_triggered = [&] { watchdog_timed_out.wait_for_value(true, timeout * 2); };

  const auto& wait_and_check_that_watchdog_did_not_trigger = [&] {
    REQUIRE_THROWS_AS(wait_and_check_that_watchdog_triggered(), timed_out_exception);
  };

  SUBCASE("Should start when it is constructed and timeout If not reset") { wait_and_check_that_watchdog_triggered(); }

  SUBCASE("Should be resettable and fire timed out event when stopped being reset") {
    reset_watchdog_for_some_time();
    REQUIRE(watchdog_timed_out.get() == false);
    wait_and_check_that_watchdog_triggered();
  }

  SUBCASE("Watchdog should be stopped on destruction") {
    watchdog.reset();
    wait_and_check_that_watchdog_did_not_trigger();
  }
}
