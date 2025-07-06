#pragma once

#include "common/common_header.hpp"

namespace drop::core
{
    struct Input
    {
        i32 screenSizeX {0};
        i32 screenSizeY {0};
    };

    extern Input* g_input;
} // namespace drop::core
