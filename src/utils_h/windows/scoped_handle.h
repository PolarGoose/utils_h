#pragma once
#include <Windows.h>

namespace utils_h::windows {

namespace priv {

struct HandleDeleter {
  using pointer = HANDLE;

  void operator()(const HANDLE h) {
    if (h == nullptr || h == INVALID_HANDLE_VALUE) {
      return;
    }

    CloseHandle(h);
  }
};

struct ServiceHandleDeleter {
  using pointer = SC_HANDLE;

  void operator()(const SC_HANDLE h) {
    if (h == nullptr || h == INVALID_HANDLE_VALUE) {
      return;
    }

    CloseServiceHandle(h);
  }
};

}

using scoped_handle = std::unique_ptr<HANDLE, priv::HandleDeleter>;
using scoped_sc_handle = std::unique_ptr<SC_HANDLE, priv::ServiceHandleDeleter>;

}
