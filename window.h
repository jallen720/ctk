/// Macros
////////////////////////////////////////////////////////////
#define CTK_MOUSE_BUTTON_HANDLERS(name, num) \
    case WM_ ## name ## DOWN: \
    { \
        Set(&g_input.mouse_button_down, num, true); \
        return 0; \
    } \
    case WM_ ## name ## UP: \
    { \
        Set(&g_input.mouse_button_down, num, false); \
        return 0; \
    }

/// Data
////////////////////////////////////////////////////////////
static constexpr uint32 MOUSE_BUTTON_COUNT = 8;

struct WindowInfo
{
    sint32      x;
    sint32      y;
    sint32      width;
    sint32      height;
    const char* title;
    DWORD       style;

    Func<LRESULT, HWND, UINT, WPARAM, LPARAM> callback;
};

struct Window
{
    HWND    hnd;
    HMODULE instance;
    sint32  width;
    sint32  height;
    bool    is_open;
    bool    moved;
};

struct Input
{
    FArray<bool, MOUSE_BUTTON_COUNT> mouse_button_down;
    FArray<bool, (uint32)Key::COUNT> key_down;
    FArray<Key,  (uint32)Key::COUNT> keys_pressed;
    FArray<Key,  (uint32)Key::COUNT> keys_released;
};

/// Globals
////////////////////////////////////////////////////////////
static Window g_window;
static Input  g_input;

/// Interface
////////////////////////////////////////////////////////////
static LRESULT CALLBACK DefaultWindowCallback(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param)
{
    if (g_window.is_open && g_window.hnd == hwnd)
    {
        switch (msg)
        {
            // Window events. Don't skip DefWindowProc().
            case WM_DESTROY:
            {
                g_window.is_open = false;
                break;
            }
            case WM_MOVING:
            {
                g_window.moved = true;
                break;
            }
            case WM_SIZE:
            {
                // Win32 Docs:
                //     The low-order word of l_param specifies the new width of the client area.
                //     The high-order word of l_param specifies the new height of the client area.
                g_window.width  = l_param & 0xFFFF;
                g_window.height = l_param >> 16;
                break;
            }

            // Key events. Skip DefProcWindow().
            case WM_KEYDOWN:
            {
                Key key = GetKey((uint32)w_param);
                uint32 key_index = (uint32)key;

                // Prevent key repeat from triggering pressed event.
                if (!Get(&g_input.key_down, key_index))
                {
                    Push(&g_input.keys_pressed, key);
                }

                Set(&g_input.key_down, key_index, true);
                return 0;
            }
            case WM_KEYUP:
            {
                Key key = GetKey((uint32)w_param);
                Set(&g_input.key_down, (uint32)key, false);
                Push(&g_input.keys_released, key);
                return 0;
            }
            case WM_SYSKEYDOWN:
            {
                Key key = GetKey((uint32)w_param);
                uint32 key_index = (uint32)key;

                // Prevent key repeat from triggering pressed event.
                if (!Get(&g_input.key_down, key_index))
                {
                    Push(&g_input.keys_pressed, key);
                }

                Set(&g_input.key_down, key_index, true);
                return 0;
            }
            case WM_SYSKEYUP:
            {
                Key key = GetKey((uint32)w_param);
                Set(&g_input.key_down, (uint32)key, false);
                Push(&g_input.keys_released, key);
                return 0;
            }

            // Mouse Button Events
            CTK_MOUSE_BUTTON_HANDLERS(LBUTTON, 0)
            CTK_MOUSE_BUTTON_HANDLERS(RBUTTON, 1)
            CTK_MOUSE_BUTTON_HANDLERS(MBUTTON, 2)
            case WM_XBUTTONDOWN:
            {
                uint32 button_index = (w_param >> 16) + 2;
                CTK_ASSERT(button_index < MOUSE_BUTTON_COUNT);
                Set(&g_input.mouse_button_down, button_index, true);
                return 0;
            }
            case WM_XBUTTONUP:
            {
                uint32 button_index = (w_param >> 16) + 2;
                CTK_ASSERT(button_index < MOUSE_BUTTON_COUNT);
                Set(&g_input.mouse_button_down, button_index, false);
                return 0;
            }
        }
    }

    return DefWindowProc(hwnd, msg, w_param, l_param);
}

