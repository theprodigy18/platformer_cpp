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

        Vec4& operator[](i32 col)
        {
            return values[col];
        }
    };

    Mat4 OrthographicProjection(f32 left, f32 right, f32 top, f32 bottom);
} // namespace drop::math