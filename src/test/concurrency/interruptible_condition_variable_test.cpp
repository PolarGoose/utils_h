#include "utils/time_passed_method.h"
#include "utils_h/concurrency/interruptible_condition_variable.h"

using namespace utils_h;
using namespace utils_h::concurrency;
using namespace std::chrono_literals;

class interruptible_condition_variable_test : protected has_time_passed_method {
protected:
  std::mutex _mutex;
  interruptible_condition_variable _cond_var;
};

TEST_CASE_METHOD(interruptible_condition_variable_test,
                 "wait() method should throw an exception on time out") {
  std::unique_lock lock{_mutex};

  REQUIRE_THROWS_AS(_cond_var.wait_for(
                        lock, [] { return false; }, 10ms),
                    timed_out_exception);
}

TEST_CASE_METHOD(interruptible_condition_variable_test,
                 "wait() method should throw an exception when interrupted") {
  auto client_func{[&] {
    try {
      std::unique_lock lock{_mutex};
      _cond_var.wait_for(
          lock, [] { return false; }, 1s);
    } catch (const interrupted_exception&) {
      return true;
    }
    return false;
  }};

  auto client1_is_interrupted{false};
  auto client2_is_interrupted{false};

  std::thread client1{[&] { client1_is_interrupted = client_func(); }};
  std::thread client2{[&] { client2_is_interrupted = client_func(); }};

  // sleep long enough to let client1 and client2 threads to get blocked on
  // _cond_var.wait_for() call
  std::this_thread::sleep_for(300ms);

  // interrupt client1 and client2
  {
    std::unique_lock lock{_mutex};
    _cond_var.interrupt(lock);
  }

  // wait until client1 and client2 threads get notified about the interruption
  // and finished
  client1.join();
  client2.join();

  REQUIRE(client1_is_interrupted == true);
  REQUIRE(client2_is_interrupted == true);
}

TEST_CASE_METHOD(
    interruptible_condition_variable_test,
    "After being interrupted, all calls to wait() should throw an exception") {
  std::unique_lock lock{_mutex};
  _cond_var.interrupt(lock);

  REQUIRE_THROWS_AS(_cond_var.wait_for(lock, [] { return false; }),
                    interrupted_exception);
}

TEST_CASE_METHOD(
    interruptible_condition_variable_test,
    "wait() method should unblock when the condition becomes true") {
  auto value_to_wait_for{false};

  std::thread client{[&] {
    std::unique_lock lock{_mutex};
    _cond_var.wait_for(lock, [&] { return value_to_wait_for; });
  }};

  // wait long enough to let client thread to get blocked on wait_for() method
  std::this_thread::sleep_for(300ms);

  // set the wait condition to true and wake up the client
  {
    std::unique_lock lock{_mutex};
    value_to_wait_for = true;
    _cond_var.notify_all();
  }

  client.join();
}
