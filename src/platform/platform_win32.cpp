#include "platform/platform.hpp"
#include "core/input.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

namespace drop::platform
{
    namespace
    {
        bool g_running {true};
        HWND g_hwnd {};
        HDC  g_dc {};

        void GetCenteredPosition(i32& outX, i32& outY, i32 width, i32 height)
        {
            RECT dr {};
            SystemParametersInfoW(SPI_GETWORKAREA, 0, &dr, 0);
            i32 dw {dr.right - dr.left};
            i32 dh {dr.bottom - dr.top};

            outX = dr.left + (dw - width) / 2;
            outY = dr.top + (dh - height) / 2;
        }

        LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
            switch (msg)
            {
            case WM_CLOSE:
            {
                PostQuitMessage(0);
                g_running = false;
                return 0;
            }
            case WM_SIZE:
            {
                RECT rc {};
                GetClientRect(hwnd, &rc);
                core::g_input->screenSizeX = rc.right - rc.left;
                core::g_input->screenSizeY = rc.bottom - rc.top;
            }
            default:
                break;
            }

            return DefWindowProcW(hwnd, msg, wParam, lParam);
        }
    } // namespace anonymous

    WindowInfo PlatformCreateWindow(u32 width, u32 height, const wchar_t* title)
    {
        WindowInfo info {};

        HINSTANCE hInstance {GetModuleHandleW(nullptr)};
        if (!hInstance)
        {
            D_ASSERT(false, "Failed to get instance handle.");
            return info;
        }

        WNDCLASSEXW wcex {};
        wcex.cbSize        = sizeof(WNDCLASSEXW);
        wcex.style         = CS_HREDRAW | CS_VREDRAW;
        wcex.hInstance     = hInstance;
        wcex.cbClsExtra    = 0;
        wcex.cbWndExtra    = 0;
        wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
        wcex.hIcon         = LoadIcon(nullptr, IDI_APPLICATION);
        wcex.hIconSm       = LoadIcon(nullptr, IDI_APPLICATION);
        wcex.lpszClassName = L"DropWindowClass";
        wcex.lpszMenuName  = nullptr;
        wcex.lpfnWndProc   = WndProc;
        wcex.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);

        if (!RegisterClassExW(&wcex))
        {
            D_ASSERT(false, "Failed to register window class.");
            return info;
        }

        u32  dwStyle {WS_OVERLAPPEDWINDOW};
        RECT rc {};
        SetRect(&rc, 0, 0, width, height);
        AdjustWindowRectEx(&rc, dwStyle, false, 0);

        i32 x {0}, y {0};
        GetCenteredPosition(x, y, rc.right - rc.left, rc.bottom - rc.top);

        g_hwnd = CreateWindowExW(
            0,
            L"DropWindowClass",
            title,
            WS_OVERLAPPEDWINDOW,
            x,
            y,
            rc.right - rc.left,
            rc.bottom - rc.top,
            nullptr,
            nullptr,
            hInstance,
            nullptr);
        if (!g_hwnd)
        {
            D_ASSERT(false, "Failed to create window.");
            return info;
        }
        TRACK_LEAK_ALLOC(g_hwnd, LeakType::HANDLE, "HWND");

        g_dc = GetDC(g_hwnd);
        if (!g_dc)
        {
            D_ASSERT(false, "Failed to get device context.");
            return info;
        }
        TRACK_LEAK_ALLOC(g_dc, LeakType::HANDLE, "HDC");

        ShowWindow(g_hwnd, SW_SHOW);
        UpdateWindow(g_hwnd);

        info.handle = g_hwnd;
        info.dc     = g_dc;

        core::g_input->screenSizeX = rc.right - rc.left;
        core::g_input->screenSizeY = rc.bottom - rc.top;

        D_TRACE("Win32 window created!");

        return info;
    }

    void PlatformUpdateWindow(bool& running)
    {
        MSG msg {};
        while (PeekMessageW(&msg, g_hwnd, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);

            if (msg.message == WM_QUIT)
            {
                break;
            }
        }

        running = g_running;
    }

    void PlatformSwapBuffers()
    {
        SwapBuffers(g_dc);
    }

    void PlatformDestroyWindow()
    {
        ReleaseDC(g_hwnd, g_dc);
        TRACK_LEAK_FREE(g_dc);
        DestroyWindow(g_hwnd);
        TRACK_LEAK_FREE(g_hwnd);

        g_hwnd = nullptr;
        g_dc   = nullptr;

        UnregisterClassW(L"DropWindowClass", GetModuleHandleW(nullptr));
        D_TRACE("Win32 window destroyed!");
    }

    void* PlatformLoadDynamicLibrary(char* dll)
    {
        HMODULE result {LoadLibraryA(dll)};
        D_ASSERT(result, "Failed to load dynamic library: %s", dll);

        return result;
    }

    void* PlatformLoadDynamicFunction(void* dll, char* funcName)
    {
        FARPROC proc {GetProcAddress((HMODULE) dll, funcName)};
        D_ASSERT(proc, "Failed to load dynamic function: %s", funcName);
        return (void*) proc;
    }

    bool PlatformFreeDynamicLibrary(void* dll)
    {
        BOOL result {FreeLibrary((HMODULE) dll)};
        D_ASSERT(result, "Failed to free dynamic library: %p", dll);
        return result;
    }
} // namespace drop::platform