#include "utils/time_passed_method.h"
#include "utils_h/concurrency/waiting_queue.h"

using namespace utils_h;
using namespace utils_h::concurrency;
using namespace std::chrono_literals;

class waiting_queue_test : protected has_time_passed_method {
protected:
  void fill_waiting_queue() {
    for (size_t i = 0; i < m_boundary; i++) {
      m_bounded_waiting_queue.blocking_push(i);
    }
  }

  const size_t m_boundary{10};
  waiting_queue<size_t> m_bounded_waiting_queue{m_boundary};
  waiting_queue<size_t> m_unbounded_waiting_queue;
};

TEST_CASE_METHOD(
    waiting_queue_test,
    "Maximum size of unbounded queue should be equal to size_t::max") {
  REQUIRE(m_unbounded_waiting_queue.max_size() ==
          std::numeric_limits<size_t>::max());
}

TEST_CASE_METHOD(waiting_queue_test, "Queue should be empty by default") {
  REQUIRE(m_unbounded_waiting_queue.empty() == true);
}

TEST_CASE_METHOD(waiting_queue_test,
                 "User can put an unlimited amount of elements into an "
                 "unbounded queue and retrieve them back") {
  const size_t numberOfElements{10 * 1000};

  for (size_t i = 0; i < numberOfElements; i++) {
    m_unbounded_waiting_queue.blocking_push(i);
  }
  REQUIRE(m_unbounded_waiting_queue.size() == numberOfElements);

  for (size_t i = 0; i < numberOfElements; i++) {
    REQUIRE(m_unbounded_waiting_queue.blocking_pop() == i);
  }
  REQUIRE(m_unbounded_waiting_queue.empty() == true);
}

TEST_CASE_METHOD(
    waiting_queue_test,
    "Reading from an empty queue should throw an exception after a time out") {
  const auto timeout = 300ms;
  REQUIRE_THROWS_AS(m_unbounded_waiting_queue.blocking_pop(timeout),
                    timed_out_exception);
  REQUIRE(time_passed() > timeout);
}

TEST_CASE_METHOD(waiting_queue_test, "Reading client should be unblocked when "
                                     "an element is added to the queue") {
  const auto timeout = 1s;
  const auto time_to_wait_before_send = timeout / 2;
  const auto data = 1;

  std::thread sender([&] {
    std::this_thread::sleep_for(time_to_wait_before_send);
    m_unbounded_waiting_queue.blocking_push(data);
  });

  REQUIRE(m_unbounded_waiting_queue.blocking_pop(timeout) == data);
  REQUIRE(time_passed() > time_to_wait_before_send);
  sender.join();
}

TEST_CASE_METHOD(waiting_queue_test,
                 "Sender should be blocked if the queue is full") {
  fill_waiting_queue();

  REQUIRE_THROWS_AS(m_bounded_waiting_queue.blocking_push(1, 100ms),
                    timed_out_exception);
  REQUIRE(time_passed() > 100ms);
}

TEST_CASE_METHOD(waiting_queue_test,
                 "Sender should be unblocked when the queue has free space") {
  fill_waiting_queue();
  const auto timeout = 1s;
  const auto time_to_wait_before_read = timeout / 2;

  std::thread receiver([&] {
    std::this_thread::sleep_for(time_to_wait_before_read);
    m_unbounded_waiting_queue.blocking_pop();
  });

  m_unbounded_waiting_queue.blocking_push(1, timeout);
  REQUIRE(time_passed() > time_to_wait_before_read);
  receiver.join();
}

TEST_CASE_METHOD(waiting_queue_test,
                 "Receive operations should be interruptible") {
  bool receiver1_interrupted{false};
  bool receiver2_interrupted{false};

  auto receiveCall{[&] {
    try {
      m_unbounded_waiting_queue.blocking_pop();
    } catch (const interrupted_exception&) {
      return true;
    }
    return false;
  }};

  std::thread receiver1{[&] { receiver1_interrupted = receiveCall(); }};
  std::thread receiver2{[&] { receiver2_interrupted = receiveCall(); }};

  // wait until receiver threads are blocked on reading from the queue
  std::this_thread::sleep_for(300ms);

  m_unbounded_waiting_queue.interrupt();

  // wait until receivers get interrupted with an exception
  receiver1.join();
  receiver2.join();

  REQUIRE(receiver1_interrupted == true);
  REQUIRE(receiver2_interrupted == true);
}

TEST_CASE_METHOD(waiting_queue_test,
                 "Send operations should be interruptible") {
  fill_waiting_queue();

  auto sender1_interrupted{false};
  auto sender2_interrupted{false};

  auto sendCall{[&] {
    try {
      m_bounded_waiting_queue.blocking_push(1);
    } catch (const interrupted_exception&) {
      return true;
    }
    return false;
  }};

  std::thread sender1{[&] { sender1_interrupted = sendCall(); }};
  std::thread sender2{[&] { sender2_interrupted = sendCall(); }};

  // wait until sender threads are blocked on adding to the queue
  std::this_thread::sleep_for(300ms);

  m_bounded_waiting_queue.interrupt();

  // wait until senders get interrupted with an exception
  sender1.join();
  sender2.join();

  REQUIRE(sender1_interrupted == true);
  REQUIRE(sender2_interrupted == true);
}

TEST_CASE_METHOD(
    waiting_queue_test,
    "After the queue is interrupted all send and receive calls should fail") {
  m_unbounded_waiting_queue.interrupt();

  REQUIRE_THROWS_AS(m_unbounded_waiting_queue.blocking_push(1),
                    interrupted_exception);
  REQUIRE_THROWS_AS(m_unbounded_waiting_queue.blocking_pop(),
                    interrupted_exception);
}
