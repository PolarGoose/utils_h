#include "utils_h/circular_buffer.h"
#include <doctest.h>

using namespace utils_h;

TEST_CASE("circular_buffer tests") {
  const size_t capacity{100};
  circular_buffer<size_t> buffer(capacity);
  const auto& fill_buffer = [&] {
    for (size_t i{0}; i < buffer.capacity(); i++) {
      buffer.put(i);
    }
  };

  SUBCASE("Should return correct capacity") { REQUIRE(buffer.capacity() == capacity); }

  SUBCASE("Should throw an exception when the buffer is empty") {
    REQUIRE(buffer.is_empty() == true);
    REQUIRE_THROWS_AS(buffer.get(), circular_buffer_is_empty_exception);
  }

  SUBCASE("Should be able to contains contain the amount of elements equal to its capacity") {
    fill_buffer();

    REQUIRE(buffer.size() == capacity);
    REQUIRE(buffer.is_full() == true);

    for (size_t i{0}; i < capacity; i++) {
      REQUIRE(buffer.get() == i);
    }

    REQUIRE(buffer.size() == 0);
    REQUIRE(buffer.is_empty() == true);
  }

  SUBCASE("When new element is added to a full buffer, it should replace it with its its last element") {
    fill_buffer();

    buffer.put(capacity);

    for (size_t i{0}; i < capacity; i++) {
      REQUIRE(buffer.get() == i + 1);
    }
  }
}
