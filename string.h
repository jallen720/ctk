/// Macros
////////////////////////////////////////////////////////////
#define CTK_WRAP_STRING(STRING) WrapString(STRING, CTK_ARRAY_SIZE(STRING))
#define CTK_WRAP_STRING_1(PTR)  WrapString(PTR, 1)

/// Data
////////////////////////////////////////////////////////////
using String = Array<char>;

/// C-String Interface
////////////////////////////////////////////////////////////
static uint32 StringSize(const char* nt_string)
{
    if (nt_string == NULL)
    {
        return 0;
    }

    uint32 size = 0;
    while (nt_string[size] != '\0')
    {
        ++size;
    }
    return size;
}

static bool StringsMatch(const char* a, const char* b, uint32 match_size, uint32 match_start = 0)
{
    for (uint32 i = 0; i < match_size; ++i)
    {
        if (a[i] != b[i])
        {
            return false;
        }
    }

    return true;
}

static bool StringsMatch(const char* a, uint32 a_size, const char* b, uint32 b_size)
{
    return a_size == b_size && StringsMatch(a, b, a_size);
}

static bool StringsMatch(const char* a, const char* b)
{
    return StringsMatch(a, StringSize(a), b, StringSize(b));
}

static bool Contains(const char* string, uint32 string_size, char c)
{
    for (uint32 i = 0; i < string_size; ++i)
    {
        if (string[i] == c)
        {
            return true;
        }
    }

    return false;
}

static bool Contains(const char* nt_string, char c)
{
    return Contains(nt_string, StringSize(nt_string), c);
}

template<typename FloatType>
static FloatType ToFloat(const char* string, uint32 size)
{
    // Skip leading spaces.
    uint32 index = 0;
    while (index < size)
    {
        if (string[index] != ' ') { break; }
        index += 1;
    }

    bool is_negative = false;
    if (index < size && string[index] == '-')
    {
        is_negative = true;
        index += 1;
    }

    FloatType value = 0.0f;
    bool decimal_found = false;
    FloatType base = 1;
    while (index < size)
    {
        char c = string[index];

        if (c == '.')
        {
            if (decimal_found) { CTK_FATAL("invalid string for parsing float: multiple decimals found"); }
            decimal_found = true;
        }
        else if (!IS_NUMERIC[c])
        {
            break;
        }
        else
        {
            if (decimal_found) { base *= 10; }
            value *= 10;
            value += c - 48;
        }

        index += 1;
    }

    return (value / base) * (is_negative ? -1 : 1);
}

template<typename FloatType>
static FloatType ToFloat(const char* nt_string)
{
    return ToFloat<FloatType>(nt_string, StringSize(nt_string));
}

#define CSTRING_TO_FLOAT_FUNC(BITS) \
static float##BITS ToFloat##BITS(const char* string, uint32 size) \
{ \
    return ToFloat<float##BITS>(string, size); \
} \
static float##BITS ToFloat##BITS(const char* nt_string) \
{ \
    return ToFloat<float##BITS>(nt_string, StringSize(nt_string)); \
}
CSTRING_TO_FLOAT_FUNC(32)
CSTRING_TO_FLOAT_FUNC(64)

template<typename IntType>
static IntType ToInt(const char* string, uint32 size)
{
    // Skip leading spaces.
    uint32 index = 0;
    while (index < size)
    {
        if (string[index] != ' ') { break; }
        index += 1;
    }

    bool is_negative = false;
    if (index < size && string[index] == '-')
    {
        is_negative = true;
        index += 1;
    }

    IntType value = 0;
    while (index < size)
    {
        char c = string[index];
        if (!IS_NUMERIC[c]) { break; } // Skip all non-numeric characters at end of string, including trailing spaces.

        value *= 10;
        value += c - 48;

        index += 1;
    }

    return value * (is_negative ? -1 : 1);
}

template<typename IntType>
static IntType ToInt(const char* nt_string)
{
    return ToInt<IntType>(nt_string, StringSize(nt_string));
}

#define CSTRING_TO_INT_FUNC(SIGN, SIGN_UPPER, BITS) \
static SIGN##int##BITS To##SIGN_UPPER##Int##BITS(const char* string, uint32 size) \
{ \
    return ToInt<SIGN##int##BITS>(string, size); \
} \
static SIGN##int##BITS To##SIGN_UPPER##Int##BITS(const char* nt_string) \
{ \
    return ToInt<SIGN##int##BITS>(nt_string, StringSize(nt_string)); \
}
CSTRING_TO_INT_FUNC(s, S, 8)
CSTRING_TO_INT_FUNC(s, S, 16)
CSTRING_TO_INT_FUNC(s, S, 32)
CSTRING_TO_INT_FUNC(s, S, 64)
CSTRING_TO_INT_FUNC(u, U, 8)
CSTRING_TO_INT_FUNC(u, U, 16)
CSTRING_TO_INT_FUNC(u, U, 32)
CSTRING_TO_INT_FUNC(u, U, 64)

