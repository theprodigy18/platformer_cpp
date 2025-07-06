#include "core/game.hpp"

namespace drop::core
{
    EXPORT_FN void UpdateGame(renderer::RenderData* renderData, Input* input)
    {
        if (renderer::g_renderData != renderData)
        {
            renderer::g_renderData = renderData;
            g_input                = input;
        }

        for (u32 x {0}; x < 10; x++)
        {
            for (u32 y {0}; y < 10; y++)
            {
                renderer::DrawSprite(resources::SPRITE_AXE, {100.f * x, 100.f * y}, {100.f, 100.f});
            }
        }
    }
} // namespace drop::core
