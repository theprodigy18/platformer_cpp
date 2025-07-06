#include "renderer/render_interface.hpp"
#include "core/input.hpp"

namespace drop::renderer
{

    RenderData* g_renderData;

    void DrawSprite(resources::SpriteID id, math::Vec2 pos)
    {
        resources::Sprite sprite {GetSprite(id)};

        math::Vec2 size {math::IVec2ToVec2(sprite.spriteSize)};

        Transform transform {};
        transform.atlasOffset = sprite.atlasOffset;
        transform.spriteSize  = sprite.spriteSize;
        transform.pos         = pos - size / 2.0f;
        transform.size        = size;

        g_renderData->transforms[g_renderData->transformCount++] = transform;
    }

    void DrawSprite(resources::SpriteID id, math::IVec2 pos)
    {
        DrawSprite(id, math::IVec2ToVec2(pos));
    }

    math::IVec2 ScreenToWorld(math::IVec2 screenPos)
    {
        OrthographicCamera2D camera {g_renderData->gameCamera};

        i32 xPos {(f32) screenPos.x / (f32) core::g_input->screenSize.x * camera.dimensions.x};

        // Offset by half the camera width and the camera position.
        xPos += -camera.dimensions.x / 2.0f + camera.position.x;

        i32 yPos {(f32) screenPos.y / (f32) core::g_input->screenSize.y * camera.dimensions.y};

        // Offset by half the camera height and the camera position.
        yPos += camera.dimensions.y / 2.0f + camera.position.y;

        return {xPos, yPos};
    }
} // namespace drop::renderer
