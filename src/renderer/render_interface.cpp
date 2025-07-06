#include "renderer/render_interface.hpp"

namespace drop::renderer
{

    RenderData* g_renderData;

    void DrawSprite(resources::SpriteID id, math::Vec2 pos, math::Vec2 size)
    {
        resources::Sprite sprite {GetSprite(id)};

        Transform transform {};
        transform.atlasOffset = sprite.atlasOffset;
        transform.spriteSize  = sprite.spriteSize;
        transform.pos         = pos;
        transform.size        = size;

        g_renderData->transforms[g_renderData->transformCount++] = transform;
    }
} // namespace drop::renderer
