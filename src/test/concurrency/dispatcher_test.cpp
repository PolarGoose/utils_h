#include "utils_h/concurrency/dispatcher.h"
#include <doctest.h>

using namespace utils_h;
using namespace utils_h::concurrency;

static void wait_while_all_tasks_are_executed(dispatcher& d) {
  d.dispatch_and_wait([] {});
}

TEST_CASE("Dispatcher tests") {
  auto unhandled_exception_handler_was_called = false;
  dispatcher d([&](auto& /* exception */) { unhandled_exception_handler_was_called = true; });

  SUBCASE("dispatch_and_wait function should work with various function types") {
    d.dispatch_and_wait([&] {});
    REQUIRE(d.dispatch_and_wait([] { return std::string{"string"}; }) == "string");
    REQUIRE(d.dispatch_and_wait([] { return 1; }) == 1);
  }

  SUBCASE("An unhandled exception handler should be called in case task throws an exception") {
    d.dispatch([] { throw std::runtime_error("error"); });
    wait_while_all_tasks_are_executed(d);
    REQUIRE(unhandled_exception_handler_was_called == true);
  }

  SUBCASE("dispatch_and_wait should propagate an exception to the caller") {
    REQUIRE_THROWS_AS(d.dispatch_and_wait([] { throw std::runtime_error("error"); }), std::runtime_error);
  }

  SUBCASE("is_dispatcher_thread should return true if it is called inside a dispatcher thread") {
    REQUIRE(d.is_dispatcher_thread() == false);
    REQUIRE(d.dispatch_and_wait([&] { return d.is_dispatcher_thread(); }) == true);
  }

  SUBCASE("Should execute tasks in the order in which they were dispatched") {
    // Arrange
    std::vector<size_t> results;
    for (size_t i = 0; i < 100; i++) {
      d.dispatch([i, &results] { results.push_back(i); });
    }

    // Act
    wait_while_all_tasks_are_executed(d);

    // Assert
    REQUIRE(results.size() == 100);
    for (size_t i = 0; i < 100; i++) {
      REQUIRE(results[i] == i);
    }
  }
}
