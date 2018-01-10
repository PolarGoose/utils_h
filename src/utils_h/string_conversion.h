#pragma once
#include <codecvt>
#include <locale>
#include <string>
#include <string_view>

namespace utils_h {

inline std::wstring to_utf16(std::string_view str) {
  static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  return converter.from_bytes(str.data());
}

inline std::string to_utf8(std::wstring_view wideStr) {
  static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  return converter.to_bytes(wideStr.data());
}

}
