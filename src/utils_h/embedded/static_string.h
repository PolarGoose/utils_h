#pragma once
#include "utils_h/non_copyable_and_non_movable.h"
#include <array>
#include <cstring>
#include <string_view>

namespace utils_h::embedded {

template <size_t StringSizeIncludingNullTerminator> class static_string : non_copyable_and_non_movable {
private:
  const size_t _str_length_excluding_null_terminator;
  const std::array<char, StringSizeIncludingNullTerminator> _buf_null_terminated;

  std::array<char, StringSizeIncludingNullTerminator> CreateBuffer(std::string_view str, const size_t length) {
    std::array<char, StringSizeIncludingNullTerminator> buf;
    std::memcpy(buf.data(), str.data(), length);
    buf[length] = '\0';
    return buf;
  }

public:
  static_string(std::string_view str)
      : _str_length_excluding_null_terminator{std::min(str.size(), _buf_null_terminated.size() - 1)},
        _buf_null_terminated{CreateBuffer(str, _str_length_excluding_null_terminator)} {}

  std::string_view str() { return {_buf_null_terminated.data(), _str_length_excluding_null_terminator}; }
};

}
