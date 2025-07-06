#pragma once

#include "renderer/render_interface.hpp"
#include "input.hpp"
#include "utils/array.hpp"

namespace drop::core
{

    enum GameInputType
    {
        MOVE_LEFT,
        MOVE_RIGHT,
        MOVE_UP,
        MOVE_DOWN,
        JUMP,

        MOUSE_LEFT,
        MOUSE_RIGHT,

        GAME_INPUT_COUNT
    };

    struct KeyMapping
    {
        utils::Array<KeyCodeID, 3> keys;
    };

    struct GameState
    {
        bool        initialized {false};
        math::IVec2 playerPos;
        KeyMapping  keyMappings[GAME_INPUT_COUNT];
    };

    extern GameState* g_gameState;

    extern "C"
    {
        EXPORT_FN void UpdateGame(GameState* gameState, renderer::RenderData* renderData, Input* input);
    }
} // namespace drop::core
