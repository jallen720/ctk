/// Macros
////////////////////////////////////////////////////////////
#define CTK_ANSI_RESET         "\x1b[0m"
#define CTK_ANSI_COLOR_RED     "\x1b[31m"
#define CTK_ANSI_COLOR_GREEN   "\x1b[32m"
#define CTK_ANSI_COLOR_YELLOW  "\x1b[33m"
#define CTK_ANSI_COLOR_MAGENTA "\x1b[35m"
#define CTK_ANSI_COLOR_SKY     "\x1b[36m"
#define CTK_ANSI_COLOR_BLUE    "\x1b[34m"

#define CTK_ANSI_HIGHLIGHT(COLOR, STR) CTK_ANSI_COLOR_ ## COLOR STR CTK_ANSI_RESET
#define CTK_INFO_TAG    CTK_ANSI_HIGHLIGHT(GREEN, "INFO") ": "
#define CTK_INFO_NL     "\n      "
#define CTK_WARNING_TAG CTK_ANSI_HIGHLIGHT(MAGENTA, "WARNING") ": "
#define CTK_WARNING_NL  "\n         "
#define CTK_ERROR_TAG   CTK_ANSI_HIGHLIGHT(RED, "ERROR") ": "
#define CTK_ERROR_NL    "\n       "

/// Data
////////////////////////////////////////////////////////////
enum struct OutputColor {
    RED,
    GREEN,
    YELLOW,
    MAGENTA,
    SKY,
    BLUE,
    COUNT,
};

/// Interface
////////////////////////////////////////////////////////////
void PrintColor(OutputColor output_color) {
    constexpr const char* COLOR_CODES[(uint32)OutputColor::COUNT] = {
        CTK_ANSI_COLOR_RED,
        CTK_ANSI_COLOR_GREEN,
        CTK_ANSI_COLOR_YELLOW,
        CTK_ANSI_COLOR_MAGENTA,
        CTK_ANSI_COLOR_SKY,
        CTK_ANSI_COLOR_BLUE,
    };
    printf(COLOR_CODES[(uint32)output_color]);
}

void PrintColorReset() {
    printf(CTK_ANSI_RESET);
}

void Print(char c) {
    putchar(c);
}

void Print(OutputColor output_color, char c) {
    PrintColor(output_color);
    putchar(c);
    PrintColorReset();
}

template<typename ...Args>
void Print(const char* msg, Args... args) {
    printf(msg, args...);
}

template<typename ...Args>
void Print(OutputColor output_color, const char* msg, Args... args) {
    PrintColor(output_color);
    printf(msg, args...);
    PrintColorReset();
}

void PrintLine() {
    Print("\n");
}

template<typename ...Args>
void PrintLine(const char* msg, Args... args) {
    Print(msg, args...);
    PrintLine();
}

template<typename ...Args>
void PrintLine(OutputColor output_color, const char* msg, Args... args) {
    Print(output_color, msg, args...);
    PrintLine();
}

template<typename ...Args>
void PrintInline(const char* msg, Args... args) {
    constexpr sint32 BUFFER_SIZE = 128;
    char buffer[BUFFER_SIZE] = {};
    sint32 chars_written = snprintf(buffer, BUFFER_SIZE, msg, args...);
    memset(buffer + chars_written, ' ', BUFFER_SIZE - chars_written);
    Print('\r');
    Print("%.*s", BUFFER_SIZE, buffer);
}

template<typename ...Args>
void PrintInline(OutputColor output_color, const char* msg, Args... args) {
    constexpr sint32 BUFFER_SIZE = 128;
    char buffer[BUFFER_SIZE] = {};
    sint32 chars_written = snprintf(buffer, BUFFER_SIZE, msg, args...);
    memset(buffer + chars_written, ' ', BUFFER_SIZE - chars_written);
    Print('\r');
    Print(output_color, "%.*s", BUFFER_SIZE, buffer);
}

void PrintLines(uint32 count) {
    CTK_REPEAT(count) {
        Print("\n");
    }
}

void PrintTabs(uint32 count, uint32 tab_size = 4) {
    static constexpr const char* TAB_BUFFER =
        "                                                                "
        "                                                                "
        "                                                                "
        "                                                                ";
    CTK_REPEAT(count) {
        Print("%.*s", tab_size, TAB_BUFFER);
    }
}

void PrintSpaces(uint32 count) {
    CTK_REPEAT(count) {
        Print(' ');
    }
}

template<typename ...Args>
void PrintError(const char* msg, Args... args) {
    Print(CTK_ERROR_TAG);
    PrintLine(msg, args...);
}

template<typename ...Args>
void PrintInfo(const char* msg, Args... args) {
    Print(CTK_INFO_TAG);
    PrintLine(msg, args...);
}

template<typename ...Args>
void PrintWarning(const char* msg, Args... args) {
    Print(CTK_WARNING_TAG);
    PrintLine(msg, args...);
}
