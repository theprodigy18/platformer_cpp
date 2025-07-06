#include "renderer/opengl.hpp"
#include "core/game.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

using namespace drop;

namespace
{
    using UpdateGameType = decltype(core::UpdateGame);
    UpdateGameType* UpdateGamePtr {&core::UpdateGame};
}

void GameUpdate(core::GameState* gameState, renderer::RenderData* renderData, core::Input* input);
void ReloadGameDLL(utils::BumpAllocator* transientStorage);

int main()
{
    SetProcessDPIAware();
    D_TRACE("Starting Platformer Game!");

    if (!renderer::GLInit())
    {
        D_ASSERT(false, "Failed to initialize OpenGL.");
        return -1;
    }

    utils::BumpAllocator persistentStorage {utils::MakeBumpAllocator(MB(50))};
    core::g_gameState      = (core::GameState*) utils::BumpAlloc(&persistentStorage, sizeof(core::GameState));
    renderer::g_renderData = (renderer::RenderData*) utils::BumpAlloc(&persistentStorage, sizeof(renderer::RenderData));
    core::g_input          = (core::Input*) utils::BumpAlloc(&persistentStorage, sizeof(core::Input));
    if (!core::g_gameState || !renderer::g_renderData || !core::g_input)
    {
        D_ASSERT(false, "Failed to allocate memory for gamestate, renderdata, and input.");
        return -1;
    }

    platform::PlatformFillKeyCodeLookupTable();
    platform::WindowInfo info {platform::PlatformCreateWindow(1280, 720, L"Platformer Game")};
    if (!info.handle || !info.dc)
    {
        D_ASSERT(false, "Failed to create window.");
        return -1;
    }

    utils::BumpAllocator transientStorage {utils::MakeBumpAllocator(MB(50))};

    if (!renderer::GLCreateContext(info, &transientStorage))
    {
        D_ASSERT(false, "Failed to create OpenGL context.");
        return -1;
    }

    bool running {true};
    while (running)
    {
        ReloadGameDLL(&transientStorage);
        platform::PlatformUpdateWindow(running);
        GameUpdate(core::g_gameState, renderer::g_renderData, core::g_input);
        renderer::GLRenderFrame();
        platform::PlatformSwapBuffers();

        transientStorage.used = 0;
    }

    renderer::GLDestroyContext();
    platform::PlatformDestroyWindow();

    renderer::GLShutdown();

    D_TRACE("Shutting down Platformer Game!");
    TRACK_LEAK_REPORT();
    return 0;
}

void GameUpdate(core::GameState* gameState, renderer::RenderData* renderData, core::Input* input)
{
    UpdateGamePtr(gameState, renderData, input);
}

void ReloadGameDLL(utils::BumpAllocator* transientStorage)
{
    static void* gameDLL;
    static i64   lastEditTimestampGameDLL;

    i64 currentEditTimestampGameDLL {utils::GetTimestamp((char*) "build/windows-debug/game.dll")};
    if (currentEditTimestampGameDLL > lastEditTimestampGameDLL)
    {
        if (gameDLL)
        {
            bool freeResult {platform::PlatformFreeDynamicLibrary(gameDLL)};
            D_ASSERT(freeResult, "Failed to free game DLL!");
            gameDLL = nullptr;
            D_TRACE("Game DLL Freed!");
        }

        while (!utils::CopyingFile("build/windows-debug/game.dll", "build/windows-debug/game_load.dll", transientStorage))
        {
            Sleep(10);
        }
        D_TRACE("Game DLL Copied to game_load.dll!");

        gameDLL = platform::PlatformLoadDynamicLibrary("build/windows-debug/game_load.dll");
        D_ASSERT(gameDLL, "Failed to load game DLL!");

        UpdateGamePtr = (UpdateGameType*) platform::PlatformLoadDynamicFunction(gameDLL, "UpdateGame");
        D_ASSERT(UpdateGamePtr, "Failed to load game DLL!");
        lastEditTimestampGameDLL = currentEditTimestampGameDLL;
    }
}
