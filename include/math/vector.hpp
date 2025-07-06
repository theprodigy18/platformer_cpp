#pragma once

#include "common/common_header.hpp"

namespace drop::math
{
    struct Vec2
    {
        f32 x {0.0f};
        f32 y {0.0f};

        Vec2 operator/(f32 scalar)
        {
            return {x / scalar, y / scalar};
        }

        Vec2 operator-(Vec2 other)
        {
            return {x - other.x, y - other.y};
        }
    };

    struct IVec2
    {
        i32 x {0};
        i32 y {0};

        IVec2 operator-(IVec2 other)
        {
            return {x - other.x, y - other.y};
        }
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

        f32& operator[](i32 index)
        {
            return values[index];
        }
    };

    Vec2 IVec2ToVec2(IVec2 v);
} // namespace drop::math