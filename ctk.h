#pragma once

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstring>

////////////////////////////////////////////////////////////
/// Types
////////////////////////////////////////////////////////////
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;
using f32 = float;
using f64 = double;
using cstr = const char *;
using b32 = bool;

////////////////////////////////////////////////////////////
/// Macros
////////////////////////////////////////////////////////////
#define CTK_S8_MIN SCHAR_MIN
#define CTK_S8_MAX SCHAR_MAX
#define CTK_U8_MAX UCHAR_MAX
#define CTK_S16_MIN SHRT_MIN
#define CTK_S16_MAX SHRT_MAX
#define CTK_U16_MAX USHRT_MAX
#define CTK_S32_MIN INT_MIN
#define CTK_S32_MAX INT_MAX
#define CTK_U32_MAX UINT_MAX
#define CTK_S64_MIN LLONG_MIN
#define CTK_S64_MAX LLONG_MAX
#define CTK_U64_MAX ULLONG_MAX
#define CTK_ANSI_RESET "\x1b[0m"
#define CTK_ANSI_COLOR_RED "\x1b[31m"
#define CTK_ANSI_COLOR_GREEN "\x1b[32m"
#define CTK_ANSI_COLOR_MAGENTA "\x1b[35m"
#define CTK_ANSI_COLOR_BLUE "\u001b[36m"
#define CTK_ANSI_HIGHLIGHT(STR, COLOR) CTK_ANSI_COLOR_ ## COLOR STR CTK_ANSI_RESET
#define CTK_ARRAY_COUNT(ARRAY) sizeof(ARRAY) / sizeof(ARRAY[0])
#define CTK_ERROR_TAG CTK_ANSI_HIGHLIGHT("ERROR", RED) ": "

#ifdef __GNUC__
    #define CTK_FATAL(MESSAGE, ...) { \
        ctk::print_line(CTK_ERROR_TAG CTK_ANSI_HIGHLIGHT("file", BLUE) ": %s", __FILE__); \
        ctk::print_line("       " CTK_ANSI_HIGHLIGHT("line", BLUE) ": %i", __LINE__); \
        ctk::print_line("       " CTK_ANSI_HIGHLIGHT("func", BLUE) ": %s()", __FUNCTION__); \
        ctk::print("       " CTK_ANSI_HIGHLIGHT("msg", BLUE) ": "); \
        ctk::print_line(MESSAGE, ## __VA_ARGS__); \
        throw 0; \
    }
#else
    #define CTK_FATAL(MESSAGE, ...) { \
        ctk::print_line(CTK_ERROR_TAG CTK_ANSI_HIGHLIGHT("file", BLUE) ": %s", __FILE__); \
        ctk::print_line("       " CTK_ANSI_HIGHLIGHT("line", BLUE) ": %i", __LINE__); \
        ctk::print_line("       " CTK_ANSI_HIGHLIGHT("func", BLUE) ": %s()", __FUNCTION__); \
        ctk::print("       " CTK_ANSI_HIGHLIGHT("msg", BLUE) ": "); \
        ctk::print_line(MESSAGE, __VA_ARGS__); \
        throw 0; |
    }
#endif

#define CTK_ASSERT(STATEMENT) if(!(STATEMENT)) { CTK_FATAL("assertion \"%s\" failed", #STATEMENT) }
#define CTK_NAME_VALUE_PAIR(VALUE) { #VALUE, VALUE }
#define CTK_VALUE_NAME_PAIR(VALUE) { VALUE, #VALUE }
#define CTK_ITERATE(COUNT) for(u32 IterationIndex = 0; IterationIndex < COUNT; IterationIndex++)
#define CTK_KILOBYTE 1000
#define CTK_MEGABYTE 1000 * CTK_KILOBYTE
#define CTK_GIGABYTE 1000 * CTK_MEGABYTE

