/// Macros
////////////////////////////////////////////////////////////
#define CTK_ARRAY_SIZE(ARRAY) sizeof(ARRAY) / sizeof(ARRAY[0])
#define CTK_VALUE_STRING_PAIR(VALUE) { VALUE, #VALUE }
#define CTK_REPEAT(COUNT) for (uint32 _ = 0; _ < COUNT; _ += 1)
#define CTK_UNUSED(VAR) (void)VAR;

#define CTK_ITER(VAR, CONTAINER) \
    for (auto* VAR = IterStart(CONTAINER); \
         VAR < IterEnd  (CONTAINER); \
         VAR += 1)

#define CTK_ITER_PTR(VAR, PTR, SIZE) \
    for (auto* VAR = PTR; \
         VAR < PTR + SIZE; \
         VAR += 1)

#define CTK_ITER_ARRAY(VAR, ARRAY) CTK_ITER_PTR(VAR, ARRAY, CTK_ARRAY_SIZE(ARRAY))

#define CTK_ITER_REV(VAR, CONTAINER) \
    for (auto* VAR = IterEnd(CONTAINER) - 1; \
         VAR > IterStart(CONTAINER) - 1; \
         VAR -= 1)

#define CTK_ITER_PTR_REV(VAR, PTR, SIZE) \
    for (auto* VAR = PTR + SIZE - 1; \
         VAR > PTR - 1; \
         VAR -= 1)

#define CTK_ITER_ARRAY_REV(VAR, ARRAY) CTK_ITER_PTR_REV(VAR, ARRAY, CTK_ARRAY_SIZE(ARRAY))

#define CTK_ITER_ENUM(VAR, ENUM_TYPE) \
    for (ENUM_TYPE VAR = (ENUM_TYPE)0; \
         (sint32)VAR < (sint32)ENUM_TYPE::COUNT; \
         VAR = (ENUM_TYPE)((sint32)VAR + 1))

#define CTK_ITER_ENUM_REV(VAR, ENUM_TYPE) \
    for (ENUM_TYPE VAR = (ENUM_TYPE)((sint32)ENUM_TYPE::COUNT - 1); \
         (sint32)VAR > -1; \
         VAR = (ENUM_TYPE)((sint32)VAR - 1))

#define CTK_ITER_IDX(VAR, CONTAINER) (uint32)(VAR - IterStart(CONTAINER))
#define CTK_ITER_PTR_IDX(VAR, PTR) (uint32)(VAR - PTR)

/// Types
////////////////////////////////////////////////////////////
using uint8   = uint8_t;
using uint16  = uint16_t;
using uint32  = uint32_t;
using uint64  = uint64_t;
using sint8   = int8_t;
using sint16  = int16_t;
using sint32  = int32_t;
using sint64  = int64_t;
using float32 = float;
using float64 = double;

/// Constants
////////////////////////////////////////////////////////////
constexpr sint8  SINT8_MIN  = SCHAR_MIN;
constexpr sint8  SINT8_MAX  = SCHAR_MAX;
constexpr sint16 SINT16_MIN = SHRT_MIN;
constexpr sint16 SINT16_MAX = SHRT_MAX;
constexpr sint32 SINT32_MIN = INT_MIN;
constexpr sint32 SINT32_MAX = INT_MAX;
constexpr sint64 SINT64_MIN = LLONG_MIN;
constexpr sint64 SINT64_MAX = LLONG_MAX;

// Defined in stdint.h
// constexpr uint8  UINT8_MAX  = UCHAR_MAX;
// constexpr uint16 UINT16_MAX = USHRT_MAX;
// constexpr uint32 UINT32_MAX = UINT_MAX;
// constexpr uint64 UINT64_MAX = ULLONG_MAX;

template<uint32 num>
constexpr uint32 Kilobyte32() {
    static_assert(num <= 4000000);
    return num * 1000;
}

template<uint32 num>
constexpr uint32 Megabyte32() {
    static_assert(num <= 4000);
    return num * Kilobyte32<1000>();
}

template<uint32 num>
constexpr uint32 Gigabyte32() {
    static_assert(num <= 4);
    return num * Megabyte32<1000>();
}

template<typename Type>
constexpr uint32 SizeOf32() {
    static_assert(sizeof(Type) <= UINT32_MAX);
    return (uint32)sizeof(Type);
}

/// Data
////////////////////////////////////////////////////////////
template<typename Return, typename... Args>
using Func = Return (*)(Args... args);
