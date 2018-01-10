#include "utils/time_passed_method.h"
#include "utils_h/concurrency/observable_atomic_variable.h"

using namespace utils_h;
using namespace utils_h::concurrency;
using namespace std::chrono_literals;

class observable_atomic_variable_test : protected has_time_passed_method {
protected:
  void on_value_changed(const size_t& value) {
    m_historyOfEvents.push_back(value);
  }

  const size_t _initialValue = 1;
  observable_atomic_variable<size_t> m_observableAtomicVariable{
      _initialValue, [&](const auto& val) { on_value_changed(val); }};
  std::vector<size_t> m_historyOfEvents;
};

TEST_CASE_METHOD(observable_atomic_variable_test, "constructed correctly") {
  REQUIRE(m_observableAtomicVariable.get() == _initialValue);
}

TEST_CASE_METHOD(observable_atomic_variable_test,
                 "When value is set, it should be saved and a client should be "
                 "notified with an event") {
  const size_t numberOfElementsToSet = 100;

  for (size_t i = 0; i < numberOfElementsToSet; i++) {
    m_observableAtomicVariable.set(i);
    REQUIRE(m_observableAtomicVariable.get() == i);
  }

  REQUIRE(m_historyOfEvents.size() == numberOfElementsToSet);
  for (size_t i = 0; i < numberOfElementsToSet; i++) {
    REQUIRE(m_historyOfEvents[i] == i);
  }
}

TEST_CASE_METHOD(
    observable_atomic_variable_test,
    "Setting the same value should not cause an event to be sent") {
  m_observableAtomicVariable.set(_initialValue);
  REQUIRE(m_historyOfEvents.empty() == true);
}

TEST_CASE_METHOD(observable_atomic_variable_test,
                 "Several clients can call wait_for_value() method") {
  const auto timeout = 1s;
  const size_t valueToWaitFor = _initialValue + 1;

  const auto client1 = std::async(std::launch::async, [&] {
    m_observableAtomicVariable.wait_for_value(valueToWaitFor, timeout);
  });

  const auto client2 = std::async(std::launch::async, [&] {
    m_observableAtomicVariable.wait_for_value(valueToWaitFor, timeout);
  });

  m_observableAtomicVariable.set(valueToWaitFor);

  REQUIRE(client1.wait_for(timeout) != std::future_status::timeout);
  REQUIRE(client2.wait_for(timeout) != std::future_status::timeout);
}

TEST_CASE_METHOD(
    observable_atomic_variable_test,
    "Client should get an exception if waiting for a value is timed out") {
  const auto timeout = 300ms;

  REQUIRE_THROWS_AS(
      m_observableAtomicVariable.wait_for_value(_initialValue + 1, timeout),
      timed_out_exception);
  REQUIRE(time_passed() > timeout);
}

TEST_CASE_METHOD(observable_atomic_variable_test,
                 "If wait() method is interrupted, a client should be "
                 "unblocked with an exception") {
  const auto timeout = 2s;

  bool client1IsInterrupted{false};
  bool client2IsInterrupted{false};

  auto waitCall{[&] {
    try {
      m_observableAtomicVariable.wait_for_value(_initialValue + 1, timeout);
    } catch (const interrupted_exception&) {
      return true;
    }
    return false;
  }};

  std::thread client1{[&] { client1IsInterrupted = waitCall(); }};
  std::thread client2{[&] { client2IsInterrupted = waitCall(); }};

  std::this_thread::sleep_for(300ms);
  m_observableAtomicVariable.interrupt();
  client1.join();
  client2.join();

  REQUIRE(client1IsInterrupted == true);
  REQUIRE(client2IsInterrupted == true);
}

TEST_CASE_METHOD(
    observable_atomic_variable_test,
    "After interrupted, all wait calls should fail by throwing an exception") {
  m_observableAtomicVariable.interrupt();

  REQUIRE_THROWS_AS(m_observableAtomicVariable.wait_for_value(_initialValue),
                    interrupted_exception);
  REQUIRE_THROWS_AS(m_observableAtomicVariable.wait_for_value(_initialValue),
                    interrupted_exception);
}
