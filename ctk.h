#pragma once

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>

typedef uint8_t         u8;
typedef uint16_t        u16;
typedef uint32_t        u32;
typedef uint64_t        u64;
typedef int8_t          s8;
typedef int16_t         s16;
typedef int32_t         s32;
typedef int64_t         s64;
typedef float           f32;
typedef double          f64;
typedef char const *    cstr;

#define CTK_S8_MIN  SCHAR_MIN
#define CTK_S8_MAX  SCHAR_MAX
#define CTK_U8_MAX  UCHAR_MAX
#define CTK_S16_MIN SHRT_MIN
#define CTK_S16_MAX SHRT_MAX
#define CTK_U16_MAX USHRT_MAX
#define CTK_S32_MIN INT_MIN
#define CTK_S32_MAX INT_MAX
#define CTK_U32_MAX UINT_MAX
#define CTK_S64_MIN LLONG_MIN
#define CTK_S64_MAX LLONG_MAX
#define CTK_U64_MAX ULLONG_MAX

#define CTK_ANSI_RESET          "\x1b[0m"
#define CTK_ANSI_COLOR_RED      "\x1b[31m"
#define CTK_ANSI_COLOR_GREEN    "\x1b[32m"
#define CTK_ANSI_COLOR_YELLOW   "\x1b[33m"
#define CTK_ANSI_COLOR_MAGENTA  "\x1b[35m"
#define CTK_ANSI_COLOR_SKY      "\x1b[36m"
#define CTK_ANSI_COLOR_BLUE     "\x1b[34m"

#define CTK_KILOBYTE 1000
#define CTK_MEGABYTE 1000 * CTK_KILOBYTE
#define CTK_GIGABYTE 1000 * CTK_MEGABYTE

#define CTK_CACHE_LINE 64
#define CTK_ANSI_HIGHLIGHT(STR, COLOR) CTK_ANSI_COLOR_ ## COLOR STR CTK_ANSI_RESET
#define CTK_ERROR_TAG CTK_ANSI_HIGHLIGHT("ERROR", RED) ": "

#ifdef __GNUC__
    #define CTK_FATAL(MSG, ...) {\
        ctk_print(CTK_ERROR_TAG);\
        ctk_print_line(MSG, ## __VA_ARGS__);\
        throw 0;\
    }
    #define CTK_TODO(MSG, ...) {\
        ctk_print(CTK_ANSI_HIGHLIGHT("TODO", RED) "[");\
        ctk_print("file: " CTK_ANSI_HIGHLIGHT("%s", SKY) " ", __FILE__);\
        ctk_print("line: " CTK_ANSI_HIGHLIGHT("%i", SKY) " ", __LINE__);\
        ctk_print("func: " CTK_ANSI_HIGHLIGHT("%s()", SKY) "]: ", __FUNCTION__);\
        ctk_print_line(MSG, ## __VA_ARGS__);\
    }
#else
    #define CTK_FATAL(MSG, ...) {\
        ctk_print(CTK_ERROR_TAG);\
        ctk_print_line(MSG, __VA_ARGS__);\
        throw 0;\
    }
    #define CTK_TODO(MSG, ...) {\
        ctk_print(CTK_ANSI_HIGHLIGHT("TODO", RED) "[");\
        ctk_print("file: " CTK_ANSI_HIGHLIGHT("%s", SKY) " ", __FILE__);\
        ctk_print("line: " CTK_ANSI_HIGHLIGHT("%i", SKY) " ", __LINE__);\
        ctk_print("func: " CTK_ANSI_HIGHLIGHT("%s()", SKY) "]: ", __FUNCTION__);\
        ctk_print_line(MSG, __VA_ARGS__);\
    }
#endif

#define CTK_ARRAY_COUNT(ARRAY) sizeof(ARRAY) / sizeof(ARRAY[0])
#define CTK_ASSERT(STATEMENT) if (!(STATEMENT)) { CTK_FATAL("assertion \"%s\" failed", #STATEMENT); }
#define CTK_NAME_VALUE_PAIR(VALUE) { #VALUE, VALUE }
#define CTK_VALUE_NAME_PAIR(VALUE) { VALUE, #VALUE }
#define CTK_REPEAT(COUNT) for (u32 _ = 0; _ < COUNT; ++_)
#define CTK_TO_CSTR(TEXT) #TEXT

////////////////////////////////////////////////////////////
/// Data
////////////////////////////////////////////////////////////
template<typename Type>
struct CTK_Optional {
    bool set;
    Type value;
    operator bool();
    CTK_Optional<Type> &operator=(Type value);
};

template<typename Key, typename Value>
struct CTK_Pair {
    Key key;
    Value value;
};

////////////////////////////////////////////////////////////
/// Logging
////////////////////////////////////////////////////////////
template<typename ...Args>
static void ctk_print(cstr msg, Args... args) {
    printf(msg, args...);
}

template<typename ...Args>
static void ctk_print_line() {
    ctk_print("\n");
}

template<typename ...Args>
static void ctk_print_line(cstr msg, Args... args) {
    ctk_print(msg, args...);
    ctk_print_line();
}

static void ctk_print_tabs(u32 tabs) {
    CTK_REPEAT(tabs) {
        ctk_print("    ");
    }
}

