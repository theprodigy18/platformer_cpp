#pragma once

#include "resources/assets.hpp"

namespace drop::renderer
{
    constexpr i32 MAX_TRANSFORMS {1000};

    struct OrthographicCamera2D
    {
        f32        zoom {1.0f};
        math::Vec2 dimensions;
        math::Vec2 position;
    };

    struct Transform
    {
        math::IVec2 atlasOffset;
        math::IVec2 spriteSize;
        math::Vec2  pos;
        math::Vec2  size;
    };

    struct RenderData
    {
        OrthographicCamera2D gameCamera {};
        OrthographicCamera2D uiCamera {};
        i32                  transformCount {0};
        Transform            transforms[MAX_TRANSFORMS];
    };

    extern RenderData* g_renderData;

    void DrawSprite(resources::SpriteID id, math::Vec2 pos);
    void DrawSprite(resources::SpriteID id, math::IVec2 pos);

    math::IVec2 ScreenToWorld(math::IVec2 screenPos);
} // namespace drop::renderer
