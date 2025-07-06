#include "platform/platform.hpp"
#include "core/input.hpp"
#include "renderer/render_interface.hpp"

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
                core::g_input->screenSize.x = rc.right - rc.left;
                core::g_input->screenSize.y = rc.bottom - rc.top;
            }
            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            {
                bool            isDown {msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN || msg == WM_LBUTTONDOWN};
                core::KeyCodeID keycode {core::KeyCodeLookupTable[wParam]};
                core::Key*      key {&core::g_input->keys[keycode]};
                key->justPressed  = !key->justPressed && !key->isDown && isDown;
                key->justReleased = !key->justReleased && key->isDown && !isDown;
                key->isDown       = isDown;
                key->halfTransitionCount++;

                break;
            }
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONUP:
            case WM_MBUTTONUP:
            {
                bool isDown {msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN};
                i32  mouseCode {
                    (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)   ? VK_LBUTTON
                     : (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP) ? VK_RBUTTON
                                                                      : VK_MBUTTON};

                core::KeyCodeID keyCode {core::KeyCodeLookupTable[mouseCode]};
                core::Key*      key {&core::g_input->keys[keyCode]};
                key->justPressed  = !key->justPressed && !key->isDown && isDown;
                key->justReleased = !key->justReleased && key->isDown && !isDown;
                key->isDown       = isDown;
                key->halfTransitionCount++;

                break;
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

        core::g_input->screenSize.x = rc.right - rc.left;
        core::g_input->screenSize.y = rc.bottom - rc.top;

        D_TRACE("Win32 window created!");

        return info;
    }

    void PlatformUpdateWindow(bool& running)
    {
        // Clear the transitionCount of all keys.
        {
            for (u32 i {0}; i < core::KEY_COUNT; i++)
            {
                core::g_input->keys[i].justPressed         = false;
                core::g_input->keys[i].justReleased        = false;
                core::g_input->keys[i].halfTransitionCount = 0;
            }
        }

        // Gather input.
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

        // Mouse Position.
        {
            POINT point {};
            GetCursorPos(&point);
            ScreenToClient(g_hwnd, &point);

            core::g_input->prevMousePos = core::g_input->mousePos;
            core::g_input->mousePos.x   = point.x;
            core::g_input->mousePos.y   = point.y;
            core::g_input->relMouse     = core::g_input->mousePos - core::g_input->prevMousePos;

            // Mouse position to world space.
            core::g_input->mousePosWorld = renderer::ScreenToWorld(core::g_input->mousePos);
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

    void PlatformFillKeyCodeLookupTable()
    {
        core::KeyCodeLookupTable[VK_LBUTTON] = core::KEY_MOUSE_LEFT;
        core::KeyCodeLookupTable[VK_MBUTTON] = core::KEY_MOUSE_MIDDLE;
        core::KeyCodeLookupTable[VK_RBUTTON] = core::KEY_MOUSE_RIGHT;

        core::KeyCodeLookupTable['A'] = core::KEY_A;
        core::KeyCodeLookupTable['B'] = core::KEY_B;
        core::KeyCodeLookupTable['C'] = core::KEY_C;
        core::KeyCodeLookupTable['D'] = core::KEY_D;
        core::KeyCodeLookupTable['E'] = core::KEY_E;
        core::KeyCodeLookupTable['F'] = core::KEY_F;
        core::KeyCodeLookupTable['G'] = core::KEY_G;
        core::KeyCodeLookupTable['H'] = core::KEY_H;
        core::KeyCodeLookupTable['I'] = core::KEY_I;
        core::KeyCodeLookupTable['J'] = core::KEY_J;
        core::KeyCodeLookupTable['K'] = core::KEY_K;
        core::KeyCodeLookupTable['L'] = core::KEY_L;
        core::KeyCodeLookupTable['M'] = core::KEY_M;
        core::KeyCodeLookupTable['N'] = core::KEY_N;
        core::KeyCodeLookupTable['O'] = core::KEY_O;
        core::KeyCodeLookupTable['P'] = core::KEY_P;
        core::KeyCodeLookupTable['Q'] = core::KEY_Q;
        core::KeyCodeLookupTable['R'] = core::KEY_R;
        core::KeyCodeLookupTable['S'] = core::KEY_S;
        core::KeyCodeLookupTable['T'] = core::KEY_T;
        core::KeyCodeLookupTable['U'] = core::KEY_U;
        core::KeyCodeLookupTable['V'] = core::KEY_V;
        core::KeyCodeLookupTable['W'] = core::KEY_W;
        core::KeyCodeLookupTable['X'] = core::KEY_X;
        core::KeyCodeLookupTable['Y'] = core::KEY_Y;
        core::KeyCodeLookupTable['Z'] = core::KEY_Z;
        core::KeyCodeLookupTable['0'] = core::KEY_0;
        core::KeyCodeLookupTable['1'] = core::KEY_1;
        core::KeyCodeLookupTable['2'] = core::KEY_2;
        core::KeyCodeLookupTable['3'] = core::KEY_3;
        core::KeyCodeLookupTable['4'] = core::KEY_4;
        core::KeyCodeLookupTable['5'] = core::KEY_5;
        core::KeyCodeLookupTable['6'] = core::KEY_6;
        core::KeyCodeLookupTable['7'] = core::KEY_7;
        core::KeyCodeLookupTable['8'] = core::KEY_8;
        core::KeyCodeLookupTable['9'] = core::KEY_9;

        core::KeyCodeLookupTable[VK_SPACE]     = core::KEY_SPACE,
        core::KeyCodeLookupTable[VK_OEM_3]     = core::KEY_TICK,
        core::KeyCodeLookupTable[VK_OEM_MINUS] = core::KEY_MINUS,

        core::KeyCodeLookupTable[VK_OEM_PLUS]   = core::KEY_EQUAL,
        core::KeyCodeLookupTable[VK_OEM_4]      = core::KEY_LEFT_BRACKET,
        core::KeyCodeLookupTable[VK_OEM_6]      = core::KEY_RIGHT_BRACKET,
        core::KeyCodeLookupTable[VK_OEM_1]      = core::KEY_SEMICOLON,
        core::KeyCodeLookupTable[VK_OEM_7]      = core::KEY_QUOTE,
        core::KeyCodeLookupTable[VK_OEM_COMMA]  = core::KEY_COMMA,
        core::KeyCodeLookupTable[VK_OEM_PERIOD] = core::KEY_PERIOD,
        core::KeyCodeLookupTable[VK_OEM_2]      = core::KEY_FORWARD_SLASH,
        core::KeyCodeLookupTable[VK_OEM_5]      = core::KEY_BACKWARD_SLASH,
        core::KeyCodeLookupTable[VK_TAB]        = core::KEY_TAB,
        core::KeyCodeLookupTable[VK_ESCAPE]     = core::KEY_ESCAPE,
        core::KeyCodeLookupTable[VK_PAUSE]      = core::KEY_PAUSE,
        core::KeyCodeLookupTable[VK_UP]         = core::KEY_UP,
        core::KeyCodeLookupTable[VK_DOWN]       = core::KEY_DOWN,
        core::KeyCodeLookupTable[VK_LEFT]       = core::KEY_LEFT,
        core::KeyCodeLookupTable[VK_RIGHT]      = core::KEY_RIGHT,
        core::KeyCodeLookupTable[VK_BACK]       = core::KEY_BACKSPACE,
        core::KeyCodeLookupTable[VK_RETURN]     = core::KEY_RETURN,
        core::KeyCodeLookupTable[VK_DELETE]     = core::KEY_DELETE,
        core::KeyCodeLookupTable[VK_INSERT]     = core::KEY_INSERT,
        core::KeyCodeLookupTable[VK_HOME]       = core::KEY_HOME,
        core::KeyCodeLookupTable[VK_END]        = core::KEY_END,
        core::KeyCodeLookupTable[VK_PRIOR]      = core::KEY_PAGE_UP,
        core::KeyCodeLookupTable[VK_NEXT]       = core::KEY_PAGE_DOWN,
        core::KeyCodeLookupTable[VK_CAPITAL]    = core::KEY_CAPS_LOCK,
        core::KeyCodeLookupTable[VK_NUMLOCK]    = core::KEY_NUM_LOCK,
        core::KeyCodeLookupTable[VK_SCROLL]     = core::KEY_SCROLL_LOCK,
        core::KeyCodeLookupTable[VK_APPS]       = core::KEY_MENU,

        core::KeyCodeLookupTable[VK_SHIFT]  = core::KEY_SHIFT,
        core::KeyCodeLookupTable[VK_LSHIFT] = core::KEY_SHIFT,
        core::KeyCodeLookupTable[VK_RSHIFT] = core::KEY_SHIFT,

        core::KeyCodeLookupTable[VK_CONTROL]  = core::KEY_CONTROL,
        core::KeyCodeLookupTable[VK_LCONTROL] = core::KEY_CONTROL,
        core::KeyCodeLookupTable[VK_RCONTROL] = core::KEY_CONTROL,

        core::KeyCodeLookupTable[VK_MENU]  = core::KEY_ALT,
        core::KeyCodeLookupTable[VK_LMENU] = core::KEY_ALT,
        core::KeyCodeLookupTable[VK_RMENU] = core::KEY_ALT,

        core::KeyCodeLookupTable[VK_F1]  = core::KEY_F1;
        core::KeyCodeLookupTable[VK_F2]  = core::KEY_F2;
        core::KeyCodeLookupTable[VK_F3]  = core::KEY_F3;
        core::KeyCodeLookupTable[VK_F4]  = core::KEY_F4;
        core::KeyCodeLookupTable[VK_F5]  = core::KEY_F5;
        core::KeyCodeLookupTable[VK_F6]  = core::KEY_F6;
        core::KeyCodeLookupTable[VK_F7]  = core::KEY_F7;
        core::KeyCodeLookupTable[VK_F8]  = core::KEY_F8;
        core::KeyCodeLookupTable[VK_F9]  = core::KEY_F9;
        core::KeyCodeLookupTable[VK_F10] = core::KEY_F10;
        core::KeyCodeLookupTable[VK_F11] = core::KEY_F11;
        core::KeyCodeLookupTable[VK_F12] = core::KEY_F12;

        core::KeyCodeLookupTable[VK_NUMPAD0] = core::KEY_NUMPAD_0;
        core::KeyCodeLookupTable[VK_NUMPAD1] = core::KEY_NUMPAD_1;
        core::KeyCodeLookupTable[VK_NUMPAD2] = core::KEY_NUMPAD_2;
        core::KeyCodeLookupTable[VK_NUMPAD3] = core::KEY_NUMPAD_3;
        core::KeyCodeLookupTable[VK_NUMPAD4] = core::KEY_NUMPAD_4;
        core::KeyCodeLookupTable[VK_NUMPAD5] = core::KEY_NUMPAD_5;
        core::KeyCodeLookupTable[VK_NUMPAD6] = core::KEY_NUMPAD_6;
        core::KeyCodeLookupTable[VK_NUMPAD7] = core::KEY_NUMPAD_7;
        core::KeyCodeLookupTable[VK_NUMPAD8] = core::KEY_NUMPAD_8;
        core::KeyCodeLookupTable[VK_NUMPAD9] = core::KEY_NUMPAD_9;
    }

} // namespace drop::platform