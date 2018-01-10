#include "test/test_utils/time_passed.h"
#include "utils_h/concurrency/interruptible_condition_variable.h"
#include <doctest.h>

using namespace utils_h;
using namespace utils_h::concurrency;
using namespace std::chrono_literals;

TEST_CASE("interruptible_condition_variable tests") {
  std::mutex m;
  interruptible_condition_variable cond;

  SUBCASE("wait() method should throw an exception on time out") {
    std::unique_lock lock{m};
    REQUIRE_THROWS_AS(cond.wait_for(
                          lock, [] { return false; }, 10ms),
                      timed_out_exception);
  }

  SUBCASE("wait() method should throw an exception when interrupted") {
    auto client_func{[&] {
      try {
        std::unique_lock lock{m};
        cond.wait_for(
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
      std::unique_lock lock{m};
      cond.interrupt(lock);
    }

    // wait until client1 and client2 threads get notified about the interruption
    // and finished
    client1.join();
    client2.join();

    REQUIRE(client1_is_interrupted == true);
    REQUIRE(client2_is_interrupted == true);
  }

  SUBCASE("After being interrupted, all calls to wait() should throw an exception") {
    std::unique_lock lock{m};
    cond.interrupt(lock);
    REQUIRE_THROWS_AS(cond.wait_for(lock, [] { return false; }), interrupted_exception);
  }

  SUBCASE("wait() method should unblock when the condition becomes true") {
    auto value_to_wait_for{false};

    std::thread client{[&] {
      std::unique_lock lock{m};
      cond.wait_for(lock, [&] { return value_to_wait_for; });
    }};

    // wait long enough to let client thread to get blocked on wait_for() method
    std::this_thread::sleep_for(300ms);

    // set the wait condition to true and wake up the client
    {
      std::unique_lock lock{m};
      value_to_wait_for = true;
      cond.notify_all();
    }

    client.join();
  }
}
