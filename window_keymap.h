/// Data
////////////////////////////////////////////////////////////
enum struct Key {
    NONE,
    NUM_0,
    NUM_1,
    NUM_2,
    NUM_3,
    NUM_4,
    NUM_5,
    NUM_6,
    NUM_7,
    NUM_8,
    NUM_9,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    MOUSE_0,
    MOUSE_1,
    CANCEL,
    MOUSE_2,
    MOUSE_3,
    MOUSE_4,
    BACKSPACE,
    TAB,
    CLEAR,
    RETURN,
    SHIFT,
    CONTROL,
    MENU,
    PAUSE,
    CAPSLOCK,
    KANA,
    HANGEUL,
    HANGUL,
    JUNJA,
    FINAL,
    HANJA,
    KANJI,
    ESCAPE,
    CONVERT,
    NONCONVERT,
    ACCEPT,
    MODECHANGE,
    SPACE,
    PRIOR,
    NEXT,
    END,
    HOME,
    LEFT,
    UP,
    RIGHT,
    DOWN,
    SELECT,
    PRINT,
    EXECUTE_,
    SNAPSHOT,
    INSERT,
    DEL,
    HELP,
    LWIN,
    RWIN,
    APPS,
    SLEEP,
    NUMPAD_0,
    NUMPAD_1,
    NUMPAD_2,
    NUMPAD_3,
    NUMPAD_4,
    NUMPAD_5,
    NUMPAD_6,
    NUMPAD_7,
    NUMPAD_8,
    NUMPAD_9,
    MULTIPLY,
    ADD,
    SEPARATOR,
    SUBTRACT,
    DECIMAL,
    DIVIDE,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    F13,
    F14,
    F15,
    F16,
    F17,
    F18,
    F19,
    F20,
    F21,
    F22,
    F23,
    F24,
    NAVIGATION_VIEW,
    NAVIGATION_MENU,
    NAVIGATION_UP,
    NAVIGATION_DOWN,
    NAVIGATION_LEFT,
    NAVIGATION_RIGHT,
    NAVIGATION_ACCEPT,
    NAVIGATION_CANCEL,
    NUMLOCK,
    SCROLL,
    NUMPAD_EQUAL,
    FJ_JISHO,
    FJ_MASSHOU,
    FJ_TOUROKU,
    FJ_LOYA,
    FJ_ROYA,
    LSHIFT,
    RSHIFT,
    LCONTROL,
    RCONTROL,
    LMENU,
    RMENU,
    BROWSER_BACK,
    BROWSER_FORWARD,
    BROWSER_REFRESH,
    BROWSER_STOP,
    BROWSER_SEARCH,
    BROWSER_FAVORITES,
    BROWSER_HOME,
    VOLUME_MUTE,
    VOLUME_DOWN,
    VOLUME_UP,
    MEDIA_NEXT_TRACK,
    MEDIA_PREV_TRACK,
    MEDIA_STOP,
    MEDIA_PLAY_PAUSE,
    LAUNCH_MAIL,
    LAUNCH_MEDIA_SELECT,
    LAUNCH_APP1,
    LAUNCH_APP2,
    SEMICOLON_COLON,
    PLUS,
    COMMA,
    MINUS,
    PERIOD,
    SLASH_QUESTION,
    BACKTICK_TILDE,
    GAMEPAD_A,
    GAMEPAD_B,
    GAMEPAD_X,
    GAMEPAD_Y,
    GAMEPAD_RIGHT_SHOULDER,
    GAMEPAD_LEFT_SHOULDER,
    GAMEPAD_LEFT_TRIGGER,
    GAMEPAD_RIGHT_TRIGGER,
    GAMEPAD_DPAD_UP,
    GAMEPAD_DPAD_DOWN,
    GAMEPAD_DPAD_LEFT,
    GAMEPAD_DPAD_RIGHT,
    GAMEPAD_MENU,
    GAMEPAD_VIEW,
    GAMEPAD_LEFT_THUMBSTICK_BUTTON,
    GAMEPAD_RIGHT_THUMBSTICK_BUTTON,
    GAMEPAD_LEFT_THUMBSTICK_UP,
    GAMEPAD_LEFT_THUMBSTICK_DOWN,
    GAMEPAD_LEFT_THUMBSTICK_RIGHT,
    GAMEPAD_LEFT_THUMBSTICK_LEFT,
    GAMEPAD_RIGHT_THUMBSTICK_UP,
    GAMEPAD_RIGHT_THUMBSTICK_DOWN,
    GAMEPAD_RIGHT_THUMBSTICK_RIGHT,
    GAMEPAD_RIGHT_THUMBSTICK_LEFT,
    OPEN_BRACKET,
    BACKSLASH_PIPE,
    CLOSE_BRACKET,
    APOSTROPHE_QUOTE,
    OEM_8,
    OEM_AX,
    OEM_102,
    ICO_HELP,
    ICO_00,
    COUNT,
};

