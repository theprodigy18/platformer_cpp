#include "resources/assets.hpp"

namespace drop::resources
{
    Sprite GetSprite(SpriteID id)
    {
        Sprite sprite {};
        switch (id)
        {
        case SPRITE_AXE:
        {
            sprite.atlasOffset = {0, 0};
            sprite.spriteSize  = {32, 32};
            break;
        }
        default:
            break;
        }

        return sprite;
    }
} // namespace drop::resources
