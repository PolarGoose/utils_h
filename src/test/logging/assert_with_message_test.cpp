#include "utils_h/logging/assert_with_message.h"

TEST_CASE("assert_macro") {
  UTILS_H_ASSERT_MSG(true, "some message");
  UTILS_H_ASSERT_MSG(2 == 2, "some message");
}
