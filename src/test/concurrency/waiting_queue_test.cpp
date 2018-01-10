#include "test/test_utils/time_passed.h"
#include "utils_h/concurrency/waiting_queue.h"
#include <doctest.h>

using namespace utils_h;
using namespace utils_h::concurrency;
using namespace std::chrono_literals;

static void fill_queue(waiting_queue<size_t>& bounded_queue) {
  for (size_t i = 0; i < bounded_queue.max_size(); i++) {
    bounded_queue.blocking_push(i);
  }
}

TEST_CASE("waiting_queue tests") {
  const size_t boundary = 10;
  const auto& start_time = std::chrono::steady_clock::now();
  waiting_queue<size_t> bounded_waiting_queue(boundary);
  waiting_queue<size_t> unbounded_waiting_queue;

  SUBCASE("Unbounded queue should have size equal to size_t::max") {
    REQUIRE(unbounded_waiting_queue.max_size() == std::numeric_limits<size_t>::max());
  }

  SUBCASE("Queue should be empty by default") { REQUIRE(unbounded_waiting_queue.empty() == true); }

  SUBCASE("Unbounded queue should fit unlimited amount of elements") {
    const size_t numberOfElements = 10 * 1000;

    for (size_t i = 0; i < numberOfElements; i++) {
      unbounded_waiting_queue.blocking_push(i);
    }
    REQUIRE(unbounded_waiting_queue.size() == numberOfElements);

    for (size_t i = 0; i < numberOfElements; i++) {
      REQUIRE(unbounded_waiting_queue.blocking_pop() == i);
    }
    REQUIRE(unbounded_waiting_queue.empty() == true);
  }

  SUBCASE("Reading from an empty queue should throw an exception after a time out") {
    const auto timeout = 300ms;
    REQUIRE_THROWS_AS(unbounded_waiting_queue.blocking_pop(timeout), timed_out_exception);
    REQUIRE(time_passed(start_time) > timeout);
  }

  SUBCASE("Should unblock the reader when an element is added to the queue") {
    const auto timeout = 1s;
    const auto time_to_wait_before_send = timeout / 2;
    const auto data = 1;

    std::thread sender([&] {
      std::this_thread::sleep_for(time_to_wait_before_send);
      unbounded_waiting_queue.blocking_push(data);
    });

    REQUIRE(unbounded_waiting_queue.blocking_pop(timeout) == data);
    REQUIRE(time_passed(start_time) > time_to_wait_before_send);
    sender.join();
  }

  SUBCASE("Should block the sender when the bounded queue is full after a time out") {
    fill_queue(bounded_waiting_queue);

    REQUIRE_THROWS_AS(bounded_waiting_queue.blocking_push(1, 100ms), timed_out_exception);
    REQUIRE(time_passed(start_time) > 100ms);
  }

  SUBCASE("Should unblock the sender when the bounded queue has free space") {
    fill_queue(bounded_waiting_queue);
    const auto timeout = 1s;
    const auto time_to_wait_before_read = timeout / 2;

    std::thread receiver([&] {
      std::this_thread::sleep_for(time_to_wait_before_read);
      unbounded_waiting_queue.blocking_pop();
    });

    unbounded_waiting_queue.blocking_push(1, timeout);
    REQUIRE(time_passed(start_time) > time_to_wait_before_read);
    receiver.join();
  }

  SUBCASE("Receive operations should be interruptible") {
    bool receiver1_interrupted{false};
    bool receiver2_interrupted{false};

    auto receiveCall{[&] {
      try {
        unbounded_waiting_queue.blocking_pop();
      } catch (const interrupted_exception&) {
        return true;
      }
      return false;
    }};

    std::thread receiver1{[&] { receiver1_interrupted = receiveCall(); }};
    std::thread receiver2{[&] { receiver2_interrupted = receiveCall(); }};

    // wait until receiver threads are blocked on reading from the queue
    std::this_thread::sleep_for(300ms);

    unbounded_waiting_queue.interrupt();

    // wait until receivers get interrupted with an exception
    receiver1.join();
    receiver2.join();

    REQUIRE(receiver1_interrupted == true);
    REQUIRE(receiver2_interrupted == true);
  }

  SUBCASE("Send operations should be interruptible") {
    fill_queue(bounded_waiting_queue);

    auto sender1_interrupted{false};
    auto sender2_interrupted{false};

    auto sendCall{[&] {
      try {
        bounded_waiting_queue.blocking_push(1);
      } catch (const interrupted_exception&) {
        return true;
      }
      return false;
    }};

    std::thread sender1{[&] { sender1_interrupted = sendCall(); }};
    std::thread sender2{[&] { sender2_interrupted = sendCall(); }};

    // wait until sender threads are blocked on adding to the queue
    std::this_thread::sleep_for(300ms);

    bounded_waiting_queue.interrupt();

    // wait until senders get interrupted with an exception
    sender1.join();
    sender2.join();

    REQUIRE(sender1_interrupted == true);
    REQUIRE(sender2_interrupted == true);
  }

  SUBCASE("Should always throw an exception after the queue is interrupted") {
    unbounded_waiting_queue.interrupt();

    REQUIRE_THROWS_AS(unbounded_waiting_queue.blocking_push(1), interrupted_exception);
    REQUIRE_THROWS_AS(unbounded_waiting_queue.blocking_pop(), interrupted_exception);
  }
}
