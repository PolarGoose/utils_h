#include "test/test_utils/time_passed.h"
#include "utils_h/concurrency/observable_atomic_variable.h"
#include <doctest.h>

using namespace utils_h;
using namespace utils_h::concurrency;
using namespace std::chrono_literals;

TEST_CASE("observable_atomic_variable tests") {
  const size_t initValue = 1;
  std::vector<size_t> historyOfEvents;
  observable_atomic_variable<size_t> var(initValue, [&](const auto& val) { historyOfEvents.push_back(val); });
  const auto& startTime = std::chrono::steady_clock::now();

  SUBCASE("Constructed correctly") { REQUIRE(var.get() == initValue); }

  SUBCASE("Should save the new value and notify the client") {
    const size_t numberOfElementsToSet = 100;

    for (size_t i = 0; i < numberOfElementsToSet; i++) {
      var.set(i);
      REQUIRE(var.get() == i);
    }

    REQUIRE(historyOfEvents.size() == numberOfElementsToSet);
    for (size_t i = 0; i < numberOfElementsToSet; i++) {
      REQUIRE(historyOfEvents[i] == i);
    }
  }

  SUBCASE("Should not notify the client if the same value is set") {
    var.set(initValue);
    REQUIRE(historyOfEvents.empty() == true);
  }

  SUBCASE("Several clients can call wait_for_value() method") {
    const auto timeout = 1s;
    const size_t valueToWaitFor = initValue + 1;

    const auto client1 = std::async(std::launch::async, [&] { var.wait_for_value(valueToWaitFor, timeout); });

    const auto client2 = std::async(std::launch::async, [&] { var.wait_for_value(valueToWaitFor, timeout); });

    var.set(valueToWaitFor);

    REQUIRE(client1.wait_for(timeout) != std::future_status::timeout);
    REQUIRE(client2.wait_for(timeout) != std::future_status::timeout);
  }

  SUBCASE("wait_for_value() should throw on time out") {
    const auto timeout = 300ms;
    REQUIRE_THROWS_AS(var.wait_for_value(initValue + 1, timeout), timed_out_exception);
    REQUIRE(time_passed(startTime) > timeout);
  }

  SUBCASE("Should unblock the client if wait() method is interrupted") {
    const auto timeout = 2s;

    bool client1IsInterrupted{false};
    bool client2IsInterrupted{false};

    auto waitCall{[&] {
      try {
        var.wait_for_value(initValue + 1, timeout);
      } catch (const interrupted_exception&) {
        return true;
      }
      return false;
    }};

    std::thread client1{[&] { client1IsInterrupted = waitCall(); }};
    std::thread client2{[&] { client2IsInterrupted = waitCall(); }};

    std::this_thread::sleep_for(300ms);
    var.interrupt();
    client1.join();
    client2.join();

    REQUIRE(client1IsInterrupted == true);
    REQUIRE(client2IsInterrupted == true);
  }

  SUBCASE("Should throw an exception after being interrupted") {
    var.interrupt();

    REQUIRE_THROWS_AS(var.wait_for_value(initValue), interrupted_exception);
    REQUIRE_THROWS_AS(var.wait_for_value(initValue), interrupted_exception);
  }
}
