#include "utils_h/concurrency/lock_guard.h"

struct lock_guard_test {
  auto is_mutex_locked() {
    const auto locked_successfully = _m.try_lock();
    if (locked_successfully) {
      _m.unlock();
    }
    return !locked_successfully;
  }

  std::mutex _m;
};

TEST_CASE_METHOD(lock_guard_test, "Should locks a mutex") {
  UTILS_H_LOCK_GUARD(_m);
  REQUIRE(is_mutex_locked() == true);
}

TEST_CASE_METHOD(lock_guard_test,
                 "Should unlock a mutex at the end of the scope") {
  { UTILS_H_LOCK_GUARD(_m); }
  REQUIRE(is_mutex_locked() == false);
}
