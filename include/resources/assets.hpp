#pragma once

#include "math/vector.hpp"

namespace drop::resources
{

    enum SpriteID
    {
        SPRITE_AXE,

        SPRITE_COUNT
    };

    struct Sprite
    {
        math::IVec2 atlasOffset;
        math::IVec2 spriteSize;
    };

    Sprite GetSprite(SpriteID id);

} // namespace drop::resources
