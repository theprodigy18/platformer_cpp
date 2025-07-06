#pragma once

#include "vector.hpp"

namespace drop::math
{

    struct Mat4
    {
        union
        {
            Vec4 values[4];
            struct
            {
                f32 ax, bx, cx, dx;
                f32 ay, by, cy, dy;
                f32 az, bz, cz, dz;
                f32 aw, bw, cw, dw;
            };
        };
    };

} // namespace drop::math