// Not defined by win32; defined here for consistency.
#define VK_NONE  0
#define VK_NUM_0 0x30
#define VK_NUM_1 0x31
#define VK_NUM_2 0x32
#define VK_NUM_3 0x33
#define VK_NUM_4 0x34
#define VK_NUM_5 0x35
#define VK_NUM_6 0x36
#define VK_NUM_7 0x37
#define VK_NUM_8 0x38
#define VK_NUM_9 0x39
#define VK_A     0x41
#define VK_B     0x42
#define VK_C     0x43
#define VK_D     0x44
#define VK_E     0x45
#define VK_F     0x46
#define VK_G     0x47
#define VK_H     0x48
#define VK_I     0x49
#define VK_J     0x4A
#define VK_K     0x4B
#define VK_L     0x4C
#define VK_M     0x4D
#define VK_N     0x4E
#define VK_O     0x4F
#define VK_P     0x50
#define VK_Q     0x51
#define VK_R     0x52
#define VK_S     0x53
#define VK_T     0x54
#define VK_U     0x55
#define VK_V     0x56
#define VK_W     0x57
#define VK_X     0x58
#define VK_Y     0x59
#define VK_Z     0x5A
#define VK_COUNT VK_ICO_00 + 1

constexpr uint32 KEY_TO_VIRTUAL_KEY[(uint32)Key::COUNT] = {
    VK_NONE, // KEY_NONE

    // Alpha-numeric Keys
    VK_NUM_0, // KEY_NUM_0
    VK_NUM_1, // KEY_NUM_1
    VK_NUM_2, // KEY_NUM_2
    VK_NUM_3, // KEY_NUM_3
    VK_NUM_4, // KEY_NUM_4
    VK_NUM_5, // KEY_NUM_5
    VK_NUM_6, // KEY_NUM_6
    VK_NUM_7, // KEY_NUM_7
    VK_NUM_8, // KEY_NUM_8
    VK_NUM_9, // KEY_NUM_9
    VK_A,     // KEY_A
    VK_B,     // KEY_B
    VK_C,     // KEY_C
    VK_D,     // KEY_D
    VK_E,     // KEY_E
    VK_F,     // KEY_F
    VK_G,     // KEY_G
    VK_H,     // KEY_H
    VK_I,     // KEY_I
    VK_J,     // KEY_J
    VK_K,     // KEY_K
    VK_L,     // KEY_L
    VK_M,     // KEY_M
    VK_N,     // KEY_N
    VK_O,     // KEY_O
    VK_P,     // KEY_P
    VK_Q,     // KEY_Q
    VK_R,     // KEY_R
    VK_S,     // KEY_S
    VK_T,     // KEY_T
    VK_U,     // KEY_U
    VK_V,     // KEY_V
    VK_W,     // KEY_W
    VK_X,     // KEY_X
    VK_Y,     // KEY_Y
    VK_Z,     // KEY_Z

    // Mouse Buttons
    VK_LBUTTON, // KEY_MOUSE_0
    VK_RBUTTON, // KEY_MOUSE_1
    VK_CANCEL,  // KEY_CANCEL
    VK_MBUTTON, // KEY_MOUSE_2
    #if(_WIN32_WINNT >= 0x0500)
        VK_XBUTTON1, // KEY_MOUSE_3
        VK_XBUTTON2, // KEY_MOUSE_4
    #endif // _WIN32_WINNT >= 0x0500

    VK_BACK,       // KEY_BACKSPACE
    VK_TAB,        // KEY_TAB
    VK_CLEAR,      // KEY_CLEAR
    VK_RETURN,     // KEY_RETURN
    VK_SHIFT,      // KEY_SHIFT
    VK_CONTROL,    // KEY_CONTROL
    VK_MENU,       // KEY_MENU
    VK_PAUSE,      // KEY_PAUSE
    VK_CAPITAL,    // KEY_CAPSLOCK
    VK_KANA,       // KEY_KANA
    VK_HANGEUL,    // KEY_HANGEUL
    VK_HANGUL,     // KEY_HANGUL
    VK_JUNJA,      // KEY_JUNJA
    VK_FINAL,      // KEY_FINAL
    VK_HANJA,      // KEY_HANJA
    VK_KANJI,      // KEY_KANJI
    VK_ESCAPE,     // KEY_ESCAPE
    VK_CONVERT,    // KEY_CONVERT
    VK_NONCONVERT, // KEY_NONCONVERT
    VK_ACCEPT,     // KEY_ACCEPT
    VK_MODECHANGE, // KEY_MODECHANGE
    VK_SPACE,      // KEY_SPACE
    VK_PRIOR,      // KEY_PRIOR
    VK_NEXT,       // KEY_NEXT
    VK_END,        // KEY_END
    VK_HOME,       // KEY_HOME
    VK_LEFT,       // KEY_LEFT
    VK_UP,         // KEY_UP
    VK_RIGHT,      // KEY_RIGHT
    VK_DOWN,       // KEY_DOWN
    VK_SELECT,     // KEY_SELECT
    VK_PRINT,      // KEY_PRINT
    VK_EXECUTE,    // KEY_EXECUTE
    VK_SNAPSHOT,   // KEY_SNAPSHOT
    VK_INSERT,     // KEY_INSERT
    VK_DELETE,     // KEY_DEL
    VK_HELP,       // KEY_HELP
    VK_LWIN,       // KEY_LWIN
    VK_RWIN,       // KEY_RWIN
    VK_APPS,       // KEY_APPS
    VK_SLEEP,      // KEY_SLEEP
    VK_NUMPAD0,    // KEY_NUMPAD_0
    VK_NUMPAD1,    // KEY_NUMPAD_1
    VK_NUMPAD2,    // KEY_NUMPAD_2
    VK_NUMPAD3,    // KEY_NUMPAD_3
    VK_NUMPAD4,    // KEY_NUMPAD_4
    VK_NUMPAD5,    // KEY_NUMPAD_5
    VK_NUMPAD6,    // KEY_NUMPAD_6
    VK_NUMPAD7,    // KEY_NUMPAD_7
    VK_NUMPAD8,    // KEY_NUMPAD_8
    VK_NUMPAD9,    // KEY_NUMPAD_9
    VK_MULTIPLY,   // KEY_MULTIPLY
    VK_ADD,        // KEY_ADD
    VK_SEPARATOR,  // KEY_SEPARATOR
    VK_SUBTRACT,   // KEY_SUBTRACT
    VK_DECIMAL,    // KEY_DECIMAL
    VK_DIVIDE,     // KEY_DIVIDE
    VK_F1,         // KEY_F1
    VK_F2,         // KEY_F2
    VK_F3,         // KEY_F3
    VK_F4,         // KEY_F4
    VK_F5,         // KEY_F5
    VK_F6,         // KEY_F6
    VK_F7,         // KEY_F7
    VK_F8,         // KEY_F8
    VK_F9,         // KEY_F9
    VK_F10,        // KEY_F10
    VK_F11,        // KEY_F11
    VK_F12,        // KEY_F12
    VK_F13,        // KEY_F13
    VK_F14,        // KEY_F14
    VK_F15,        // KEY_F15
    VK_F16,        // KEY_F16
    VK_F17,        // KEY_F17
    VK_F18,        // KEY_F18
    VK_F19,        // KEY_F19
    VK_F20,        // KEY_F20
    VK_F21,        // KEY_F21
    VK_F22,        // KEY_F22
    VK_F23,        // KEY_F23
    VK_F24,        // KEY_F24
    #if(_WIN32_WINNT >= 0x0604)
        VK_NAVIGATION_VIEW,   // KEY_NAVIGATION_VIEW
        VK_NAVIGATION_MENU,   // KEY_NAVIGATION_MENU
        VK_NAVIGATION_UP,     // KEY_NAVIGATION_UP
        VK_NAVIGATION_DOWN,   // KEY_NAVIGATION_DOWN
        VK_NAVIGATION_LEFT,   // KEY_NAVIGATION_LEFT
        VK_NAVIGATION_RIGHT,  // KEY_NAVIGATION_RIGHT
        VK_NAVIGATION_ACCEPT, // KEY_NAVIGATION_ACCEPT
        VK_NAVIGATION_CANCEL, // KEY_NAVIGATION_CANCEL
    #endif // _WIN32_WINNT >= 0x0604
    VK_NUMLOCK,        // KEY_NUMLOCK
    VK_SCROLL,         // KEY_SCROLL
    VK_OEM_NEC_EQUAL,  // KEY_NUMPAD_EQUAL
    VK_OEM_FJ_JISHO,   // KEY_FJ_JISHO
    VK_OEM_FJ_MASSHOU, // KEY_FJ_MASSHOU
    VK_OEM_FJ_TOUROKU, // KEY_FJ_TOUROKU
    VK_OEM_FJ_LOYA,    // KEY_FJ_LOYA
    VK_OEM_FJ_ROYA,    // KEY_FJ_ROYA
    VK_LSHIFT,         // KEY_LSHIFT
    VK_RSHIFT,         // KEY_RSHIFT
    VK_LCONTROL,       // KEY_LCONTROL
    VK_RCONTROL,       // KEY_RCONTROL
    VK_LMENU,          // KEY_LMENU
    VK_RMENU,          // KEY_RMENU
    #if(_WIN32_WINNT >= 0x0500)
        VK_BROWSER_BACK,        // KEY_BROWSER_BACK
        VK_BROWSER_FORWARD,     // KEY_BROWSER_FORWARD
        VK_BROWSER_REFRESH,     // KEY_BROWSER_REFRESH
        VK_BROWSER_STOP,        // KEY_BROWSER_STOP
        VK_BROWSER_SEARCH,      // KEY_BROWSER_SEARCH
        VK_BROWSER_FAVORITES,   // KEY_BROWSER_FAVORITES
        VK_BROWSER_HOME,        // KEY_BROWSER_HOME
        VK_VOLUME_MUTE,         // KEY_VOLUME_MUTE
        VK_VOLUME_DOWN,         // KEY_VOLUME_DOWN
        VK_VOLUME_UP,           // KEY_VOLUME_UP
        VK_MEDIA_NEXT_TRACK,    // KEY_MEDIA_NEXT_TRACK
        VK_MEDIA_PREV_TRACK,    // KEY_MEDIA_PREV_TRACK
        VK_MEDIA_STOP,          // KEY_MEDIA_STOP
        VK_MEDIA_PLAY_PAUSE,    // KEY_MEDIA_PLAY_PAUSE
        VK_LAUNCH_MAIL,         // KEY_LAUNCH_MAIL
        VK_LAUNCH_MEDIA_SELECT, // KEY_LAUNCH_MEDIA_SELECT
        VK_LAUNCH_APP1,         // KEY_LAUNCH_APP1
        VK_LAUNCH_APP2,         // KEY_LAUNCH_APP2
    #endif // _WIN32_WINNT >= 0x0500
    VK_OEM_1,      // KEY_SEMICOLON_COLON
    VK_OEM_PLUS,   // KEY_PLUS
    VK_OEM_COMMA,  // KEY_COMMA
    VK_OEM_MINUS,  // KEY_MINUS
    VK_OEM_PERIOD, // KEY_PERIOD
    VK_OEM_2,      // KEY_SLASH_QUESTION
    VK_OEM_3,      // KEY_BACKTICK_TILDE
    #if(_WIN32_WINNT >= 0x0604)
        // Gamepad Input
        VK_GAMEPAD_A,                       // KEY_GAMEPAD_A
        VK_GAMEPAD_B,                       // KEY_GAMEPAD_B
        VK_GAMEPAD_X,                       // KEY_GAMEPAD_X
        VK_GAMEPAD_Y,                       // KEY_GAMEPAD_Y
        VK_GAMEPAD_RIGHT_SHOULDER,          // KEY_GAMEPAD_RIGHT_SHOULDER
        VK_GAMEPAD_LEFT_SHOULDER,           // KEY_GAMEPAD_LEFT_SHOULDER
        VK_GAMEPAD_LEFT_TRIGGER,            // KEY_GAMEPAD_LEFT_TRIGGER
        VK_GAMEPAD_RIGHT_TRIGGER,           // KEY_GAMEPAD_RIGHT_TRIGGER
        VK_GAMEPAD_DPAD_UP,                 // KEY_GAMEPAD_DPAD_UP
        VK_GAMEPAD_DPAD_DOWN,               // KEY_GAMEPAD_DPAD_DOWN
        VK_GAMEPAD_DPAD_LEFT,               // KEY_GAMEPAD_DPAD_LEFT
        VK_GAMEPAD_DPAD_RIGHT,              // KEY_GAMEPAD_DPAD_RIGHT
        VK_GAMEPAD_MENU,                    // KEY_GAMEPAD_MENU
        VK_GAMEPAD_VIEW,                    // KEY_GAMEPAD_VIEW
        VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON,  // KEY_GAMEPAD_LEFT_THUMBSTICK_BUTTON
        VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON, // KEY_GAMEPAD_RIGHT_THUMBSTICK_BUTTON
        VK_GAMEPAD_LEFT_THUMBSTICK_UP,      // KEY_GAMEPAD_LEFT_THUMBSTICK_UP
        VK_GAMEPAD_LEFT_THUMBSTICK_DOWN,    // KEY_GAMEPAD_LEFT_THUMBSTICK_DOWN
        VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT,   // KEY_GAMEPAD_LEFT_THUMBSTICK_RIGHT
        VK_GAMEPAD_LEFT_THUMBSTICK_LEFT,    // KEY_GAMEPAD_LEFT_THUMBSTICK_LEFT
        VK_GAMEPAD_RIGHT_THUMBSTICK_UP,     // KEY_GAMEPAD_RIGHT_THUMBSTICK_UP
        VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN,   // KEY_GAMEPAD_RIGHT_THUMBSTICK_DOWN
        VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT,  // KEY_GAMEPAD_RIGHT_THUMBSTICK_RIGHT
        VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT,   // KEY_GAMEPAD_RIGHT_THUMBSTICK_LEFT
    #endif // _WIN32_WINNT >= 0x0604

    VK_OEM_4, // KEY_OPEN_BRACKET
    VK_OEM_5, // KEY_BACKSLASH_PIPE
    VK_OEM_6, // KEY_CLOSE_BRACKET
    VK_OEM_7, // KEY_APOSTROPHE_QUOTE
    VK_OEM_8, // KEY_OEM_8

    // Various Extended or Enhanced Keyboards
    VK_OEM_AX,   // KEY_OEM_AX
    VK_OEM_102,  // KEY_OEM_102
    VK_ICO_HELP, // KEY_ICO_HELP
    VK_ICO_00,   // KEY_ICO_00
};

