/// Interface
////////////////////////////////////////////////////////////
uint32 StringSize(const char* nt_string) {
    if (nt_string == NULL) {
        return 0;
    }

    uint32 size = 0;
    while (nt_string[size] != '\0') {
        size += 1;
    }
    return size;
}

bool StringsMatch(const char* string_a, const char* string_b, uint32 match_size) {
    for (uint32 i = 0; i < match_size; i += 1) {
        if (string_a[i] != string_b[i]) {
            return false;
        }
    }

    return true;
}

bool StringsMatch(const char* string_a, uint32 string_a_size,
                  const char* string_b, uint32 string_b_size)
{
    if (string_a_size != string_b_size) {
        return false;
    }

    return StringsMatch(string_a, string_b, string_a_size);
}

bool StringsMatch(const char* nt_string_a, const char* nt_string_b) {
    return StringsMatch(nt_string_a, StringSize(nt_string_a), nt_string_b, StringSize(nt_string_b));
}

bool StringsMatch(const char* string_a, uint32 string_a_size, const char* nt_string_b) {
    return StringsMatch(string_a, string_a_size, nt_string_b, StringSize(nt_string_b));
}

bool IsSubstring(const char* string,    uint32 string_size,
                 const char* substring, uint32 substring_size)
{
    if (string_size < substring_size) {
        return false;
    }

    return StringsMatch(string, substring, substring_size);
}

bool IsSubstring(const char* nt_string, const char* nt_substring) {
    return IsSubstring(nt_string, StringSize(nt_string), nt_substring, StringSize(nt_substring));
}

bool IsSubstring(const char* string, uint32 string_size, const char* nt_substring) {
    return IsSubstring(string, string_size, nt_substring, StringSize(nt_substring));
}

bool Contains(const char* string, uint32 string_size, char c) {
    for (uint32 i = 0; i < string_size; i += 1) {
        if (string[i] == c) {
            return true;
        }
    }

    return false;
}

template<typename FloatType>
FloatType ToFloat(const char* string, uint32 size) {
    // Skip leading spaces.
    uint32 index = 0;
    while (index < size) {
        if (string[index] != ' ') {
            break;
        }
        index += 1;
    }

    bool is_negative = false;
    if (index < size && string[index] == '-') {
        is_negative = true;
        index += 1;
    }

    FloatType value = 0.0f;
    bool decimal_found = false;
    FloatType base = 1;
    while (index < size) {
        char c = string[index];

        if (c == '.') {
            if (decimal_found) {
                CTK_FATAL("invalid string for parsing float: multiple decimals found");
            }
            decimal_found = true;
        }
        else if (!ASCII_NUMERIC[c]) {
            break;
        }
        else {
            if (decimal_found) {
                base *= 10;
            }
            value *= 10;
            value += c - 48;
        }

        index += 1;
    }

    return (value / base) * (is_negative ? -1 : 1);
}

template<typename FloatType>
FloatType ToFloat(const char* nt_string) {
    return ToFloat<FloatType>(nt_string, StringSize(nt_string));
}

#define CSTRING_TO_FLOAT_FUNC(BITS) \
float##BITS ToFloat##BITS(const char* string, uint32 size) { \
    return ToFloat<float##BITS>(string, size); \
} \
float##BITS ToFloat##BITS(const char* nt_string) { \
    return ToFloat<float##BITS>(nt_string, StringSize(nt_string)); \
}
CSTRING_TO_FLOAT_FUNC(32)
CSTRING_TO_FLOAT_FUNC(64)

template<typename IntType>
IntType ToInt(const char* string, uint32 size) {
    // Skip leading spaces.
    uint32 index = 0;
    while (index < size) {
        if (string[index] != ' ') {
            break;
        }
        index += 1;
    }

    bool is_negative = false;
    if (index < size && string[index] == '-') {
        is_negative = true;
        index += 1;
    }

    IntType value = 0;
    while (index < size) {
        char c = string[index];
        if (!ASCII_NUMERIC[c]) {
            // Skip all non-numeric characters at end of string, including trailing spaces.
            break;
        }

        value *= 10;
        value += c - 48;

        index += 1;
    }

    return value * (is_negative ? -1 : 1);
}

template<typename IntType>
IntType ToInt(const char* nt_string) {
    return ToInt<IntType>(nt_string, StringSize(nt_string));
}

#define CSTRING_TO_INT_FUNC(SIGN, SIGN_UPPER, BITS) \
SIGN##int##BITS To##SIGN_UPPER##Int##BITS(const char* string, uint32 size) { \
    return ToInt<SIGN##int##BITS>(string, size); \
} \
SIGN##int##BITS To##SIGN_UPPER##Int##BITS(const char* nt_string) { \
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

bool ToBool(const char* string, uint32 size) {
    if (size == 4) {
        if (StringsMatch(string, size, "true")) {
            return true; 
        }
    }
    else if (size == 5) {
        if (StringsMatch(string, size, "false")) {
            return false;
        }
    }

    CTK_FATAL("string '%.*s' can't be converted to a boolean value: must be 'true' or 'false'", size, string);
}

bool ToBool(const char* nt_string) {
    return ToBool(nt_string, StringSize(nt_string));
}

template<typename ...Args>
sint32 Write(char* string, uint32 max_string_size, const char* fmt_string, Args... args) {
    return snprintf(string, max_string_size, fmt_string, args...);
}