static bool ToBool(const char* string, uint32 size)
{
    if (size == 4)
    {
        if (StringsMatch(string, "true", 4))  { return true;  }
    }
    else if (size == 5)
    {
        if (StringsMatch(string, "false", 5)) { return false; }
    }

    CTK_FATAL("string '%.*s' can't be converted to a boolean value: must be 'true' or 'false'", size, string);
}

static bool ToBool(const char* nt_string)
{
    return ToBool(nt_string, StringSize(nt_string));
}

template<typename ...Args>
static sint32 Write(char* string, uint32 max_string_size, const char* fmt_string, Args... args)
{
    return snprintf(string, max_string_size, fmt_string, args...);
}

/// String Interface
////////////////////////////////////////////////////////////
static String CreateString(Allocator* allocator, uint32 size)
{
    return CreateArray<char>(allocator, size);
}

static String CreateString(Allocator* allocator, const char* nt_src_string)
{
    return CreateArray(allocator, nt_src_string, StringSize(nt_src_string));
}

static String CreateString(Allocator* allocator, const char* src_string, uint32 size)
{
   return CreateArray(allocator, src_string, size);
}

static String CreateString(Allocator* allocator, String* src_string)
{
    return CreateArray(allocator, src_string);
}

static String CreateStringFull(Allocator* allocator, uint32 size)
{
    return CreateArrayFull<char>(allocator, size);
}

static void DestroyString(String* string, Allocator* allocator)
{
    DestroyArray(string, allocator);
}

static String WrapString(char* string, uint32 size)
{
    return WrapArray(string, size);
}

static void PushRange(String* string, const char* nt_src_string)
{
    PushRange(string, nt_src_string, StringSize(nt_src_string));
}

template<typename ...Args>
static void Write(String* string, const char* fmt_string, Args... args)
{
    uint32 chars_to_write = (uint32)snprintf(string->data, string->size, fmt_string, args...);
    string->count = chars_to_write <= string->size ? chars_to_write : string->size;
}

template<typename ...Args>
static void Append(String* string, const char* fmt_string, Args... args)
{
    uint32 remaining_space = string->size - string->count;
    uint32 chars_to_append = (uint32)snprintf(string->data + string->count, remaining_space, fmt_string, args...);
    string->count += chars_to_append <= remaining_space ? chars_to_append : remaining_space;
}

static bool StringsMatch(String* str_a, const char* nt_string)
{
    return StringsMatch(str_a->data, str_a->count, nt_string, StringSize(nt_string));
}

static bool StringsMatch(String* str_a, String* str_b)
{
    return StringsMatch(str_a->data, str_a->count, str_b->data, str_b->count);
}

static bool StringsMatch(String* str_a, const char* str_b, uint32 match_size, uint32 match_start = 0)
{
    return StringsMatch(str_a->data, str_b, match_size, match_start);
}

static bool StringsMatch(String* str_a, String* str_b, uint32 match_size, uint32 match_start = 0)
{
    return StringsMatch(str_a->data, str_b->data, match_size, match_start);
}

template<typename FloatType>
static FloatType ToFloat(String* string)
{
    return ToFloat<FloatType>(string->data, string->count);
}

#define STRING_TO_FLOAT_FUNC(BITS) \
static float##BITS ToFloat##BITS(String* string) \
{ \
    return ToFloat##BITS(string->data, string->count); \
}
STRING_TO_FLOAT_FUNC(32)
STRING_TO_FLOAT_FUNC(64)

template<typename IntType>
static IntType ToInt(String* string)
{
    return ToInt<IntType>(string->data, string->count);
}

#define STRING_TO_INT_FUNC(SIGN, SIGN_UPPER, BITS) \
static SIGN##int##BITS To##SIGN_UPPER##Int##BITS(String* string) \
{ \
    return To##SIGN_UPPER##Int##BITS(string->data, string->count); \
}
STRING_TO_INT_FUNC(s, S, 8)
STRING_TO_INT_FUNC(s, S, 16)
STRING_TO_INT_FUNC(s, S, 32)
STRING_TO_INT_FUNC(s, S, 64)
STRING_TO_INT_FUNC(u, U, 8)
STRING_TO_INT_FUNC(u, U, 16)
STRING_TO_INT_FUNC(u, U, 32)
STRING_TO_INT_FUNC(u, U, 64)

static bool ToBool(String* string)
{
    if (StringsMatch(string, "true"))  { return true;  }
    if (StringsMatch(string, "false")) { return false; }

    CTK_FATAL("string '%.*s' can't be converted to a boolean value: must be 'true' or 'false'", string->data, string->count);
}