constexpr const char* KEY_NAMES[(uint32)Key::COUNT] = {
    "NONE",
    "NUM_0",
    "NUM_1",
    "NUM_2",
    "NUM_3",
    "NUM_4",
    "NUM_5",
    "NUM_6",
    "NUM_7",
    "NUM_8",
    "NUM_9",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
    "MOUSE_0",
    "MOUSE_1",
    "CANCEL",
    "MOUSE_2",
    "MOUSE_3",
    "MOUSE_4",
    "BACKSPACE",
    "TAB",
    "CLEAR",
    "RETURN",
    "SHIFT",
    "CONTROL",
    "MENU",
    "PAUSE",
    "CAPSLOCK",
    "KANA",
    "HANGEUL",
    "HANGUL",
    "JUNJA",
    "FINAL",
    "HANJA",
    "KANJI",
    "ESCAPE",
    "CONVERT",
    "NONCONVERT",
    "ACCEPT",
    "MODECHANGE",
    "SPACE",
    "PRIOR",
    "NEXT",
    "END",
    "HOME",
    "LEFT",
    "UP",
    "RIGHT",
    "DOWN",
    "SELECT",
    "PRINT",
    "EXECUTE",
    "SNAPSHOT",
    "INSERT",
    "DEL",
    "HELP",
    "LWIN",
    "RWIN",
    "APPS",
    "SLEEP",
    "NUMPAD_0",
    "NUMPAD_1",
    "NUMPAD_2",
    "NUMPAD_3",
    "NUMPAD_4",
    "NUMPAD_5",
    "NUMPAD_6",
    "NUMPAD_7",
    "NUMPAD_8",
    "NUMPAD_9",
    "MULTIPLY",
    "ADD",
    "SEPARATOR",
    "SUBTRACT",
    "DECIMAL",
    "DIVIDE",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",
    "F13",
    "F14",
    "F15",
    "F16",
    "F17",
    "F18",
    "F19",
    "F20",
    "F21",
    "F22",
    "F23",
    "F24",
    "NAVIGATION_VIEW",
    "NAVIGATION_MENU",
    "NAVIGATION_UP",
    "NAVIGATION_DOWN",
    "NAVIGATION_LEFT",
    "NAVIGATION_RIGHT",
    "NAVIGATION_ACCEPT",
    "NAVIGATION_CANCEL",
    "NUMLOCK",
    "SCROLL",
    "NUMPAD_EQUAL",
    "FJ_JISHO",
    "FJ_MASSHOU",
    "FJ_TOUROKU",
    "FJ_LOYA",
    "FJ_ROYA",
    "LSHIFT",
    "RSHIFT",
    "LCONTROL",
    "RCONTROL",
    "LMENU",
    "RMENU",
    "BROWSER_BACK",
    "BROWSER_FORWARD",
    "BROWSER_REFRESH",
    "BROWSER_STOP",
    "BROWSER_SEARCH",
    "BROWSER_FAVORITES",
    "BROWSER_HOME",
    "VOLUME_MUTE",
    "VOLUME_DOWN",
    "VOLUME_UP",
    "MEDIA_NEXT_TRACK",
    "MEDIA_PREV_TRACK",
    "MEDIA_STOP",
    "MEDIA_PLAY_PAUSE",
    "LAUNCH_MAIL",
    "LAUNCH_MEDIA_SELECT",
    "LAUNCH_APP1",
    "LAUNCH_APP2",
    "SEMICOLON_COLON",
    "PLUS",
    "COMMA",
    "MINUS",
    "PERIOD",
    "SLASH_QUESTION",
    "BACKTICK_TILDE",
    "GAMEPAD_A",
    "GAMEPAD_B",
    "GAMEPAD_X",
    "GAMEPAD_Y",
    "GAMEPAD_RIGHT_SHOULDER",
    "GAMEPAD_LEFT_SHOULDER",
    "GAMEPAD_LEFT_TRIGGER",
    "GAMEPAD_RIGHT_TRIGGER",
    "GAMEPAD_DPAD_UP",
    "GAMEPAD_DPAD_DOWN",
    "GAMEPAD_DPAD_LEFT",
    "GAMEPAD_DPAD_RIGHT",
    "GAMEPAD_MENU",
    "GAMEPAD_VIEW",
    "GAMEPAD_LEFT_THUMBSTICK_BUTTON",
    "GAMEPAD_RIGHT_THUMBSTICK_BUTTON",
    "GAMEPAD_LEFT_THUMBSTICK_UP",
    "GAMEPAD_LEFT_THUMBSTICK_DOWN",
    "GAMEPAD_LEFT_THUMBSTICK_RIGHT",
    "GAMEPAD_LEFT_THUMBSTICK_LEFT",
    "GAMEPAD_RIGHT_THUMBSTICK_UP",
    "GAMEPAD_RIGHT_THUMBSTICK_DOWN",
    "GAMEPAD_RIGHT_THUMBSTICK_RIGHT",
    "GAMEPAD_RIGHT_THUMBSTICK_LEFT",
    "OPEN_BRACKET",
    "BACKSLASH_PIPE",
    "CLOSE_BRACKET",
    "APOSTROPHE_QUOTE",
    "OEM_8",
    "OEM_AX",
    "OEM_102",
    "ICO_HELP",
    "ICO_00",
};

