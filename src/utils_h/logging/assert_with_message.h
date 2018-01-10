#pragma once
#include "utils_h/type_traits.h"
#include <cassert>
#include <type_traits>

#define UTILS_H_ASSERT_MSG(expression, message) assert((expression) && ("" message))
