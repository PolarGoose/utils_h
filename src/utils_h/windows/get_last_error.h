#pragma once
#include <Windows.h>

namespace utils_h::windows {

inline std::wstring get_last_error() {
  if (const auto& last_error = GetLastError(); last_error != 0) {
    LPTSTR err_msg = nullptr;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK,
                  nullptr, last_error, 0, (LPTSTR)&err_msg, 0, nullptr);
    std::wstringstream str;
    str << L"ErrorCode=" << last_error << L"(0x" << std::hex << last_error << L")" << L". ErrorMessage='" << err_msg << L"'";

    LocalFree(err_msg);
    return str.str();
  }
  return L"No last error information";
}

}
