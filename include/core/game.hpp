#pragma once

#include "renderer/render_interface.hpp"
#include "input.hpp"

namespace drop::core
{
    extern "C"
    {
        EXPORT_FN void UpdateGame(renderer::RenderData* renderData, Input* input);
    }
} // namespace drop::core