constexpr Key VIRTUAL_KEY_TO_KEY[VK_COUNT] = {
    Key::NONE,
    Key::MOUSE_0,
    Key::MOUSE_1,
    Key::CANCEL,
    Key::MOUSE_2,
    Key::MOUSE_3,
    Key::MOUSE_4,
    Key::NONE,
    Key::BACKSPACE,
    Key::TAB,
    Key::NONE,
    Key::NONE,
    Key::CLEAR,
    Key::RETURN,
    Key::NONE,
    Key::NONE,
    Key::SHIFT,
    Key::CONTROL,
    Key::MENU,
    Key::PAUSE,
    Key::CAPSLOCK,
    Key::HANGUL,
    Key::NONE,
    Key::JUNJA,
    Key::FINAL,
    Key::KANJI,
    Key::NONE,
    Key::ESCAPE,
    Key::CONVERT,
    Key::NONCONVERT,
    Key::ACCEPT,
    Key::MODECHANGE,
    Key::SPACE,
    Key::PRIOR,
    Key::NEXT,
    Key::END,
    Key::HOME,
    Key::LEFT,
    Key::UP,
    Key::RIGHT,
    Key::DOWN,
    Key::SELECT,
    Key::PRINT,
    Key::EXECUTE_,
    Key::SNAPSHOT,
    Key::INSERT,
    Key::DEL,
    Key::HELP,
    Key::NUM_0,
    Key::NUM_1,
    Key::NUM_2,
    Key::NUM_3,
    Key::NUM_4,
    Key::NUM_5,
    Key::NUM_6,
    Key::NUM_7,
    Key::NUM_8,
    Key::NUM_9,
    Key::NONE,
    Key::NONE,
    Key::NONE,
    Key::NONE,
    Key::NONE,
    Key::NONE,
    Key::NONE,
    Key::A,
    Key::B,
    Key::C,
    Key::D,
    Key::E,
    Key::F,
    Key::G,
    Key::H,
    Key::I,
    Key::J,
    Key::K,
    Key::L,
    Key::M,
    Key::N,
    Key::O,
    Key::P,
    Key::Q,
    Key::R,
    Key::S,
    Key::T,
    Key::U,
    Key::V,
    Key::W,
    Key::X,
    Key::Y,
    Key::Z,
    Key::LWIN,
    Key::RWIN,
    Key::APPS,
    Key::NONE,
    Key::SLEEP,
    Key::NUMPAD_0,
    Key::NUMPAD_1,
    Key::NUMPAD_2,
    Key::NUMPAD_3,
    Key::NUMPAD_4,
    Key::NUMPAD_5,
    Key::NUMPAD_6,
    Key::NUMPAD_7,
    Key::NUMPAD_8,
    Key::NUMPAD_9,
    Key::MULTIPLY,
    Key::ADD,
    Key::SEPARATOR,
    Key::SUBTRACT,
    Key::DECIMAL,
    Key::DIVIDE,
    Key::F1,
    Key::F2,
    Key::F3,
    Key::F4,
    Key::F5,
    Key::F6,
    Key::F7,
    Key::F8,
    Key::F9,
    Key::F10,
    Key::F11,
    Key::F12,
    Key::F13,
    Key::F14,
    Key::F15,
    Key::F16,
    Key::F17,
    Key::F18,
    Key::F19,
    Key::F20,
    Key::F21,
    Key::F22,
    Key::F23,
    Key::F24,
    Key::NAVIGATION_VIEW,
    Key::NAVIGATION_MENU,
    Key::NAVIGATION_UP,
    Key::NAVIGATION_DOWN,
    Key::NAVIGATION_LEFT,
    Key::NAVIGATION_RIGHT,
    Key::NAVIGATION_ACCEPT,
    Key::NAVIGATION_CANCEL,
    Key::NUMLOCK,
    Key::SCROLL,
    Key::FJ_JISHO,
    Key::FJ_MASSHOU,
    Key::FJ_TOUROKU,
    Key::FJ_LOYA,
    Key::FJ_ROYA,
    Key::NONE,
    Key::NONE,
    Key::NONE,
    Key::NONE,
    Key::NONE,
    Key::NONE,
    Key::NONE,
    Key::NONE,
    Key::NONE,
    Key::LSHIFT,
    Key::RSHIFT,
    Key::LCONTROL,
    Key::RCONTROL,
    Key::LMENU,
    Key::RMENU,
    Key::BROWSER_BACK,
    Key::BROWSER_FORWARD,
    Key::BROWSER_REFRESH,
    Key::BROWSER_STOP,
    Key::BROWSER_SEARCH,
    Key::BROWSER_FAVORITES,
    Key::BROWSER_HOME,
    Key::VOLUME_MUTE,
    Key::VOLUME_DOWN,
    Key::VOLUME_UP,
    Key::MEDIA_NEXT_TRACK,
    Key::MEDIA_PREV_TRACK,
    Key::MEDIA_STOP,
    Key::MEDIA_PLAY_PAUSE,
    Key::LAUNCH_MAIL,
    Key::LAUNCH_MEDIA_SELECT,
    Key::LAUNCH_APP1,
    Key::LAUNCH_APP2,
    Key::NONE,
    Key::NONE,
    Key::SEMICOLON_COLON,
    Key::PLUS,
    Key::COMMA,
    Key::MINUS,
    Key::PERIOD,
    Key::SLASH_QUESTION,
    Key::BACKTICK_TILDE,
    Key::NONE,
    Key::NONE,
    Key::GAMEPAD_A,
    Key::GAMEPAD_B,
    Key::GAMEPAD_X,
    Key::GAMEPAD_Y,
    Key::GAMEPAD_RIGHT_SHOULDER,
    Key::GAMEPAD_LEFT_SHOULDER,
    Key::GAMEPAD_LEFT_TRIGGER,
    Key::GAMEPAD_RIGHT_TRIGGER,
    Key::GAMEPAD_DPAD_UP,
    Key::GAMEPAD_DPAD_DOWN,
    Key::GAMEPAD_DPAD_LEFT,
    Key::GAMEPAD_DPAD_RIGHT,
    Key::GAMEPAD_MENU,
    Key::GAMEPAD_VIEW,
    Key::GAMEPAD_LEFT_THUMBSTICK_BUTTON,
    Key::GAMEPAD_RIGHT_THUMBSTICK_BUTTON,
    Key::GAMEPAD_LEFT_THUMBSTICK_UP,
    Key::GAMEPAD_LEFT_THUMBSTICK_DOWN,
    Key::GAMEPAD_LEFT_THUMBSTICK_RIGHT,
    Key::GAMEPAD_LEFT_THUMBSTICK_LEFT,
    Key::GAMEPAD_RIGHT_THUMBSTICK_UP,
    Key::GAMEPAD_RIGHT_THUMBSTICK_DOWN,
    Key::GAMEPAD_RIGHT_THUMBSTICK_RIGHT,
    Key::GAMEPAD_RIGHT_THUMBSTICK_LEFT,
    Key::OPEN_BRACKET,
    Key::BACKSLASH_PIPE,
    Key::CLOSE_BRACKET,
    Key::APOSTROPHE_QUOTE,
    Key::OEM_8,
    Key::NONE,
    Key::OEM_AX,
    Key::OEM_102,
    Key::ICO_HELP,
    Key::ICO_00,
};