static void OpenWindow(WindowInfo* info)
{
    if (g_window.is_open)
    {
        CTK_FATAL("OpenWindow() called while a window is already open; can't create multiple windows");
    }

    // Calculate window rect based on surface rect.
    RECT window_rect =
    {
        .left   = info->x,
        .top    = info->y,
        .right  = info->x + info->width,
        .bottom = info->y + info->height,
    };
    DWORD WINDOW_STYLE = info->style == 0 ? WS_OVERLAPPEDWINDOW : info->style;
    AdjustWindowRectEx(&window_rect, WINDOW_STYLE, 0, 0);

    HMODULE instance = GetModuleHandle(NULL);

    // Register and create window.
    static constexpr const char* CLASS_NAME = "Win32 Window";
    WNDCLASS win_class =
    {
        .lpfnWndProc   = info->callback,
        .hInstance     = instance,
        .lpszClassName = CLASS_NAME,
    };
    RegisterClass(&win_class);
    g_window.hnd = CreateWindowEx(0,                                     // Optional Styles
                                  CLASS_NAME,                            // Class
                                  info->title,
                                  WINDOW_STYLE,
                                  window_rect.left,                      // X
                                  window_rect.top,                       // Y
                                  window_rect.right  - window_rect.left, // Width
                                  window_rect.bottom - window_rect.top,  // Height
                                  NULL,                                  // Parent
                                  NULL,                                  // Menu
                                  instance,
                                  NULL);                                 // App Data
    if (g_window.hnd == NULL)
    {
        Win32Error e = {};
        GetWin32Error(&e);
        CTK_FATAL("CreateWindowEx() failed: %.*s", e.message_length, e.message);
    }

    // Initialize window.
    g_window.is_open  = true;
    g_window.instance = instance;
    g_window.width    = info->width;
    g_window.height   = info->height;

    // Initialize input.
    g_input.key_down.count          = (uint32)Key::COUNT;
    g_input.mouse_button_down.count = MOUSE_BUTTON_COUNT;

    ShowWindow(g_window.hnd, SW_SHOW);
}

static void ProcessWindowEvents()
{
    // Clear key events.
    g_input.keys_pressed.count = 0;
    g_input.keys_released.count = 0;

    // Handle event messages via DefaultWindowCallback().
    MSG msg = {};
    while (PeekMessage(&msg, g_window.hnd, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // If window moved during processing, clear key/mb down flags as they wouldn't have been reset if the user released
    // the key while the window was moving.
    if (g_window.moved)
    {
        memset(g_input.key_down.data,          0, sizeof(g_input.key_down.data));
        memset(g_input.mouse_button_down.data, 0, sizeof(g_input.mouse_button_down.data));
        g_window.moved = false;
    }
}

static bool KeyDown(Key key)
{
    return Get(&g_input.key_down, (uint32)key);
}

static bool KeyPressed(Key key)
{
    for (uint32 i = 0; i < g_input.keys_pressed.count; ++i)
    {
        if (Get(&g_input.keys_pressed, i) == key)
        {
            return true;
        }
    }

    return false;
}

static bool KeyReleased(Key key)
{
    for (uint32 i = 0; i < g_input.keys_released.count; ++i)
    {
        if (Get(&g_input.keys_released, i) == key)
        {
            return true;
        }
    }

    return false;
}

static bool MouseButtonDown(uint32 button)
{
    return Get(&g_input.mouse_button_down, button);
}

static Vec2<sint32> GetMousePosition()
{
    POINT mouse_position = {};
    Win32Error e = {};

    if (!GetCursorPos(&mouse_position))
    {
        GetWin32Error(&e);
        CTK_FATAL("GetCursorPos() failed: %.*s", e.message_length, e.message);
    }

    if (!ScreenToClient(g_window.hnd, &mouse_position))
    {
        GetWin32Error(&e);
        CTK_FATAL("ScreenToClient() failed: %.*s", e.message_length, e.message);
    }

    return { mouse_position.x, mouse_position.y };
}

static void SetMousePosition(Vec2<sint32> position)
{
    POINT p = { position.x, position.y };
    Win32Error e = {};

    if (!ClientToScreen(g_window.hnd, &p))
    {
        GetWin32Error(&e);
        CTK_FATAL("ClientToScreen() failed: %.*s", e.message_length, e.message);
    }

    if (!SetCursorPos(p.x, p.y))
    {
        GetWin32Error(&e);
        CTK_FATAL("SetCursorPos() failed: %.*s", e.message_length, e.message);
    }
}

static void SetMouseVisible(bool visible)
{
    ShowCursor(visible);
}

static void SetWindowTitle(const char* title)
{
    SetWindowTextA(g_window.hnd, title);
}

static bool WindowIsActive()
{
    return GetActiveWindow() == g_window.hnd;
}

static bool WindowIsOpen()
{
    return g_window.is_open;
}

static void CloseWindow()
{
    g_window.is_open = false;
}

static Window* GetWindow()
{
    if (!WindowIsOpen())
    {
        CTK_FATAL("can't get window: window has not been opened with OpenWindow()");
    }

    return &g_window;
}

