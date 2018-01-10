#include "utils_h/concurrency/lock_guard.h"
#include <doctest.h>

static auto is_mutex_locked(std::mutex& m) {
  const auto locked_successfully = m.try_lock();
  if (locked_successfully) {
    m.unlock();
  }
  return !locked_successfully;
}

TEST_CASE("lock_guard tests") {
  std::mutex m;

  SUBCASE("Should lock a mutex at the beginning of the scope") {
    UTILS_H_LOCK_GUARD(m);
    REQUIRE(is_mutex_locked(m) == true);
  }

  SUBCASE("Should unlock a mutex at the end of the scope") {
    { UTILS_H_LOCK_GUARD(m); }
    REQUIRE(is_mutex_locked(m) == false);
  }
}
