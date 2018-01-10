#include "utils_h/concurrency/dispatcher.h"

using namespace utils_h;
using namespace utils_h::concurrency;

class dispatcher_test {
protected:
  void wait_while_all_tasks_are_executed() {
    _dispatcher.dispatch_and_wait([] {});
  }

  void unhandled_exception_handler() {
    _unhandled_exception_handler_was_called = true;
  }

  dispatcher _dispatcher{
      [&](auto& /* exception */) { unhandled_exception_handler(); }};
  bool _unhandled_exception_handler_was_called{false};
};

TEST_CASE_METHOD(
    dispatcher_test,
    "dispatch_and_wait function should work with functions which return void") {
  _dispatcher.dispatch_and_wait([&] {});
}

TEST_CASE_METHOD(dispatcher_test, "dispatch_and_wait function should work with "
                                  "functions which return value") {
  REQUIRE(_dispatcher.dispatch_and_wait([] { return std::string{"string"}; }) ==
          "string");
  REQUIRE(_dispatcher.dispatch_and_wait([] { return 1; }) == 1);
}

TEST_CASE_METHOD(dispatcher_test,
                 "If a dispatched task throws an exception, an unhandled "
                 "exception handler should be called") {
  _dispatcher.dispatch([] { throw std::runtime_error("error"); });
  wait_while_all_tasks_are_executed();

  REQUIRE(_unhandled_exception_handler_was_called == true);
}

TEST_CASE_METHOD(dispatcher_test, "Exception during dispatch_and_wait function "
                                  "call should be propagated to the caller") {
  REQUIRE_THROWS_AS(
      _dispatcher.dispatch_and_wait([] { throw std::runtime_error("error"); }),
      std::runtime_error);
}

TEST_CASE_METHOD(dispatcher_test,
                 "is_dispatcher_thread function returns true if it is called "
                 "inside a dispatcher thread") {
  REQUIRE(_dispatcher.is_dispatcher_thread() == false);
  REQUIRE(_dispatcher.dispatch_and_wait(
              [&] { return _dispatcher.is_dispatcher_thread(); }) == true);
}

TEST_CASE_METHOD(
    dispatcher_test,
    "Tasks should be executed in the order in which they were dispatched") {
  std::vector<size_t> results;

  for (size_t i = 0; i < 100; i++) {
    _dispatcher.dispatch([i, &results] { results.push_back(i); });
  }

  wait_while_all_tasks_are_executed();
  REQUIRE(results.size() == 100);

  for (size_t i = 0; i < 100; i++) {
    REQUIRE(results[i] == i);
  }
}
