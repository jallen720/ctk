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
    #define CTK_FATAL(MESSAGE, ...) \
        ctk::PrintLine(CTK_ERROR_TAG CTK_ANSI_HIGHLIGHT("file", BLUE) ": %s", __FILE__); \
        ctk::PrintLine("       " CTK_ANSI_HIGHLIGHT("line", BLUE) ": %i", __LINE__); \
        ctk::PrintLine("       " CTK_ANSI_HIGHLIGHT("func", BLUE) ": %s()", __FUNCTION__); \
        ctk::Print("       " CTK_ANSI_HIGHLIGHT("msg", BLUE) ": "); \
        ctk::PrintLine(MESSAGE, ## __VA_ARGS__); \
        throw 0;
#else
    #define CTK_FATAL(MESSAGE, ...) \
        ctk::PrintLine(CTK_ERROR_TAG CTK_ANSI_HIGHLIGHT("file", BLUE) ": %s", __FILE__); \
        ctk::PrintLine("       " CTK_ANSI_HIGHLIGHT("line", BLUE) ": %i", __LINE__); \
        ctk::PrintLine("       " CTK_ANSI_HIGHLIGHT("func", BLUE) ": %s()", __FUNCTION__); \
        ctk::Print("       " CTK_ANSI_HIGHLIGHT("msg", BLUE) ": "); \
        ctk::PrintLine(MESSAGE, __VA_ARGS__); \
        throw 0;
#endif

#define CTK_ASSERT(STATEMENT) if(!(STATEMENT)) { CTK_FATAL("assertion \"%s\" failed", #STATEMENT) }
#define CTK_VALUE_NAME_PAIR(VALUE) { #VALUE, VALUE }
#define CTK_REPEAT(COUNT) for(u32 _ = 0; _ < COUNT; _++)

namespace ctk {

////////////////////////////////////////////////////////////
/// Declarations
////////////////////////////////////////////////////////////
static b32
StringEqual(cstr A, cstr B);

////////////////////////////////////////////////////////////
/// Data
////////////////////////////////////////////////////////////
template<typename type>
struct array
{
    type *Data;
    u32 Size;
    u32 Count;
    type &operator [](u32 Index);
};

template<typename type, u32 size>
struct sarray
{
    type Data[size];
    u32 Size = size;
    u32 Count;
    type &operator [](u32 Index);
};

using map_key = char[64];

template<typename type>
struct map
{
    map_key *Keys;
    type *Values;
    u32 Size;
    u32 Count;
};

template<typename type, u32 size>
struct smap
{
    map_key Keys[size];
    type Values[size];
    u32 Size = size;
    u32 Count;
};

using string = array<char>;

template<typename return_type, typename ...args>
using fn = return_type (*)(args...);

template<typename return_type, typename ...args>
struct functor
{
    fn<return_type, void *, args...> Fn;
    void *Data;
};

template<typename key, typename value>
struct pair
{
    key Key;
    value Value;
};

template<typename type>
struct optional
{
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
static void
Print(cstr Message, args... Args)
{
    printf(Message, Args...);
}

template<typename ...args>
static void
PrintLine()
{
    Print("\n");
}

template<typename ...args>
static void
PrintLine(cstr Message, args... Args)
{
    Print(Message, Args...);
    PrintLine();
}

static void
PrintTabs(u32 TabCount)
{
    CTK_REPEAT(TabCount)
    {
        Print("    ");
    }
}

template<typename ...args>
static void
Print(u32 TabCount, cstr Message, args... Args)
{
    PrintTabs(TabCount);
    Print(Message, Args...);
}

template<typename ...args>
static void
PrintLine(u32 TabCount, cstr Message, args... Args)
{
    PrintTabs(TabCount);
    PrintLine(Message, Args...);
}

template<typename ...args>
static void
Error(cstr Message, args... Args)
{
    Print(CTK_ERROR_TAG);
    PrintLine(Message, Args...);
}

template<typename ...args>
static void
Error(u32 TabCount, cstr Message, args... Args)
{
    Print(CTK_ERROR_TAG);
    PrintTabs(TabCount);
    PrintLine(Message, Args...);
}

template<typename ...args>
static void
Info(cstr Message, args... Args)
{
    Print(CTK_ANSI_HIGHLIGHT("INFO", GREEN) ": ");
    PrintLine(Message, Args...);
}

template<typename ...args>
static void
Info(u32 TabCount, cstr Message, args... Args)
{
    Print(CTK_ANSI_HIGHLIGHT("INFO", GREEN) ": ");
    PrintTabs(TabCount);
    PrintLine(Message, Args...);
}

template<typename ...args>
static void
Warning(cstr Message, args... Args)
{
    Print(CTK_ANSI_HIGHLIGHT("WARNING", MAGENTA) ": ");
    PrintLine(Message, Args...);
}

template<typename ...args>
static void
Warning(u32 TabCount, cstr Message, args... Args)
{
    Print(CTK_ANSI_HIGHLIGHT("WARNING", MAGENTA) ": ");
    PrintTabs(TabCount);
    PrintLine(Message, Args...);
}

template<typename ...args>
static void
Todo(cstr Message, args... Args)
{
    Print(CTK_ANSI_HIGHLIGHT("TODO", BLUE) ": ");
    PrintLine(Message, Args...);
}

template<typename ...args>
static void
Todo(u32 TabCount, cstr Message, args... Args)
{
    Print(CTK_ANSI_HIGHLIGHT("TODO", BLUE) ": ");
    PrintTabs(TabCount);
    PrintLine(Message, Args...);
}

////////////////////////////////////////////////////////////
/// Memory
////////////////////////////////////////////////////////////
template<typename type>
static type *
Alloc(u32 Size = 1)
{
    CTK_ASSERT(Size > 0)
    return (type *)malloc(Size * sizeof(type));
}

////////////////////////////////////////////////////////////
/// Array
////////////////////////////////////////////////////////////
template<typename type>
static void
AllocArray(array<type> *Array, u32 Size)
{
    Array->Data = Alloc<type>(Size);
    Array->Size = Size;
}

template<typename type>
static array<type>
CreateArray(u32 Size, type InitValue)
{
    array<type> Array = {};
    AllocArray(&Array, Size);
    Array.Count = Size;
    for(u32 Index = 0; Index < Array.Count; Index++)
    {
        Array[Index] = InitValue;
    }
    return Array;
}

template<typename type>
static array<type>
CreateArray(u32 Size)
{
    return CreateArray<type>(Size, {});
}

template<typename type>
static array<type>
CreateArray(type *Data, u32 Size)
{
    array<type> Array = {};
    AllocArray(&Array, Size);
    Array.Count = Size;
    memcpy(Array.Data, Data, Size * sizeof(type));
    return Array;
}

template<typename type>
static array<type>
CreateArray(array<type> *CopyArray)
{
    array<type> Array = {};
    AllocArray(&Array, CopyArray->Size);
    Array.Count = CopyArray->Count;
    memcpy(Array.Data, CopyArray->Data, ByteCount(CopyArray));
    return Array;
}

template<typename type>
static array<type>
CreateArrayEmpty(u32 Size)
{
    array<type> Array = {};
    AllocArray(&Array, Size);
    Array.Count = 0;
    return Array;
}

template<typename type>
static void
Free(array<type> *Array)
{
    if(Array->Data != NULL)
    {
        free(Array->Data);
        *Array = {};
    }
}

template<typename type>
static type *
Push(array<type> *Array, type Element)
{
    if(Array->Count + 1 > Array->Size)
    {
        CTK_FATAL("array (size=%u count=%u) cannot hold any more elements", Array->Size, Array->Count)
    }
    type *NewElement = At(Array, Array->Count++);
    *NewElement = Element;
    return NewElement;
}

template<typename type>
static type *
Push(array<type> *Array)
{
    return Push(Array, {});
}

template<typename type>
static void
Push(array<type> *Array, type *Elements, u32 ElementCount)
{
    if(ElementCount == 0)
    {
        return;
    }

    if(Array->Size == 0)
    {
        CTK_FATAL("pushing to unallocated array (size=0)")
    }

    if(Array->Count + ElementCount > Array->Size)
    {
        CTK_FATAL("array (size=%u count=%u) cannot hold %u more elements", Array->Size, Array->Count, ElementCount)
    }
    memcpy(At(Array, Array->Count), Elements, sizeof(type) * ElementCount);
    Array->Count += ElementCount;
}

template<typename type>
static u32
ByteSize(array<type> *Array)
{
    return Array->Size * sizeof(type);
}

template<typename type>
static u32
ByteCount(array<type> *Array)
{
    return Array->Count * sizeof(type);
}

template<typename type>
static type *
At(array<type> *Array, u32 Index)
{
    CTK_ASSERT(Index < Array->Size)
    return Array->Data + Index;
}

template<typename type>
type &
array<type>::operator [](u32 Index)
{
    CTK_ASSERT(Index < Size)
    return Data[Index];
}

template<typename type>
static type *
operator +(array<type> &Array, u32 Index)
{
    CTK_ASSERT(Index < Array.Size)
    return Array.Data + Index;
}

////////////////////////////////////////////////////////////
/// Static Array
////////////////////////////////////////////////////////////
template<typename type, u32 size>
static type *
Push(sarray<type, size> *Array, type Element)
{
    if(Array->Count + 1 > Array->Size)
    {
        CTK_FATAL("static array (size=%u count=%u) cannot hold any more elements", Array->Size, Array->Count)
    }
    type *NewElement = At(Array, Array->Count++);
    *NewElement = Element;
    return NewElement;
}

template<typename type, u32 size>
static type *
Push(sarray<type, size> *Array)
{
    return Push(Array, {});
}

template<typename type, u32 size>
static void
Push(sarray<type, size> *Array, type *Elements, u32 ElementCount)
{
    if(ElementCount == 0)
    {
        return;
    }

    if(Array->Count + ElementCount > Array->Size)
    {
        CTK_FATAL("static array (size=%u count=%u) cannot hold %u more elements", Array->Size, Array->Count, ElementCount)
    }
    memcpy(At(Array, Array->Count), Elements, sizeof(type) * ElementCount);
    Array->Count += ElementCount;
}

template<typename type, u32 size>
static u32
ByteSize(sarray<type, size> *Array)
{
    return Array->Size * sizeof(type);
}

template<typename type, u32 size>
static u32
ByteCount(sarray<type, size> *Array)
{
    return Array->Count * sizeof(type);
}

template<typename type, u32 size>
static type *
At(sarray<type, size> *Array, u32 Index)
{
    CTK_ASSERT(Index < Array->Size)
    return Array->Data + Index;
}

template<typename type, u32 size>
type &
sarray<type, size>::operator [](u32 Index)
{
    CTK_ASSERT(Index < Size)
    return Data[Index];
}

template<typename type, u32 size>
static type *
operator +(sarray<type, size> &Array, u32 Index)
{
    CTK_ASSERT(Index < Array.Size)
    return Array.Data + Index;
}

////////////////////////////////////////////////////////////
/// Map
////////////////////////////////////////////////////////////
template<typename type>
static map<type>
CreateMap(u32 Size)
{
    map<type> Map = {};
    Map.Keys = Alloc<map_key>(Size);
    Map.Values = Alloc<type>(Size);
    Map.Size = Size;
    Map.Count = 0;
    return Map;
}

template<typename type>
static void
Free(map<type> *Map)
{
    if(Map->Keys != NULL)
    {
        free(Map->Keys);
        free(Map->Values);
        *Map = {};
    }
}

template<typename type>
static type *
Push(map<type> *Map, cstr Key, type Value)
{
    if(Map->Size == 0)
    {
        CTK_FATAL("pushing to unallocated map (Size=0)")
    }

    if(Map->Count + 1 > Map->Size)
    {
        CTK_FATAL("map (size=%u count=%u) cannot hold any more elements", Map->Size, Map->Count)
    }

    if(strlen(Key) >= sizeof(map_key))
    {
        CTK_FATAL("pushing key \"%s\" (size=%u) which is longer than max key size of %u",
                  Key, strlen(Key),
                  sizeof(map_key) - 1) // Must have room for null-terminator.
    }

    // Check if duplicate key.
    if(Find(Map, Key) != NULL)
    {
        CTK_FATAL("attempting to push key \"%s\" to map that already has that key", Key);
    }

    strcpy(Map->Keys[Map->Count], Key);
    Map->Values[Map->Count] = Value;
    Map->Count++;
    return Map->Values + Map->Count - 1;
}

template<typename type>
static type *
Push(map<type> *Map, cstr Key)
{
    return Push(Map, Key, {});
}

template<typename type>
static type *
Find(map<type> *Map, cstr Key)
{
    for(u32 Index = 0; Index < Map->Count; Index++)
    {
        if(StringEqual(Key, (cstr)(Map->Keys + Index)))
        {
            return Map->Values + Index;
        }
    }
    return NULL;
}

template<typename type>
static type *
At(map<type> *Map, cstr Key)
{
    type *Value = Find(Map, Key);
    if(Value == NULL)
    {
        CTK_FATAL("failed to find entry for key \"%s\" in map", Key);
    }
    return Value;
}

////////////////////////////////////////////////////////////
/// Static Map
////////////////////////////////////////////////////////////
template<typename type, u32 size>
static type *
Push(smap<type, size> *Map, cstr Key, type Value)
{
    if(Map->Count + 1 > Map->Size)
    {
        CTK_FATAL("static map (size=%u count=%u) cannot hold any more elements", Map->Size, Map->Count)
    }

    if(strlen(Key) >= sizeof(map_key))
    {
        CTK_FATAL("pushing key \"%s\" (size=%u) which is longer than max key size of %u",
                  Key, strlen(Key),
                  sizeof(map_key) - 1) // Must have room for null-terminator.
    }

    // Check if duplicate key.
    if(Find(Map, Key) != NULL)
    {
        CTK_FATAL("attempting to push key \"%s\" to static map that already has that key", Key);
    }

    strcpy(Map->Keys[Map->Count], Key);
    Map->Values[Map->Count] = Value;
    Map->Count++;
    return Map->Values + Map->Count - 1;
}

template<typename type, u32 size>
static type *
Push(smap<type, size> *Map, cstr Key)
{
    return Push(Map, Key, {});
}

template<typename type, u32 size>
static type *
Find(smap<type, size> *Map, cstr Key)
{
    for(u32 Index = 0; Index < Map->Count; Index++)
    {
        if(StringEqual(Key, (cstr)(Map->Keys + Index)))
        {
            return Map->Values + Index;
        }
    }
    return NULL;
}

template<typename type, u32 size>
static type *
At(smap<type, size> *Map, cstr Key)
{
    type *Value = Find(Map, Key);
    if(Value == NULL)
    {
        CTK_FATAL("failed to find entry for key \"%s\" in static map", Key);
    }
    return Value;
}

////////////////////////////////////////////////////////////
/// Functor
////////////////////////////////////////////////////////////
template<typename return_type, typename ...args>
static return_type
Call(functor<return_type, args...> *Functor, args... Args)
{
    return Functor->Fn(Functor->Data, Args...);
}

////////////////////////////////////////////////////////////
/// String
////////////////////////////////////////////////////////////
static string
CreateString(cstr String, u32 StringSize)
{
    string NewString = CreateArray<char>(StringSize + 1); // Add room for null-terminator.
    NewString.Count--; // Exclude null-terminator from count.
    memcpy(NewString.Data, String, StringSize);
    NewString[NewString.Size - 1] = '\0'; // Add null-terminator to last index.
    return NewString;
}

static string
CreateString(cstr String)
{
    return CreateString(String, strlen(String));
}

static string
CreateStringEmpty(u32 StringSize)
{
    string NewString = CreateArrayEmpty<char>(StringSize + 1);
    NewString[0] = '\0';
    return NewString;
}

static void
Push(string *String, cstr Other, u32 OtherSize)
{
    if(String->Size == 0)
    {
        CTK_FATAL("pushing to unallocated string (Size=0)")
    }

    if(String->Count + OtherSize > String->Size)
    {
        CTK_FATAL("string (size=%u count=%u) cannot hold %u more characters", String->Size, String->Count, OtherSize)
    }
    memcpy(At(String, String->Count), Other, sizeof(char) * OtherSize);
    String->Count += OtherSize;
    *At(String, String->Count) = '\0';
}

static void
Push(string *String, char *Other, u32 OtherSize)
{
    Push(String, (cstr)Other, OtherSize);
}

static void
Push(string *String, cstr Other)
{
    Push(String, Other, strlen(Other)); // Don't push null-terminator (strlen does not include it).
}

static void
Push(string *String, string *Other)
{
    Push(String, Other->Data, Other->Count);
}

static b32
StringEqual(cstr A, cstr B)
{
    return strcmp(A, B) == 0;
}

static b32
StringEqual(string *A, cstr B)
{
    return strcmp(A->Data, B) == 0;
}

static b32
StringEqual(string *A, string *B)
{
    return strcmp(A->Data, B->Data) == 0;
}

static f32
StringToF32(string *String)
{
    return strtof(String->Data, NULL);
}

static u32
StringToU32(string *String)
{
    return strtoul(String->Data, NULL, 10);
}

static u32
StringToU32(cstr String)
{
    return strtoul(String, NULL, 10);
}

static s32
StringToS32(string *String)
{
    return strtol(String->Data, NULL, 10);
}

static b32
StringToB32(cstr String)
{
    if(StringEqual(String, "true"))
    {
        return true;
    }

    if(StringEqual(String, "false"))
    {
        return false;
    }
    CTK_FATAL("string \"%s\" cannot be converted to a boolean value", String)
}

static b32
StringToB32(string *String)
{
    return StringToB32(String->Data);
}

////////////////////////////////////////////////////////////
/// Pair
////////////////////////////////////////////////////////////
template<typename key, typename value>
static pair<key, value> *
FindPair(pair<key, value> *Pairs, u32 PairCount, key Key)
{
    for(u32 PairIndex = 0; PairIndex < PairCount; PairIndex++)
    {
        auto Pair = Pairs + PairIndex;
        if(Pair->Key == Key)
        {
            return Pair;
        }
    }
    return NULL;
}

template<typename key, typename value>
static pair<key, value> *
FindPair(pair<key, value> *Pairs, u32 PairCount, key Key, fn<b32, key, key> Comparator)
{
    for(u32 PairIndex = 0; PairIndex < PairCount; PairIndex++)
    {
        auto Pair = Pairs + PairIndex;
        if(Comparator(Pair->Key, Key))
        {
            return Pair;
        }
    }
    return NULL;
}

////////////////////////////////////////////////////////////
/// Optional
////////////////////////////////////////////////////////////
template<typename type>
optional<type>::operator b32()
{
    return Set;
}

template<typename type>
optional<type> &
optional<type>::operator =(type Value)
{
    this->Value = Value;
    Set = true;
    return *this;
}

////////////////////////////////////////////////////////////
/// Misc.
////////////////////////////////////////////////////////////
template<typename type>
static array<type>
ReadFile(cstr Path)
{
    CTK_ASSERT(Path != NULL)
    array<type> Elements = {};
    FILE *File = fopen(Path, "rb");
    if(File == NULL)
    {
        CTK_FATAL("failed to open \"%s\"", Path)
    }
    fseek(File, 0, SEEK_END);
    u32 FileSize = ftell(File);
    if(FileSize > 0)
    {
        rewind(File);
        Elements = CreateArray<type>(FileSize);
        fread(Elements.Data, FileSize, 1, File);
    }
    fclose(File);
    return Elements;
}

static string
ReadTextFile(cstr Path)
{
    CTK_ASSERT(Path != NULL)
    array<char> Chars = ReadFile<char>(Path);
    string Text = CreateString(Chars.Data, Chars.Count);
    Free(&Chars);
    return Text;
}

} // ctk
