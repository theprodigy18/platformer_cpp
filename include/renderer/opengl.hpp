#pragma once

#include "platform/platform.hpp"
#include "utils/bump_allocator.hpp"
#include "utils/file_io.hpp"

#include "opengl/glcorearb.h"
#include "opengl/glext.h"

namespace drop::renderer
{
    bool GLInit();
    bool GLCreateContext(platform::WindowInfo info, utils::BumpAllocator* transientStorage);
    void GLRenderFrame();
    void GLDestroyContext();
    void GLShutdown();
}