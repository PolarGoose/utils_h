#pragma once
#include <chrono>

namespace {

auto time_passed(const std::chrono::steady_clock::time_point& startTime) { return std::chrono::steady_clock::now() - startTime; }

}
