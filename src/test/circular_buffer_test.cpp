#include "utils_h/circular_buffer.h"

using namespace utils_h;

class circular_buffer_test {
protected:
  void fill_buffer() {
    for (size_t i{0}; i < m_capacity; i++) {
      m_buffer.put(i);
    }
  }

  const size_t m_capacity{100};
  circular_buffer<size_t> m_buffer{m_capacity};
};

TEST_CASE_METHOD(circular_buffer_test, "Capacity should return correct value") {
  REQUIRE(m_buffer.capacity() == m_capacity);
}

TEST_CASE_METHOD(
    circular_buffer_test,
    "Get method should throw an exception when the buffer is empty") {
  REQUIRE(m_buffer.is_empty() == true);
  REQUIRE_THROWS_AS(m_buffer.get(), circular_buffer_is_empty_exception);
}

TEST_CASE_METHOD(
    circular_buffer_test,
    "Buffer can contain the amount of elements equal to its capacity") {
  fill_buffer();

  REQUIRE(m_buffer.size() == m_capacity);
  REQUIRE(m_buffer.is_full() == true);

  for (size_t i{0}; i < m_capacity; i++) {
    REQUIRE(m_buffer.get() == i);
  }

  REQUIRE(m_buffer.size() == 0);
  REQUIRE(m_buffer.is_empty() == true);
}

TEST_CASE_METHOD(circular_buffer_test, "Adding one element to a full buffer "
                                       "should remove its last added element") {
  fill_buffer();

  m_buffer.put(m_capacity);

  for (size_t i{0}; i < m_capacity; i++) {
    REQUIRE(m_buffer.get() == i + 1);
  }
}
