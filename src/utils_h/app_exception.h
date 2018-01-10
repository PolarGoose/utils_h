#pragma once
#include "utils_h/non_copyable_and_non_movable.h"
#include "utils_h/string_conversion.h"

namespace utils_h {

class app_exception : public std::exception {
public:
  app_exception() = default;

  app_exception(const char* msg) { ss << msg; }

  app_exception(const app_exception& other) noexcept : ss(other.ss.str()) {}

  app_exception& operator=(const app_exception& other) = delete;

  virtual const char* what() const noexcept override {
    msg = ss.str();
    return msg.c_str();
  }

  template <typename T> app_exception& operator<<(const T& val) {
    ss << val;
    return *this;
  }

private:
  std::stringstream ss;
  mutable std::string msg;
};

class app_wide_exception : public std::exception {
public:
  app_wide_exception() = default;

  app_wide_exception(const wchar_t* msg) { ss << msg; }

  app_wide_exception(const app_wide_exception& other) noexcept : ss(other.ss.str()) {}

  app_wide_exception& operator=(const app_wide_exception& other) = delete;

  virtual const char* what() const noexcept override {
    msg = to_utf8(ss.str());
    return msg.c_str();
  }

  const wchar_t* wide_what() const noexcept {
    wide_msg = ss.str();
    return wide_msg.c_str();
  }

  template <typename T> app_wide_exception& operator<<(const T& val) {
    ss << val;
    return *this;
  }

private:
  std::wstringstream ss;
  mutable std::string msg;
  mutable std::wstring wide_msg;
};

}
