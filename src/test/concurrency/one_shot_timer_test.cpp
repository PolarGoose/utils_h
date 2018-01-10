#include "utils/time_passed_method.h"
#include "utils_h/concurrency/observable_atomic_variable.h"
#include "utils_h/concurrency/one_shot_timer.h"

using namespace utils_h;
using namespace utils_h::concurrency;
using namespace std::chrono_literals;

class one_shot_timer_test : protected has_time_passed_method {
protected:
  void wait_for_timer_callback_is_called() {
    _timer_callback_was_called.wait_for_value(true, _time * 2);
  }

  const std::chrono::milliseconds _time{300};
  observable_atomic_variable<bool> _timer_callback_was_called{false};
  std::unique_ptr<one_shot_timer> _timer{std::make_unique<one_shot_timer>(
      _time, [&] { _timer_callback_was_called.set(true); })};
};

TEST_CASE_METHOD(one_shot_timer_test,
                 "Timer should be started when it is constructed") {
  REQUIRE_NOTHROW(wait_for_timer_callback_is_called());
  REQUIRE(time_passed() > _time);
}

TEST_CASE_METHOD(one_shot_timer_test,
                 "Timer should be stopped when it is destructed") {
  _timer.reset();
  REQUIRE_THROWS_AS(wait_for_timer_callback_is_called(), timed_out_exception);
}
