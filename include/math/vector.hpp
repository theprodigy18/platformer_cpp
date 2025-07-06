#pragma once

#include "common/common_header.hpp"

namespace drop::math
{
    struct Vec2
    {
        f32 x {0.0f};
        f32 y {0.0f};
    };

    struct IVec2
    {
        i32 x {0};
        i32 y {0};
    };

    struct Vec4
    {
        union
        {
            f32 values[4];
            struct
            {
                f32 x, y, z, w;
            };
            struct
            {
                f32 r, g, b, a;
            };
        };
    };

} // namespace drop::math