#include "test/test_utils/time_passed.h"
#include "utils_h/concurrency/observable_atomic_variable.h"
#include "utils_h/concurrency/one_shot_timer.h"
#include <doctest.h>

using namespace utils_h;
using namespace utils_h::concurrency;
using namespace std::chrono_literals;

TEST_CASE("one_shot_timer_test tests") {
  const auto& time = 300ms;
  const auto& start_time = std::chrono::steady_clock::now();
  observable_atomic_variable<bool> timer_callback_was_called{false};
  auto timer = std::make_unique<one_shot_timer>(time, [&] { timer_callback_was_called.set(true); });
  const auto& wait_for_timer_callback_is_called = [&] { timer_callback_was_called.wait_for_value(true, time * 2); };

  SUBCASE("Should be started when it is constructed") {
    REQUIRE_NOTHROW(wait_for_timer_callback_is_called());
    REQUIRE(time_passed(start_time) > time);
  }

  SUBCASE("Should be stopped when it is destructed") {
    timer.reset();
    REQUIRE_THROWS_AS(wait_for_timer_callback_is_called(), timed_out_exception);
  }
}
