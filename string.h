/// Macros
////////////////////////////////////////////////////////////
#define CTK_WRAP_STRING(STRING) WrapString((char*)STRING, CTK_ARRAY_SIZE(STRING))
#define CTK_WRAP_STRING_1(PTR)  WrapString(PTR, 1)

/// Data
////////////////////////////////////////////////////////////
using String = Array<char>;

/// Interface
////////////////////////////////////////////////////////////
String CreateString(Allocator* allocator, uint32 size = 0) {
    return CreateArray<char>(allocator, size);
}

String CreateString(Allocator* allocator, const char* nt_src_string) {
    return CreateArray(allocator, nt_src_string, StringSize(nt_src_string));
}

String CreateString(Allocator* allocator, const char* src_string, uint32 size) {
   return CreateArray(allocator, src_string, size);
}

String CreateString(Allocator* allocator, String* src_string) {
    return CreateArray(allocator, src_string);
}

String CreateStringFull(Allocator* allocator, uint32 size) {
    return CreateArrayFull<char>(allocator, size);
}

void DestroyString(String* string) {
    DestroyArray(string);
}

String WrapString(char* string, uint32 size) {
    return WrapArray(string, size);
}

void PushRange(String* string, const char* nt_src_string) {
    PushRange(string, nt_src_string, StringSize(nt_src_string));
}

void PushRangeResize(String* string, const char* nt_src_string, uint32 additional_space) {
    PushRangeResize(string, nt_src_string, StringSize(nt_src_string), additional_space);
}

template<typename ...Args>
void Write(String* string, const char* fmt_string, Args... args) {
    uint32 chars_to_write = (uint32)snprintf(string->data, string->size, fmt_string, args...);
    string->count = chars_to_write <= string->size ? chars_to_write : string->size;
}

template<typename ...Args>
void Append(String* string, const char* fmt_string, Args... args) {
    uint32 remaining_space = string->size - string->count;
    uint32 chars_to_append = (uint32)snprintf(&string->data[string->count], remaining_space, fmt_string, args...);
    string->count += chars_to_append <= remaining_space ? chars_to_append : remaining_space;
}

bool StringsMatch(String* string_a, const char* string_b, uint32 string_b_size) {
    return StringsMatch(string_a->data, string_a->count, string_b, string_b_size);
}

bool StringsMatch(String* string_a, const char* nt_string_b) {
    return StringsMatch(string_a->data, string_a->count, nt_string_b, StringSize(nt_string_b));
}

bool StringsMatch(String* string_a, String* string_b) {
    return StringsMatch(string_a->data, string_a->count, string_b->data, string_b->count);
}

bool IsSubstring(String* string, const char* substring, uint32 substring_size) {
    return IsSubstring(string->data, string->count, substring, substring_size);
}

bool IsSubstring(String* string, const char* nt_substring) {
    return IsSubstring(string->data, string->count, nt_substring, StringSize(nt_substring));
}

bool IsSubstring(String* string, String* substring) {
    return IsSubstring(string->data, string->count, substring->data, substring->count);
}

template<typename FloatType>
FloatType ToFloat(String* string) {
    return ToFloat<FloatType>(string->data, string->count);
}

#define STRING_TO_FLOAT_FUNC(BITS) \
float##BITS ToFloat##BITS(String* string) { \
    return ToFloat##BITS(string->data, string->count); \
}
STRING_TO_FLOAT_FUNC(32)
STRING_TO_FLOAT_FUNC(64)

template<typename IntType>
IntType ToInt(String* string) {
    return ToInt<IntType>(string->data, string->count);
}

#define STRING_TO_INT_FUNC(SIGN, SIGN_UPPER, BITS) \
SIGN##int##BITS To##SIGN_UPPER##Int##BITS(String* string) { \
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

bool ToBool(String* string) {
    if (StringsMatch(string, "true"))  { return true;  }
    if (StringsMatch(string, "false")) { return false; }

    CTK_FATAL("string '%.*s' can't be converted to a boolean value: must be 'true' or 'false'", string->data, string->count);
}

