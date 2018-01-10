#include "utils_h/bit_cast.h"
#include "utils_h/struct_aliasing_disable.h"
#include <doctest.h>

using namespace utils_h;

template <typename Source, typename Dest> void bit_cast_test(const Source src) {
  const auto dst = bit_cast<Dest>(src);
  REQUIRE(memcmp(&dst, &src, sizeof(src)) == 0);

  const auto new_src = bit_cast<Source>(dst);
  REQUIRE(memcmp(&new_src, &src, sizeof(src)) == 0);
}

UTILS_H_DISABLE_STRUCT_ALIASING_BEGIN
struct struct_of_64_bytes {
  uint8_t a;
  uint32_t b;
  uint8_t c;
  uint8_t d;
  uint8_t e;
};
UTILS_H_DISABLE_STRUCT_ALIASING_END

TEST_CASE("bit_cast - Should convert data back and forth") {
  bit_cast_test<float, uint32_t>(1.2F);
  bit_cast_test<char, uint8_t>('a');
  bit_cast_test<uint64_t, struct_of_64_bytes>(0x12af43db4589a7c4);
}