template<typename ...Args>
static void ctk_print(u32 tabs, cstr msg, Args... args) {
    ctk_print_tabs(tabs);
    ctk_print(msg, args...);
}

template<typename ...Args>
static void ctk_print_line(u32 tabs, cstr msg, Args... args) {
    ctk_print_tabs(tabs);
    ctk_print_line(msg, args...);
}

template<typename ...Args>
static void ctk_error(cstr msg, Args... args) {
    ctk_print(CTK_ERROR_TAG);
    ctk_print_line(msg, args...);
}

template<typename ...Args>
static void ctk_error(u32 tabs, cstr msg, Args... args) {
    ctk_print(CTK_ERROR_TAG);
    ctk_print_tabs(tabs);
    ctk_print_line(msg, args...);
}

template<typename ...Args>
static void ctk_info(cstr msg, Args... args) {
    ctk_print(CTK_ANSI_HIGHLIGHT("INFO", GREEN) ": ");
    ctk_print_line(msg, args...);
}

template<typename ...Args>
static void ctk_info(u32 tabs, cstr msg, Args... args) {
    ctk_print(CTK_ANSI_HIGHLIGHT("INFO", GREEN) ": ");
    ctk_print_tabs(tabs);
    ctk_print_line(msg, args...);
}

template<typename ...Args>
static void ctk_warning(cstr msg, Args... args) {
    ctk_print(CTK_ANSI_HIGHLIGHT("WARNING", MAGENTA) ": ");
    ctk_print_line(msg, args...);
}

template<typename ...Args>
static void ctk_warning(u32 tabs, cstr msg, Args... args) {
    ctk_print(CTK_ANSI_HIGHLIGHT("WARNING", MAGENTA) ": ");
    ctk_print_tabs(tabs);
    ctk_print_line(msg, args...);
}

////////////////////////////////////////////////////////////
/// Debugging
////////////////////////////////////////////////////////////
template<typename Type>
void ctk_print_bits(Type val) {
    auto val_bytes = (u8 *)&val;

    for (s32 i = sizeof(Type) - 1; i >= 0; --i) {
        for (s32 j = 7; j >= 0; --j)
            ctk_print("%u", (val_bytes[i] >> j) & 1);

        ctk_print(" ");
    }
}

////////////////////////////////////////////////////////////
/// C-String
////////////////////////////////////////////////////////////
static bool ctk_strings_match(cstr a, u64 a_size, cstr b, u64 b_size) {
    if (a_size != b_size)
        return false;

    for (u64 i = 0; i < a_size; ++i)
        if (a[i] != b[i])
            return false;

    return true;
}

static bool ctk_strings_match(cstr a, cstr b) {
    return ctk_strings_match(a, strlen(a), b, strlen(b));
}

static f32 ctk_f32(cstr s) {
    return strtof(s, NULL);
}

static f32 ctk_f64(cstr s) {
    return strtod(s, NULL);
}

static s32 ctk_s32(cstr s) {
    return strtol(s, NULL, 10);
}

static s32 ctk_s64(cstr s) {
    return strtoll(s, NULL, 10);
}

static u32 ctk_u32(cstr s) {
    return strtoul(s, NULL, 10);
}

static u32 ctk_u64(cstr s) {
    return strtoull(s, NULL, 10);
}

static bool ctk_bool(cstr s) {
    if (ctk_strings_match(s, "true"))
        return true;

    if (ctk_strings_match(s, "false"))
        return false;

    CTK_FATAL("string \"%s\" cannot be converted to a boolean value", s);
}

////////////////////////////////////////////////////////////
/// Optional
////////////////////////////////////////////////////////////
template<typename Type>
static Type *ctk_set(CTK_Optional<Type> *opt, Type val) {
    opt->value = val;
    opt->set = true;
    return &opt->value;
}

template<typename Type>
CTK_Optional<Type> &CTK_Optional<Type>::operator=(Type val) {
    this->value = val;
    this->set = true;
    return *this;
}

////////////////////////////////////////////////////////////
/// Pair
////////////////////////////////////////////////////////////
template<typename Key, typename Value>
static Value ctk_find_value(Key key, CTK_Pair<Key, Value> *pairs, u32 pair_count) {
    for (u32 i = 0; i < pair_count; ++i)
        if (pairs[i].key == key)
            return pairs[i].value;

    return {};
}

template<typename Key, typename Value>
static Value ctk_get_value(Key key, CTK_Pair<Key, Value> *pairs, u32 pair_count) {
    Value res = ctk_find_value(key, pairs, pair_count);
    if (res == NULL)
        CTK_FATAL("failed to get value from pairs by key");
    return res;
}

template<typename Key, typename Value>
static Key ctk_find_key(Value value, CTK_Pair<Key, Value> *pairs, u32 pair_count) {
    for (u32 i = 0; i < pair_count; ++i)
        if (pairs[i].value == value)
            return pairs[i].key;

    return {};
}

template<typename Key, typename Value>
static Key ctk_get_key(Value value, CTK_Pair<Key, Value> *pairs, u32 pair_count) {
    Key res = ctk_find_key(value, pairs, pair_count);
    if (res == NULL)
        CTK_FATAL("failed to get key from pairs by value");
    return res;
}