namespace ctk {

////////////////////////////////////////////////////////////
/// Declarations
////////////////////////////////////////////////////////////
static b32 equal(cstr A, cstr B);

////////////////////////////////////////////////////////////
/// Data
////////////////////////////////////////////////////////////
template<typename type>
struct array {
    type *Data;
    u32 Size;
    u32 Count;
    type &operator [](u32 Index);
};

template<typename type, u32 size>
struct sarray {
    type Data[size];
    u32 Size = size;
    u32 Count;
    type &operator [](u32 Index);
};

using map_key = char[64];

template<typename type>
struct map {
    map_key *Keys;
    type *Values;
    u32 Size;
    u32 Count;
};

template<typename type, u32 size>
struct smap {
    map_key Keys[size];
    type Values[size];
    u32 Size = size;
    u32 Count;
};

using string = array<char>;

template<typename return_type, typename ...args>
using fn = return_type (*)(args...);

template<typename key, typename value>
struct pair {
    key Key;
    value Value;
};

template<typename type>
struct optional {
    bool Set;
    type Value;
    operator b32();
    optional<type> &operator =(type Value);
};

////////////////////////////////////////////////////////////
/// Interface
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Logging
////////////////////////////////////////////////////////////
template<typename ...args>
static void print(cstr Message, args... Args) {
    printf(Message, Args...);
}

template<typename ...args>
static void print_line() {
    print("\n");
}

template<typename ...args>
static void print_line(cstr Message, args... Args) {
    print(Message, Args...);
    print_line();
}

static void print_tabs(u32 TabCount) {
    CTK_ITERATE(TabCount) {
        print("    ");
    }
}

template<typename ...args>
static void print(u32 TabCount, cstr Message, args... Args) {
    print_tabs(TabCount);
    print(Message, Args...);
}

template<typename ...args>
static void print_line(u32 TabCount, cstr Message, args... Args) {
    print_tabs(TabCount);
    print_line(Message, Args...);
}

template<typename ...args>
static void error(cstr Message, args... Args) {
    print(CTK_ERROR_TAG);
    print_line(Message, Args...);
}

template<typename ...args>
static void error(u32 TabCount, cstr Message, args... Args) {
    print(CTK_ERROR_TAG);
    print_tabs(TabCount);
    print_line(Message, Args...);
}

template<typename ...args>
static void info(cstr Message, args... Args) {
    print(CTK_ANSI_HIGHLIGHT("INFO", GREEN) ": ");
    print_line(Message, Args...);
}

template<typename ...args>
static void info(u32 TabCount, cstr Message, args... Args) {
    print(CTK_ANSI_HIGHLIGHT("INFO", GREEN) ": ");
    print_tabs(TabCount);
    print_line(Message, Args...);
}

template<typename ...args>
static void warning(cstr Message, args... Args) {
    print(CTK_ANSI_HIGHLIGHT("WARNING", MAGENTA) ": ");
    print_line(Message, Args...);
}

template<typename ...args>
static void warning(u32 TabCount, cstr Message, args... Args) {
    print(CTK_ANSI_HIGHLIGHT("WARNING", MAGENTA) ": ");
    print_tabs(TabCount);
    print_line(Message, Args...);
}

template<typename ...args>
static void todo(cstr Message, args... Args) {
    print(CTK_ANSI_HIGHLIGHT("TODO", BLUE) ": ");
    print_line(Message, Args...);
}

template<typename ...args>
static void todo(u32 TabCount, cstr Message, args... Args) {
    print(CTK_ANSI_HIGHLIGHT("TODO", BLUE) ": ");
    print_tabs(TabCount);
    print_line(Message, Args...);
}

////////////////////////////////////////////////////////////
/// Memory
////////////////////////////////////////////////////////////
template<typename type>
static type *allocate(u32 Size = 1) {
    CTK_ASSERT(Size > 0)
    return (type *)malloc(Size * sizeof(type));
}

////////////////////////////////////////////////////////////
/// Array
////////////////////////////////////////////////////////////
template<typename type>
static void allocate_array(array<type> *Array, u32 Size) {
    Array->Data = allocate<type>(Size);
    Array->Size = Size;
}

template<typename type>
static array<type> create_array(u32 Size, type InitValue) {
    array<type> Array = {};
    allocate_array(&Array, Size);
    Array.Count = Size;
    for(u32 Index = 0; Index < Array.Count; Index++) {
        Array[Index] = InitValue;
    }
    return Array;
}

template<typename type>
static array<type> create_array(u32 Size) {
    return create_array<type>(Size, {});
}

template<typename type>
static array<type> create_array(type *Data, u32 Size) {
    array<type> Array = {};
    allocate_array(&Array, Size);
    Array.Count = Size;
    memcpy(Array.Data, Data, Size * sizeof(type));
    return Array;
}

template<typename type>
static array<type> create_array(array<type> *CopyArray) {
    array<type> Array = {};
    allocate_array(&Array, CopyArray->Size);
    Array.Count = CopyArray->Count;
    memcpy(Array.Data, CopyArray->Data, byte_count(CopyArray));
    return Array;
}

template<typename type>
static array<type> create_array_empty(u32 Size) {
    array<type> Array = {};
    allocate_array(&Array, Size);
    Array.Count = 0;
    return Array;
}

template<typename type>
static void _free(array<type> *Array) {
    if(Array->Data != NULL) {
        free(Array->Data);
        *Array = {};
    }
}

template<typename type>
static type *push(array<type> *Array, type Element) {
    if(Array->Count + 1 > Array->Size) {
        CTK_FATAL("array (size=%u count=%u) cannot hold any more elements", Array->Size, Array->Count)
    }
    type *NewElement = at(Array, Array->Count++);
    *NewElement = Element;
    return NewElement;
}

template<typename type>
static type *push(array<type> *Array) {
    return push(Array, {});
}

template<typename type>
static void push(array<type> *Array, type *Elements, u32 ElementCount) {
    if(ElementCount == 0) {
        return;
    }

    if(Array->Size == 0) {
        CTK_FATAL("pushing to unallocated array (size=0)")
    }

    if(Array->Count + ElementCount > Array->Size) {
        CTK_FATAL("array (size=%u count=%u) cannot hold %u more elements", Array->Size, Array->Count, ElementCount)
    }
    memcpy(at(Array, Array->Count), Elements, sizeof(type) * ElementCount);
    Array->Count += ElementCount;
}

template<typename type>
static u32 byte_size(array<type> *Array) {
    return Array->Size * sizeof(type);
}

template<typename type>
static u32 byte_count(array<type> *Array) {
    return Array->Count * sizeof(type);
}

template<typename type>
static type *at(array<type> *Array, u32 Index) {
    CTK_ASSERT(Index < Array->Size)
    return Array->Data + Index;
}

template<typename type>
type &array<type>::operator [](u32 Index) {
    CTK_ASSERT(Index < Size)
    return Data[Index];
}

template<typename type>
static type *operator +(array<type> &Array, u32 Index) {
    CTK_ASSERT(Index < Array.Size)
    return Array.Data + Index;
}

////////////////////////////////////////////////////////////
/// Static Array
////////////////////////////////////////////////////////////
template<typename type, u32 size>
static type *push(sarray<type, size> *Array, type Element) {
    if(Array->Count + 1 > Array->Size) {
        CTK_FATAL("static array (size=%u count=%u) cannot hold any more elements", Array->Size, Array->Count)
    }
    type *NewElement = at(Array, Array->Count++);
    *NewElement = Element;
    return NewElement;
}

template<typename type, u32 size>
static type *push(sarray<type, size> *Array) {
    return push(Array, {});
}

template<typename type, u32 size>
static void push(sarray<type, size> *Array, type *Elements, u32 ElementCount) {
    if(ElementCount == 0) {
        return;
    }

    if(Array->Count + ElementCount > Array->Size) {
        CTK_FATAL("static array (size=%u count=%u) cannot hold %u more elements", Array->Size, Array->Count, ElementCount)
    }
    memcpy(at(Array, Array->Count), Elements, sizeof(type) * ElementCount);
    Array->Count += ElementCount;
}

template<typename type, u32 size>
static type pop(sarray<type, size>* Array) {
    CTK_ASSERT(Array->Count > 0);
    return Array->Data[--Array->Count];
}

template<typename type, u32 size>
static u32 byte_size(sarray<type, size> *Array) {
    return Array->Size * sizeof(type);
}

template<typename type, u32 size>
static u32 byte_count(sarray<type, size> *Array) {
    return Array->Count * sizeof(type);
}

template<typename type, u32 size>
static type *at(sarray<type, size> *Array, u32 Index) {
    CTK_ASSERT(Index < Array->Size)
    return Array->Data + Index;
}

template<typename type, u32 size>
type &sarray<type, size>::operator [](u32 Index) {
    CTK_ASSERT(Index < Size)
    return Data[Index];
}

template<typename type, u32 size>
static type *operator +(sarray<type, size> &Array, u32 Index) {
    CTK_ASSERT(Index < Array.Size)
    return Array.Data + Index;
}

////////////////////////////////////////////////////////////
/// Map
////////////////////////////////////////////////////////////
template<typename type>
static map<type> create_map(u32 Size) {
    map<type> Map = {};
    Map.Keys = allocate<map_key>(Size);
    Map.Values = allocate<type>(Size);
    Map.Size = Size;
    Map.Count = 0;
    return Map;
}

template<typename type>
static void _free(map<type> *Map) {
    if(Map->Keys != NULL) {
        free(Map->Keys);
        free(Map->Values);
        *Map = {};
    }
}

template<typename type>
static type *push(map<type> *Map, cstr Key, type Value) {
    if(Map->Size == 0) {
        CTK_FATAL("pushing to unallocated map (Size=0)")
    }

    if(Map->Count + 1 > Map->Size) {
        CTK_FATAL("map (size=%u count=%u) cannot hold any more elements", Map->Size, Map->Count)
    }

    if(strlen(Key) >= sizeof(map_key)) {
        CTK_FATAL("pushing key \"%s\" (size=%u) which is longer than max key size of %u",
                  Key, strlen(Key),
                  sizeof(map_key) - 1) // Must have room for null-terminator.
    }

    // Check if duplicate key.
    if(find(Map, Key) != NULL) {
        CTK_FATAL("attempting to push key \"%s\" to map that already has that key", Key);
    }

    strcpy(Map->Keys[Map->Count], Key);
    Map->Values[Map->Count] = Value;
    Map->Count++;
    return Map->Values + Map->Count - 1;
}

template<typename type>
static type *push(map<type> *Map, cstr Key) {
    return push(Map, Key, {});
}

template<typename type>
static type *find(map<type> *Map, cstr Key) {
    for(u32 Index = 0; Index < Map->Count; Index++) {
        if(equal(Key, (cstr)(Map->Keys + Index))) {
            return Map->Values + Index;
        }
    }
    return NULL;
}

template<typename type>
static type *find(map<type> *Map, cstr Key, u32 *OutIndex) {
    for(u32 Index = 0; Index < Map->Count; Index++) {
        if(equal(Key, (cstr)(Map->Keys + Index))) {
            *OutIndex = Index;
            return Map->Values + Index;
        }
    }
    return NULL;
}

template<typename type>
static type *at(map<type> *Map, cstr Key) {
    type *Value = find(Map, Key);
    if(Value == NULL) {
        CTK_FATAL("failed to find entry for key \"%s\" in map", Key);
    }
    return Value;
}

template<typename type>
static u32 values_byte_size(map<type> *Map) {
    return Map->Size * sizeof(type);
}

template<typename type>
static u32 values_byte_count(map<type> *Map) {
    return Map->Count * sizeof(type);
}

////////////////////////////////////////////////////////////
/// Static Map
////////////////////////////////////////////////////////////
template<typename type, u32 size>
static type *push(smap<type, size> *Map, cstr Key, type Value) {
    if(Map->Count + 1 > Map->Size) {
        CTK_FATAL("static map (size=%u count=%u) cannot hold any more elements", Map->Size, Map->Count)
    }

    if(strlen(Key) >= sizeof(map_key)) {
        CTK_FATAL("pushing key \"%s\" (size=%u) which is longer than max key size of %u", Key, strlen(Key), sizeof(map_key) - 1)
    }

    // Check if duplicate key.
    if(find(Map, Key) != NULL) {
        CTK_FATAL("attempting to push key \"%s\" to static map that already has that key", Key);
    }

    strcpy(Map->Keys[Map->Count], Key);
    Map->Values[Map->Count] = Value;
    Map->Count++;
    return Map->Values + Map->Count - 1;
}

template<typename type, u32 size>
static type *push(smap<type, size> *Map, cstr Key) {
    return push(Map, Key, {});
}

template<typename type, u32 size>
static type *find(smap<type, size> *Map, cstr Key) {
    for(u32 Index = 0; Index < Map->Count; Index++) {
        if(equal(Key, (cstr)(Map->Keys + Index))) {
            return Map->Values + Index;
        }
    }
    return NULL;
}

template<typename type, u32 size>
static type *find(smap<type, size> *Map, cstr Key, u32 *OutIndex) {
    for(u32 Index = 0; Index < Map->Count; Index++) {
        if(equal(Key, (cstr)(Map->Keys + Index))) {
            *OutIndex = Index;
            return Map->Values + Index;
        }
    }
    return NULL;
}

template<typename type, u32 size>
static type *at(smap<type, size> *Map, cstr Key) {
    type *Value = find(Map, Key);
    if(Value == NULL) {
        CTK_FATAL("failed to find entry for key \"%s\" in static map", Key);
    }
    return Value;
}

template<typename type, u32 size>
static u32 values_byte_size(smap<type, size> *Map) {
    return Map->Size * sizeof(type);
}

template<typename type, u32 size>
static u32 values_byte_count(smap<type, size> *Map) {
    return Map->Count * sizeof(type);
}

////////////////////////////////////////////////////////////
/// String
////////////////////////////////////////////////////////////
static string create_string(cstr String, u32 StringSize) {
    string NewString = create_array<char>(StringSize + 1); // Add room for null-terminator.
    NewString.Count--; // Exclude null-terminator from count.
    memcpy(NewString.Data, String, StringSize);
    NewString[NewString.Size - 1] = '\0'; // Add null-terminator to last index.
    return NewString;
}

static string create_string(cstr String) {
    return create_string(String, strlen(String));
}

static string create_string_empty(u32 StringSize) {
    string NewString = create_array_empty<char>(StringSize + 1);
    NewString[0] = '\0';
    return NewString;
}

static void push(string *String, cstr Other, u32 OtherSize) {
    if(String->Size == 0) {
        CTK_FATAL("pushing to unallocated string (Size=0)")
    }

    if(String->Count + OtherSize > String->Size) {
        CTK_FATAL("string (size=%u count=%u) cannot hold %u more characters", String->Size, String->Count, OtherSize)
    }
    memcpy(at(String, String->Count), Other, sizeof(char) * OtherSize);
    String->Count += OtherSize;
    *at(String, String->Count) = '\0';
}

static void push(string *String, char *Other, u32 OtherSize) {
    push(String, (cstr)Other, OtherSize);
}

static void push(string *String, cstr Other) {
    push(String, Other, strlen(Other)); // Don't push null-terminator (strlen does not include it).
}

static void push(string *String, string *Other) {
    push(String, Other->Data, Other->Count);
}

static b32 equal(cstr A, cstr B) {
    return strcmp(A, B) == 0;
}

static b32 equal(string *A, cstr B) {
    return strcmp(A->Data, B) == 0;
}

static b32 equal(string *A, string *B) {
    return strcmp(A->Data, B->Data) == 0;
}

static f32 to_f32(string *String) {
    return strtof(String->Data, NULL);
}

static u32 to_u32(string *String) {
    return strtoul(String->Data, NULL, 10);
}

static u32 to_u32(cstr String) {
    return strtoul(String, NULL, 10);
}

static s32 to_s32(string *String) {
    return strtol(String->Data, NULL, 10);
}

static b32 to_b32(cstr String) {
    if(equal(String, "true")) {
        return true;
    }

    if(equal(String, "false")) {
        return false;
    }
    CTK_FATAL("string \"%s\" cannot be converted to a boolean value", String)
}

static b32 to_b32(string *String) {
    return to_b32(String->Data);
}

////////////////////////////////////////////////////////////
/// Pair
////////////////////////////////////////////////////////////
template<typename key, typename value>
static pair<key, value> *find_pair(pair<key, value> *Pairs, u32 PairCount, key Key) {
    for(u32 PairIndex = 0; PairIndex < PairCount; PairIndex++) {
        auto Pair = Pairs + PairIndex;
        if(Pair->Key == Key) {
            return Pair;
        }
    }
    return NULL;
}

template<typename key, typename value>
static pair<key, value> *find_pair(pair<key, value> *Pairs, u32 PairCount, key Key, fn<b32, key, key> Comparator) {
    for(u32 PairIndex = 0; PairIndex < PairCount; PairIndex++) {
        auto Pair = Pairs + PairIndex;
        if(Comparator(Pair->Key, Key)) {
            return Pair;
        }
    }
    return NULL;
}

////////////////////////////////////////////////////////////
/// Optional
////////////////////////////////////////////////////////////
template<typename type>
static type *set(optional<type> *Optional, type Value) {
    Optional->Value = Value;
    Optional->Set = true;
    return &Optional->Value;
}

template<typename type>
optional<type>::operator b32() {
    return Set;
}

template<typename type>
optional<type> &optional<type>::operator =(type Value) {
    this->Value = Value;
    Set = true;
    return *this;
}

////////////////////////////////////////////////////////////
/// Misc.
////////////////////////////////////////////////////////////
template<typename type>
static array<type> read_file(cstr Path) {
    CTK_ASSERT(Path != NULL)
    array<type> Elements = {};
    FILE *File = fopen(Path, "rb");
    if(File == NULL) {
        CTK_FATAL("failed to open \"%s\"", Path)
    }
    fseek(File, 0, SEEK_END);
    u32 FileSize = ftell(File);
    if(FileSize > 0) {
        rewind(File);
        Elements = create_array<type>(FileSize);
        fread(Elements.Data, FileSize, 1, File);
    }
    fclose(File);
    return Elements;
}

static string read_text_file(cstr Path) {
    CTK_ASSERT(Path != NULL)
    array<char> Chars = read_file<char>(Path);
    string Text = create_string(Chars.Data, Chars.Count);
    _free(&Chars);
    return Text;
}

} // ctk
