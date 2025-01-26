/// Data
////////////////////////////////////////////////////////////
template<uint32 size>
using FString = FArray<char, size>;

/// Interface
////////////////////////////////////////////////////////////
template<uint32 size>
void PushRange(FString<size>* string, const char* nt_string) {
    PushRange(string, nt_string, StringSize(nt_string));
}

template<uint32 size, typename ...Args>
void Write(FString<size>* string, const char* fmt_string, Args... args) {
    uint32 chars_to_write = (uint32)snprintf(string->data, size, fmt_string, args...);
    string->count = chars_to_write <= size ? chars_to_write : size;
}

template<uint32 size, typename ...Args>
void Append(FString<size>* string, const char* fmt_string, Args... args) {
    uint32 remaining_space = size - string->count;
    uint32 chars_to_append = (uint32)snprintf(&string->data[string->count], remaining_space, fmt_string, args...);
    string->count += chars_to_append <= remaining_space ? chars_to_append : remaining_space;
}

template<uint32 size>
bool StringsMatch(FString<size>* string, const char* other_string, uint32 other_string_size) {
    return StringsMatch(string->data, string->count, other_string, other_string_size);
}

template<uint32 size>
bool StringsMatch(FString<size>* str_a, const char* nt_string) {
    return StringsMatch(str_a->data, str_a->count, nt_string, StringSize(nt_string));
}

template<uint32 size_a, uint32 size_b>
bool StringsMatch(FString<size_a>* str_a, FString<size_b>* str_b) {
    return StringsMatch(str_a->data, str_a->count, str_b->data, str_b->count);
}

template<uint32 size>
bool StringsMatch(FString<size>* str_a, const char* str_b, uint32 match_size, uint32 match_start = 0) {
    return StringsMatch(str_a->data, str_b, match_size, match_start);
}

template<uint32 size_a, uint32 size_b>
bool StringsMatch(FString<size_a>* str_a, FString<size_b>* str_b, uint32 match_size, uint32 match_start = 0) {
    return StringsMatch(str_a->data, str_b->data, match_size, match_start);
}


template<typename FloatType, uint32 size>
FloatType ToFloat(FString<size>* s) {
    return ToFloat<FloatType>(s->data, s->count);
}

#define FSTRING_TO_FLOAT_FUNC(BITS) \
template<uint32 size> \
float##BITS ToFloat##BITS(FString<size>* s) { \
    return ToFloat##BITS(s->data, s->count); \
}
FSTRING_TO_FLOAT_FUNC(32)
FSTRING_TO_FLOAT_FUNC(64)

template<typename IntType, uint32 size>
IntType ToInt(FString<size>* s) {
    return ToInt<IntType>(s->data, s->count);
}

#define FSTRING_TO_INT_FUNC(SIGN, SIGN_UPPER, BITS) \
template<uint32 size> \
SIGN##int##BITS To##SIGN_UPPER##Int##BITS(FString<size>* s) { \
    return To##SIGN_UPPER##Int##BITS(s->data, s->count); \
}
FSTRING_TO_INT_FUNC(s, S, 8)
FSTRING_TO_INT_FUNC(s, S, 16)
FSTRING_TO_INT_FUNC(s, S, 32)
FSTRING_TO_INT_FUNC(s, S, 64)
FSTRING_TO_INT_FUNC(u, U, 8)
FSTRING_TO_INT_FUNC(u, U, 16)
FSTRING_TO_INT_FUNC(u, U, 32)
FSTRING_TO_INT_FUNC(u, U, 64)

template<uint32 size>
bool ToBool(FString<size>* s) {
    if (StringsMatch(s, "true"))  { return true;  }
    if (StringsMatch(s, "false")) { return false; }

    CTK_FATAL("string '%.*s' can't be converted to a boolean value: must be 'true' or 'false'", s->data, s->count);
}

