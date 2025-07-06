#pragma once

#include "common/common_header.hpp"

namespace drop::platform
{
    struct WindowInfo
    {
        void* handle {nullptr};
        void* dc {nullptr};
    };

    WindowInfo PlatformCreateWindow(u32 width, u32 height, const wchar_t* title);
    void       PlatformUpdateWindow(bool& running);
    void       PlatformSwapBuffers();
    void       PlatformDestroyWindow();

    void* PlatformLoadDynamicLibrary(char* dll);
    void* PlatformLoadDynamicFunction(void* dll, char* funcName);
    bool  PlatformFreeDynamicLibrary(void* dll);

} // namespace drop::platform
