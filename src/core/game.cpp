#include "core/game.hpp"

namespace drop::core
{
    constexpr i32 WORLD_WIDTH {320};
    constexpr i32 WORLD_HEIGHT {180};
    constexpr i32 TILE_SIZE {8};

    GameState* g_gameState;

    bool JustPressed(GameInputType inputType)
    {
        KeyMapping mapping {g_gameState->keyMappings[inputType]};
        for (u32 i {0}; i < mapping.keys.count; i++)
        {
            if (g_input->keys[mapping.keys[i]].justPressed)
            {
                return true;
            }
        }

        return false;
    }

    bool IsDown(GameInputType inputType)
    {
        KeyMapping mapping {g_gameState->keyMappings[inputType]};
        for (u32 i {0}; i < mapping.keys.count; i++)
        {
            if (g_input->keys[mapping.keys[i]].isDown)
            {
                return true;
            }
        }

        return false;
    }

    EXPORT_FN void UpdateGame(GameState* gameState, renderer::RenderData* renderData, Input* input)
    {
        if (renderer::g_renderData != renderData)
        {
            g_gameState            = gameState;
            renderer::g_renderData = renderData;
            g_input                = input;
        }

        if (!g_gameState->initialized)
        {
            renderData->gameCamera.dimensions = {WORLD_WIDTH, WORLD_HEIGHT};
            gameState->initialized            = true;

            // Key mappings.
            {
                g_gameState->keyMappings[MOVE_UP].keys.Add(KEY_W);
                g_gameState->keyMappings[MOVE_UP].keys.Add(KEY_UP);
                g_gameState->keyMappings[MOVE_DOWN].keys.Add(KEY_S);
                g_gameState->keyMappings[MOVE_DOWN].keys.Add(KEY_DOWN);
                g_gameState->keyMappings[MOVE_LEFT].keys.Add(KEY_A);
                g_gameState->keyMappings[MOVE_LEFT].keys.Add(KEY_LEFT);
                g_gameState->keyMappings[MOVE_RIGHT].keys.Add(KEY_D);
                g_gameState->keyMappings[MOVE_RIGHT].keys.Add(KEY_RIGHT);
            }
        }

        renderData->gameCamera.position.x = 0;
        renderData->gameCamera.position.y = 0;

        renderer::DrawSprite(resources::SPRITE_AXE, g_gameState->playerPos);

        if (IsDown(MOVE_LEFT))
        {
            g_gameState->playerPos.x -= 1;
        }
        if (IsDown(MOVE_RIGHT))
        {
            g_gameState->playerPos.x += 1;
        }
        if (IsDown(MOVE_UP))
        {
            g_gameState->playerPos.y -= 1;
        }
        if (IsDown(MOVE_DOWN))
        {
            g_gameState->playerPos.y += 1;
        }
    }
} // namespace drop::core
