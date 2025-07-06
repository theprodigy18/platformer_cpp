#include "math/vector.hpp"

namespace drop::math
{
    Vec2 IVec2ToVec2(IVec2 v)
    {
        return Vec2 {(f32) v.x, (f32) v.y};
    }
} // namespace drop::math