constexpr const char* VIRTUAL_KEY_NAMES[VK_COUNT] = {
    "VK_NONE",
    "VK_LBUTTON",
    "VK_RBUTTON",
    "VK_CANCEL",
    "VK_MBUTTON",
    #if(_WIN32_WINNT >= 0x0500)
        "VK_XBUTTON1",
        "VK_XBUTTON2",
    #else
        "VK_NONE",
        "VK_NONE",
    #endif // _WIN32_WINNT >= 0x0500
    "VK_NONE",
    "VK_BACK",
    "VK_TAB",
    "VK_NONE",
    "VK_NONE",
    "VK_CLEAR",
    "VK_RETURN",
    "VK_NONE",
    "VK_NONE",
    "VK_SHIFT",
    "VK_CONTROL",
    "VK_MENU",
    "VK_PAUSE",
    "VK_CAPITAL",
    "VK_HANGUL",
    "VK_NONE",
    "VK_JUNJA",
    "VK_FINAL",
    "VK_KANJI",
    "VK_NONE",
    "VK_ESCAPE",
    "VK_CONVERT",
    "VK_NONCONVERT",
    "VK_ACCEPT",
    "VK_MODECHANGE",
    "VK_SPACE",
    "VK_PRIOR",
    "VK_NEXT",
    "VK_END",
    "VK_HOME",
    "VK_LEFT",
    "VK_UP",
    "VK_RIGHT",
    "VK_DOWN",
    "VK_SELECT",
    "VK_PRINT",
    "VK_EXECUTE",
    "VK_SNAPSHOT",
    "VK_INSERT",
    "VK_DELETE",
    "VK_HELP",
    "VK_NUM_0",
    "VK_NUM_1",
    "VK_NUM_2",
    "VK_NUM_3",
    "VK_NUM_4",
    "VK_NUM_5",
    "VK_NUM_6",
    "VK_NUM_7",
    "VK_NUM_8",
    "VK_NUM_9",
    "VK_NONE",
    "VK_NONE",
    "VK_NONE",
    "VK_NONE",
    "VK_NONE",
    "VK_NONE",
    "VK_NONE",
    "VK_A",
    "VK_B",
    "VK_C",
    "VK_D",
    "VK_E",
    "VK_F",
    "VK_G",
    "VK_H",
    "VK_I",
    "VK_J",
    "VK_K",
    "VK_L",
    "VK_M",
    "VK_N",
    "VK_O",
    "VK_P",
    "VK_Q",
    "VK_R",
    "VK_S",
    "VK_T",
    "VK_U",
    "VK_V",
    "VK_W",
    "VK_X",
    "VK_Y",
    "VK_Z",
    "VK_LWIN",
    "VK_RWIN",
    "VK_APPS",
    "VK_NONE",
    "VK_SLEEP",
    "VK_NUMPAD0",
    "VK_NUMPAD1",
    "VK_NUMPAD2",
    "VK_NUMPAD3",
    "VK_NUMPAD4",
    "VK_NUMPAD5",
    "VK_NUMPAD6",
    "VK_NUMPAD7",
    "VK_NUMPAD8",
    "VK_NUMPAD9",
    "VK_MULTIPLY",
    "VK_ADD",
    "VK_SEPARATOR",
    "VK_SUBTRACT",
    "VK_DECIMAL",
    "VK_DIVIDE",
    "VK_F1",
    "VK_F2",
    "VK_F3",
    "VK_F4",
    "VK_F5",
    "VK_F6",
    "VK_F7",
    "VK_F8",
    "VK_F9",
    "VK_F10",
    "VK_F11",
    "VK_F12",
    "VK_F13",
    "VK_F14",
    "VK_F15",
    "VK_F16",
    "VK_F17",
    "VK_F18",
    "VK_F19",
    "VK_F20",
    "VK_F21",
    "VK_F22",
    "VK_F23",
    "VK_F24",
    #if(_WIN32_WINNT >= 0x0604)
        "VK_NAVIGATION_VIEW",
        "VK_NAVIGATION_MENU",
        "VK_NAVIGATION_UP",
        "VK_NAVIGATION_DOWN",
        "VK_NAVIGATION_LEFT",
        "VK_NAVIGATION_RIGHT",
        "VK_NAVIGATION_ACCEPT",
        "VK_NAVIGATION_CANCEL",
    #else
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
    #endif // _WIN32_WINNT >= 0x0604
    "VK_NUMLOCK",
    "VK_SCROLL",
    "VK_OEM_FJ_JISHO",
    "VK_OEM_FJ_MASSHOU",
    "VK_OEM_FJ_TOUROKU",
    "VK_OEM_FJ_LOYA",
    "VK_OEM_FJ_ROYA",
    "VK_NONE",
    "VK_NONE",
    "VK_NONE",
    "VK_NONE",
    "VK_NONE",
    "VK_NONE",
    "VK_NONE",
    "VK_NONE",
    "VK_NONE",
    "VK_LSHIFT",
    "VK_RSHIFT",
    "VK_LCONTROL",
    "VK_RCONTROL",
    "VK_LMENU",
    "VK_RMENU",
    #if(_WIN32_WINNT >= 0x0500)
        "VK_BROWSER_BACK",
        "VK_BROWSER_FORWARD",
        "VK_BROWSER_REFRESH",
        "VK_BROWSER_STOP",
        "VK_BROWSER_SEARCH",
        "VK_BROWSER_FAVORITES",
        "VK_BROWSER_HOME",
        "VK_VOLUME_MUTE",
        "VK_VOLUME_DOWN",
        "VK_VOLUME_UP",
        "VK_MEDIA_NEXT_TRACK",
        "VK_MEDIA_PREV_TRACK",
        "VK_MEDIA_STOP",
        "VK_MEDIA_PLAY_PAUSE",
        "VK_LAUNCH_MAIL",
        "VK_LAUNCH_MEDIA_SELECT",
        "VK_LAUNCH_APP1",
        "VK_LAUNCH_APP2",
    #else
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
    #endif // _WIN32_WINNT >= 0x0500
    "VK_NONE",
    "VK_NONE",
    "VK_OEM_1",
    "VK_OEM_PLUS",
    "VK_OEM_COMMA",
    "VK_OEM_MINUS",
    "VK_OEM_PERIOD",
    "VK_OEM_2",
    "VK_OEM_3",
    "VK_NONE",
    "VK_NONE",
    #if(_WIN32_WINNT >= 0x0604)
        "VK_GAMEPAD_A",
        "VK_GAMEPAD_B",
        "VK_GAMEPAD_X",
        "VK_GAMEPAD_Y",
        "VK_GAMEPAD_RIGHT_SHOULDER",
        "VK_GAMEPAD_LEFT_SHOULDER",
        "VK_GAMEPAD_LEFT_TRIGGER",
        "VK_GAMEPAD_RIGHT_TRIGGER",
        "VK_GAMEPAD_DPAD_UP",
        "VK_GAMEPAD_DPAD_DOWN",
        "VK_GAMEPAD_DPAD_LEFT",
        "VK_GAMEPAD_DPAD_RIGHT",
        "VK_GAMEPAD_MENU",
        "VK_GAMEPAD_VIEW",
        "VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON",
        "VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON",
        "VK_GAMEPAD_LEFT_THUMBSTICK_UP",
        "VK_GAMEPAD_LEFT_THUMBSTICK_DOWN",
        "VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT",
        "VK_GAMEPAD_LEFT_THUMBSTICK_LEFT",
        "VK_GAMEPAD_RIGHT_THUMBSTICK_UP",
        "VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN",
        "VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT",
        "VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT",
    #else
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
        "VK_NONE",
    #endif // _WIN32_WINNT >= 0x0604
    "VK_OEM_4",
    "VK_OEM_5",
    "VK_OEM_6",
    "VK_OEM_7",
    "VK_OEM_8",
    "VK_NONE",
    "VK_OEM_AX",
    "VK_OEM_102",
    "VK_ICO_HELP",
    "VK_ICO_00",
};

/// Interface
////////////////////////////////////////////////////////////
const char* KeyName(Key key) {
    return KEY_NAMES[(uint32)key];
}

Key GetKey(uint32 virtual_key) {
    return VIRTUAL_KEY_TO_KEY[virtual_key];
}

const char* VirtualKeyName(uint32 key) {
    return VIRTUAL_KEY_NAMES[key];
}

uint32 GetVirtualKey(Key key) {
    return KEY_TO_VIRTUAL_KEY[(uint32)key];
}

