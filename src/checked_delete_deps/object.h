#pragma once

#include <print>

#include "cpp_utils/util.h"

class Object {
public:
    ~Object() {
        PRINT_CURRENT_FUNCTION_NAME;
    }
};
