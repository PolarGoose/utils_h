#pragma once
#include <array>
#include <cstdarg>
#include <cstdio>
#include <optional>
#include <string_view>

#ifdef __GNUC__
#define PRIV_ATTRIBUTE_CHECK_FORMAT_STRING(format_string_index_one_based, vargs_index_one_based)                                                     \
  __attribute__((format(printf, format_string_index_one_based, vargs_index_one_based)))
#else
#define PRIV_ATTRIBUTE_CHECK_FORMAT_STRING(format_string_index_one_based, vargs_index_one_based)
#endif

#if defined(_MSC_VER)
#include <sal.h>
#else
#define _Printf_format_string_
#endif

namespace utils_h::embedded {

template <int NumOfCharsExclNullTerminator> struct snprintf_result {
  std::string_view str() const { return std::string_view(null_terminated_str_buf.data(), str_len_excluding_null_terminator); }

private:
  std::array<char, NumOfCharsExclNullTerminator + 1> null_terminated_str_buf;
  size_t str_len_excluding_null_terminator;

  template <int N> friend std::optional<snprintf_result<N>> snprintf(const char* formatString, ...);
};

template <int NumOfCharsExclNullTerminator>
std::optional<snprintf_result<NumOfCharsExclNullTerminator>> snprintf(_Printf_format_string_ const char* formatString, ...)
    PRIV_ATTRIBUTE_CHECK_FORMAT_STRING(1, 2);
template <int NumOfCharsExclNullTerminator>
std::optional<snprintf_result<NumOfCharsExclNullTerminator>> snprintf(_Printf_format_string_ const char* formatString, ...) {
  snprintf_result<NumOfCharsExclNullTerminator> res{};

  va_list args;
  va_start(args, formatString);
  res.str_len_excluding_null_terminator = std::vsnprintf(res.null_terminated_str_buf.data(), res.null_terminated_str_buf.size(), formatString, args);
  va_end(args);

  if (res.str_len_excluding_null_terminator < 0) {
    return {};
  }

  if (res.str_len_excluding_null_terminator > NumOfCharsExclNullTerminator) {
    res.str_len_excluding_null_terminator = res.null_terminated_str_buf.size() - 1;
  }

  return res;
}

}
