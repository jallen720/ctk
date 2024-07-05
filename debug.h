/// Macros
////////////////////////////////////////////////////////////
#define CTK_FATAL(MSG, ...) \
{ \
    CTK::Print(CTK_ERROR_TAG); \
    CTK::PrintLine(MSG, __VA_ARGS__); \
    throw 0; \
}

#define CTK_TODO(MSG, ...) \
{ \
    CTK::PrintLine(CTK_ANSI_HIGHLIGHT(MAGENTA, "TODO") ": " MSG, __VA_ARGS__); \
    CTK::PrintLine("    " CTK_ANSI_HIGHLIGHT(SKY, "file") ": %s", __FILE__); \
    CTK::PrintLine("    " CTK_ANSI_HIGHLIGHT(SKY, "line") ": %i", __LINE__); \
}

#define CTK_ASSERT(STATEMENT) if (!(STATEMENT)) { CTK_FATAL("assertion '%s' failed", #STATEMENT); }

/// Utils
////////////////////////////////////////////////////////////
static uint32 GetASCIICharWidth(char c)
{
    if (c == '\n')
    {
        return 2;
    }
    else if (c == '\r')
    {
        return 2;
    }
    else if (c == '\t')
    {
        return 2;
    }
    else if (c == '\0')
    {
        return 2;
    }
    else if (c >= 32 && c < 127)
    {
        return 1;
    }
    else
    {
        return 1;
    }
}

static void PrintASCIICharSingle(char c, char non_single_char)
{
    Print(c >= 32 && c < 127 ? c : non_single_char);
}

/// Interface
////////////////////////////////////////////////////////////
void PrintBits(void* bytes, uint32 size)
{
    if (size == 0)
    {
        return;
    }

    for (uint32 byte_index = 0; byte_index <= size - 1; ++byte_index)
    {
        for (uint32 bit_index = 0; bit_index < 8; ++bit_index)
        {
            Print("%u", (((uint8*)bytes)[byte_index] >> (7 - bit_index)) & 1);
        }
        Print(" ");
    }
}

template<typename Type>
void PrintBits(Type data)
{
    PrintBits(&data, sizeof(Type));
}

template<typename ...Args>
void PrintBitsLine(void* bytes, uint32 size)
{
    PrintBits(bytes, size);
    PrintLine();
}

template<typename ...Args>
void PrintBitsLine(void* bytes, uint32 size, const char* fmt, Args... args)
{
    Print(fmt, args...);
    PrintBitsLine(bytes, size);
}

template<typename Type, typename ...Args>
void PrintBitsLine(Type data)
{
    PrintBits(&data, sizeof(Type));
    PrintLine();
}

template<typename Type, typename ...Args>
void PrintBitsLine(Type data, const char* fmt, Args... args)
{
    Print(fmt, args...);
    PrintBitsLine(data);
}

static void PrintBytes(uint8* bytes, uint32 size, uint32 width = 32)
{
    uint32 col = 0;
    for (uint32 i = 0; i < size; ++i)
    {
        PrintASCIICharSingle((char)bytes[i], '.');
        if ((i + 1) % 4 == 0)
        {
            ++col;
            if (col >= width)
            {
                PrintLine();
                col = 0;
            }
            else
            {
                Print(' ');
            }
        }
    }
}

static void PrintByteValues(uint8* bytes, uint32 size, uint32 width = 8)
{
    uint32 col = 0;
    for (uint32 i = 0; i < size; ++i)
    {
        Print("\\%03u", bytes[i]);
        if ((i + 1) % 4 == 0)
        {
            ++col;
            if (col >= width)
            {
                PrintLine();
                col = 0;
            }
            else
            {
                Print(' ');
            }
        }
    }
}

static void PrintASCIIChar(char c)
{
    if (c == '\n')
    {
        Print("\\n");
    }
    else if (c == '\r')
    {
        Print("\\r");
    }
    else if (c == '\t')
    {
        Print("\\t");
    }
    else if (c == '\0')
    {
        Print("\\0");
    }
    else if (c >= 32 && c < 127)
    {
        Print(c);
    }
    else
    {
        Print(' ');
    }
}

static void PrintASCIICharUniform(char c)
{
    if (c >= 32 && c < 127)
    {
        Print("%c ", c);
    }
    else if (c == '\n')
    {
        Print("\\n");
    }
    else if (c == '\r')
    {
        Print("\\r");
    }
    else if (c == '\t')
    {
        Print("\\t");
    }
    else if (c == '\0')
    {
        Print("\\0");
    }
    else
    {
        Print("  ");
    }
}

static void PrintASCIIString(const char* ascii_string, uint32 size, uint32 max_width)
{
    uint32 width = 0;
    for (uint32 i = 0; i < size; ++i)
    {
        uint32 ascii_char_width = GetASCIICharWidth(ascii_string[i]);
        if (width + ascii_char_width >= max_width)
        {
            PrintLine();
            width = 0;
        }
        PrintASCIIChar(ascii_string[i]);
        width += ascii_char_width;
    }
}

static void PrintASCIIStringUniform(const char* ascii_string, uint32 size, uint32 max_width)
{
    uint32 width = 0;
    for (uint32 i = 0; i < size; ++i)
    {
        PrintASCIICharUniform(ascii_string[i]);
        width += 2;
        if (width >= max_width)
        {
            PrintLine();
            width = 0;
        }
    }
}

static void PrintASCIIStringSingle(const char* ascii_string, uint32 size, uint32 max_width, char non_single_char)
{
    uint32 width = 0;
    for (uint32 i = 0; i < size; ++i)
    {
        PrintASCIICharSingle(ascii_string[i], non_single_char);
        width += 1;
        if (width >= max_width)
        {
            PrintLine();
            width = 0;
        }
    }
}

static void PrintASCIIString(OutputColor output_color, const char* ascii_string, uint32 size, uint32 max_width)
{
    PrintColor(output_color);
    PrintASCIIString(ascii_string, size, max_width);
    PrintColorReset();
}

static void PrintASCIIStringUniform(OutputColor output_color, const char* ascii_string, uint32 size, uint32 max_width)
{
    PrintColor(output_color);
    PrintASCIIStringUniform(ascii_string, size, max_width);
    PrintColorReset();
}

static void PrintASCIIStringSingle(OutputColor output_color, const char* ascii_string, uint32 size,
                                   uint32 max_width, char non_single_char)
{
    PrintColor(output_color);
    PrintASCIIStringSingle(ascii_string, size, max_width, non_single_char);
    PrintColorReset();
}




