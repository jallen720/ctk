#pragma once

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cmath>

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
#define CTK_ANSI_COLOR_YELLOW "\x1b[33m"
#define CTK_ANSI_COLOR_MAGENTA "\x1b[35m"
#define CTK_ANSI_COLOR_SKY "\x1b[36m"
#define CTK_ANSI_COLOR_BLUE "\x1b[34m"
#define CTK_ANSI_HIGHLIGHT(STR, COLOR) CTK_ANSI_COLOR_ ## COLOR STR CTK_ANSI_RESET
#define CTK_ERROR_TAG CTK_ANSI_HIGHLIGHT("ERROR", RED) ": "

#ifdef __GNUC__
    #define CTK_FATAL(MSG, ...) {\
        ctk_print_line(CTK_ERROR_TAG CTK_ANSI_HIGHLIGHT("file", SKY) ": %s", __FILE__);\
        ctk_print_line("       " CTK_ANSI_HIGHLIGHT("line", SKY) ": %i", __LINE__);\
        ctk_print_line("       " CTK_ANSI_HIGHLIGHT("func", SKY) ": %s()", __FUNCTION__);\
        ctk_print("       " CTK_ANSI_HIGHLIGHT("msg", SKY) ": ");\
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
        ctk_print_line(CTK_ERROR_TAG CTK_ANSI_HIGHLIGHT("file", SKY) ": %s", __FILE__);\
        ctk_print_line("       " CTK_ANSI_HIGHLIGHT("line", SKY) ": %i", __LINE__);\
        ctk_print_line("       " CTK_ANSI_HIGHLIGHT("func", SKY) ": %s()", __FUNCTION__);\
        ctk_print("       " CTK_ANSI_HIGHLIGHT("msg", SKY) ": ");\
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
#define CTK_RANGE(INDEX, START, LIMIT) for (u32 INDEX = start; INDEX < LIMIT; ++INDEX)
#define CTK_EACH(TYPE, ITER, ARRAY) for (TYPE *ITER = ARRAY + 0; ITER < ARRAY.data + ARRAY.count; ++ITER)
#define CTK_KILOBYTE 1000
#define CTK_MEGABYTE 1000 * CTK_KILOBYTE
#define CTK_GIGABYTE 1000 * CTK_MEGABYTE
#define _CTK_CACHE_LINE 64


////////////////////////////////////////////////////////////
/// Data
////////////////////////////////////////////////////////////
template<typename type>
struct CTK_Vector4 {
    type x;
    type y;
    type z;
    type w;
    type operator[](u32 i);
};

template<typename type>
struct CTK_Vector3 {
    type x;
    type y;
    type z;
    type operator[](u32 i);

    template<typename RType>
    CTK_Vector3<type> &operator+=(CTK_Vector3<RType> const &r);

    template<typename RType>
    CTK_Vector3<type> &operator*=(RType r);
};

template<typename type>
struct CTK_Vector2 {
    type x;
    type y;
    type operator[](u32 i);

    template<typename RType>
    CTK_Vector2<type> &operator+=(CTK_Vector2<RType> const &r);
};

struct CTK_Matrix {
    f32 data[16];
    f32 *operator[](u32 row);
};

struct CTK_Stack {
    u8 *mem;
    u32 size;
    u32 count;
};

template<typename type>
union _CTK_PoolNode {
    type element;
    _CTK_PoolNode *next;
};

template<typename type>
struct CTK_Pool {
    _CTK_PoolNode<type> *next;
    u32 chunk_size;
};

template<typename type>
struct CTK_Array {
    type *data;
    u32 size;
    u32 count;
    type &operator[](u32 i);
};

typedef CTK_Array<char> CTK_String;

template<typename type, u32 size>
struct CTK_StaticArray {
    type data[size];
    u32 count;
    type &operator[](u32 i);
};

template<typename type, u32 size, u32 key_size = _CTK_CACHE_LINE>
struct CTK_StaticMap {
    char keys[size][key_size];
    type values[size];
    u32 count;
};

template<typename type>
struct CTK_Optional {
    bool set;
    type value;
    operator bool();
    CTK_Optional<type> &operator=(type value);
};

template<typename key_type, typename val_type>
struct CTK_Pair {
    key_type key;
    val_type value;
};

////////////////////////////////////////////////////////////
/// Logging
////////////////////////////////////////////////////////////
template<typename ...arg_types>
static void ctk_print(cstr msg, arg_types... args) {
    printf(msg, args...);
}

template<typename ...arg_types>
static void ctk_print_line() {
    ctk_print("\n");
}

template<typename ...arg_types>
static void ctk_print_line(cstr msg, arg_types... args) {
    ctk_print(msg, args...);
    ctk_print_line();
}

static void _ctk_print_tabs(u32 tabs) {
    CTK_REPEAT(tabs)
        ctk_print("    ");
}

template<typename ...arg_types>
static void ctk_print(u32 tabs, cstr msg, arg_types... args) {
    _ctk_print_tabs(tabs);
    ctk_print(msg, args...);
}

template<typename ...arg_types>
static void ctk_print_line(u32 tabs, cstr msg, arg_types... args) {
    _ctk_print_tabs(tabs);
    ctk_print_line(msg, args...);
}

template<typename ...arg_types>
static void ctk_error(cstr msg, arg_types... args) {
    ctk_print(CTK_ERROR_TAG);
    ctk_print_line(msg, args...);
}

template<typename ...arg_types>
static void ctk_error(u32 tabs, cstr msg, arg_types... args) {
    ctk_print(CTK_ERROR_TAG);
    _ctk_print_tabs(tabs);
    ctk_print_line(msg, args...);
}

template<typename ...arg_types>
static void ctk_info(cstr msg, arg_types... args) {
    ctk_print(CTK_ANSI_HIGHLIGHT("INFO", GREEN) ": ");
    ctk_print_line(msg, args...);
}

template<typename ...arg_types>
static void ctk_info(u32 tabs, cstr msg, arg_types... args) {
    ctk_print(CTK_ANSI_HIGHLIGHT("INFO", GREEN) ": ");
    _ctk_print_tabs(tabs);
    ctk_print_line(msg, args...);
}

template<typename ...arg_types>
static void ctk_warning(cstr msg, arg_types... args) {
    ctk_print(CTK_ANSI_HIGHLIGHT("WARNING", MAGENTA) ": ");
    ctk_print_line(msg, args...);
}

template<typename ...arg_types>
static void ctk_warning(u32 tabs, cstr msg, arg_types... args) {
    ctk_print(CTK_ANSI_HIGHLIGHT("WARNING", MAGENTA) ": ");
    _ctk_print_tabs(tabs);
    ctk_print_line(msg, args...);
}

////////////////////////////////////////////////////////////
/// Debugging
////////////////////////////////////////////////////////////
template<typename type>
void ctk_print_bits(type val) {
    auto val_bytes = (u8 *)&val;
    for (s32 i = sizeof(type) - 1; i >= 0; --i) {
        for (s32 j = 7; j >= 0; --j)
            ctk_print("%u", (val_bytes[i] >> j) & 1);
        ctk_print(" ");
    }
}

////////////////////////////////////////////////////////////
/// Math
////////////////////////////////////////////////////////////

// Vector4
template<typename type>
type CTK_Vector4<type>::operator[](u32 i) {
    CTK_ASSERT(i < 4);
    return *(&x + i);
}

template<typename LType, typename RType>
static CTK_Vector4<LType> operator*(CTK_Vector4<LType> const &l, RType r) {
    return {
        l.x * r,
        l.y * r,
        l.z * r,
        l.w * r,
    };
}

// Vector3
template<typename type>
type CTK_Vector3<type>::operator[](u32 i) {
    CTK_ASSERT(i < 3);
    return *(&x + i);
}

template<typename type>
template<typename RType>
CTK_Vector3<type> &CTK_Vector3<type>::operator+=(CTK_Vector3<RType> const &r) {
    x += r.x;
    y += r.y;
    z += r.z;
    return *this;
}

template<typename type>
template<typename RType>
CTK_Vector3<type> &CTK_Vector3<type>::operator*=(RType r) {
    x *= r;
    y *= r;
    z *= r;
    return *this;
}

template<typename LType, typename RType>
static CTK_Vector3<LType> operator+(CTK_Vector3<LType> const &l, CTK_Vector3<RType> const &r) {
    return {
        l.x + r.x,
        l.y + r.y,
        l.z + r.z,
    };
}

template<typename LType, typename RType>
static CTK_Vector3<LType> operator-(CTK_Vector3<LType> const &l, CTK_Vector3<RType> const &r) {
    return {
        l.x - r.x,
        l.y - r.y,
        l.z - r.z,
    };
}

template<typename LType, typename RType>
static CTK_Vector3<LType> operator*(CTK_Vector3<LType> const &l, RType r) {
    return {
        l.x * r,
        l.y * r,
        l.z * r,
    };
}

template<typename LType, typename RType>
static CTK_Vector3<LType> operator*(CTK_Vector3<LType> const &l, CTK_Vector3<RType> const &r) {
    return {
        l.x * r.x,
        l.y * r.y,
        l.z * r.z,
    };
}

template<typename LType, typename RType>
static CTK_Vector3<LType> operator/(CTK_Vector3<LType> const &l, RType r) {
    return {
        l.x / r,
        l.y / r,
        l.z / r,
    };
}

static f32 ctk_length(CTK_Vector3<f32> v) {
    return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

static CTK_Vector3<f32> ctk_normalize(CTK_Vector3<f32> v) {
    return v / ctk_length(v);
}

// Vector2
template<typename type>
type CTK_Vector2<type>::operator[](u32 i) {
    CTK_ASSERT(i < 2);
    return *(&x + i);
}

template<typename type>
template<typename RType>
CTK_Vector2<type> &CTK_Vector2<type>::operator+=(CTK_Vector2<RType> const &r) {
    x += r.x;
    y += r.y;
    return *this;
}

template<typename LType, typename RType>
static CTK_Vector2<LType> operator+(CTK_Vector2<LType> const &l, CTK_Vector2<RType> const &r) {
    return {
        l.x + r.x,
        l.y + r.y,
    };
}

template<typename LType, typename RType>
static CTK_Vector2<LType> operator-(CTK_Vector2<LType> const &l, CTK_Vector2<RType> const &r) {
    return {
        l.x - r.x,
        l.y - r.y,
    };
}

template<typename LType, typename RType>
static CTK_Vector2<LType> operator*(CTK_Vector2<LType> const &l, RType r) {
    return {
        l.x * r,
        l.y * r,
    };
}

template<typename LType, typename RType>
static bool operator==(CTK_Vector2<LType> const &l, CTK_Vector2<RType> const &r) {
    return l.x == r.x && l.y == r.y;
}

template<typename LType, typename RType>
static bool operator!=(CTK_Vector2<LType> const &l, CTK_Vector2<RType> const &r) {
    return l.x != r.x || l.y != r.y;
}

// Matrix
f32 *CTK_Matrix::operator[](u32 row) {
    return data + (row * 4);
}

static CTK_Matrix const CTK_MTX4_ID = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
};

static CTK_Matrix operator*(CTK_Matrix &l, CTK_Matrix &r) {
    CTK_Matrix res = {};
    for (u32 row = 0; row < 4; ++row)
    for (u32 col = 0; col < 4; ++col)
        for (u32 i = 0; i < 4; ++i)
            res[row][col] += l[i][col] * r[row][i];
    return res;
}

static CTK_Matrix ctk_translate(CTK_Matrix m, CTK_Vector3<f32> v) {
    CTK_Matrix res = m;
    for (u32 row = 0; row < 3; ++row)
    for (u32 col = 0; col < 3; ++col)
        res[3][col] += v[row] * res[row][col];
    return res;
}

static CTK_Matrix ctk_scale(CTK_Matrix m, CTK_Vector3<f32> v) {
    CTK_Matrix res = m;
    for (u32 row = 0; row < 3; ++row)
    for (u32 col = 0; col < 3; ++col)
        res[row][col] *= v[col];
    return res;
}

static CTK_Matrix ctk_rotate(CTK_Matrix m, f32 angle, CTK_Vector3<f32> axis) {
    CTK_Vector3<f32> norm_axis = ctk_normalize(axis);

}

// General Math Functions
template<typename type>
static type ctk_max(type a, type b) {
    return a > b ? a : b;
}

template<typename type>
static type ctk_min(type a, type b) {
    return a < b ? a : b;
}

template<typename type>
static type ctk_clamp(type val, type min, type max) {
    return ctk_min(ctk_max(val, min), max);
}

template<typename type>
static type ctk_random_range(type min, type max) {
    return (max <= min) ? min : ((rand() % (max - min)) + min);
}

static inline u32 ctk_total_chunk_size(u32 min_size, u32 chunk_size) {
    return (((min_size - 1) / chunk_size) + 1) * chunk_size;
}

////////////////////////////////////////////////////////////
/// System Memory
////////////////////////////////////////////////////////////
#ifdef __GNUC__
    static void *ctk_alloc_aligned(u32 alignment, u32 size) {
        CTK_ASSERT(size > 0);
        CTK_ASSERT(alignment > 0);
        void *mem = aligned_alloc(alignment, size);
        CTK_ASSERT(mem != NULL);
        memset(mem, 0, size);
        return mem;
    }

    static void ctk_free_aligned(void *mem) {
        free(mem);
    }
#else
    static void *ctk_alloc_aligned(u32 alignment, u32 size) {
        CTK_ASSERT(size > 0);
        CTK_ASSERT(alignment > 0);
        void *mem = _aligned_malloc(size, alignment);
        CTK_ASSERT(mem != NULL);
        memset(mem, 0, size);
        return mem;
    }

    static void ctk_free_aligned(void *mem) {
        _aligned_free(mem);
    }
#endif

template<typename type>
static type *ctk_alloc_aligned(u32 alignment, u32 count = 1) {
    return (type *)ctk_alloc_aligned(alignment, sizeof(type) * count);
}

static void *ctk_alloc(u32 size) {
    CTK_ASSERT(size > 0);
    void *mem = malloc(size);
    CTK_ASSERT(mem != NULL);
    memset(mem, 0, size);
    return mem;
}

template<typename type>
static type *ctk_alloc(u32 count = 1) {
    return (type *)ctk_alloc(sizeof(type) * count);
}

static void *ctk_realloc(void *mem, u32 old_size, u32 new_size) {
    mem = realloc(mem, new_size);
    CTK_ASSERT(mem != NULL);
    if (new_size > old_size)
        memset((u8 *)mem + old_size, 0, new_size - old_size);
    return mem;
}

template<typename type>
static type *ctk_realloc(type *mem, u32 old_count, u32 new_count) {
    return (type *)ctk_realloc((void *)mem, sizeof(type) * old_count, sizeof(type) * new_count);
}

////////////////////////////////////////////////////////////
/// Stack
////////////////////////////////////////////////////////////
static u8 *ctk_push(CTK_Stack *stack, u32 size) {
    CTK_ASSERT(stack->count + size <= stack->size);
    u8 *mem = stack->mem + stack->count;
    stack->count += size;
    return mem;
}

template<typename type>
static type *ctk_push(CTK_Stack *stack, u32 count = 1) {
    return (type *)ctk_push(stack, sizeof(type) * count);
}

static CTK_Stack ctk_create_stack(u32 size) {
    CTK_Stack stack = {};
    stack.size = size;
    stack.mem = ctk_alloc<u8>(size);
    return stack;
}

static CTK_Stack ctk_create_stack(CTK_Stack *parent, u32 size) {
    CTK_Stack stack = {};
    stack.size = size;
    stack.mem = ctk_push<u8>(parent, size);
    return stack;
}

static u32 ctk_begin_region(CTK_Stack *stack) {
    return stack->count;
}

static void ctk_end_region(CTK_Stack *stack, u32 region) {
    stack->count = region;
}

static void _ctk_visualize_stack(CTK_Stack *stack, cstr title = NULL) {
    auto buf = (char *)malloc(stack->size + 1);
    memset(buf, '#', stack->count);
    memset(buf + stack->count, '_', stack->size - stack->count);
    buf[stack->size] = '\0';
    ctk_print_line("%s (size: %u, count: %u):", title ? title : "stack", stack->size, stack->count);
    ctk_print_line(buf);
    ctk_print_line();
    free(buf);
}

////////////////////////////////////////////////////////////
/// Pool
////////////////////////////////////////////////////////////
template<typename type>
static CTK_Pool<type> ctk_create_pool(u32 chunk_size) {
    if (sizeof(type) < sizeof(void *))
        CTK_FATAL("size of type for CTK_Pool must be >= 8");
    return { NULL, chunk_size };
}

template<typename type>
static type *ctk_push(CTK_Pool<type> *pool) {
    if (!pool->next) {
        ctk_print_line("allocating new chunk");
        pool->next = ctk_alloc<_CTK_PoolNode<type>>(pool->chunk_size);

        // Point each node except last to next node in allocated chunk.
        for (u32 i = 0; i < pool->chunk_size - 1; ++i)
            pool->next->next = pool->next + 1;
    }
    type *element = &pool->next->element;
    pool->next = pool->next->next;
    memset(element, 0, sizeof(type));
    return element;
}

template<typename type>
static void ctk_free(CTK_Pool<type> *pool, void *mem) {
    auto node = (_CTK_PoolNode<type> *)mem;
    node->next = pool->next;
    pool->next = node;
}

////////////////////////////////////////////////////////////
/// Array
////////////////////////////////////////////////////////////
template<typename type>
static CTK_Array<type> ctk_create_array(u32 size) {
    CTK_Array<type> arr = {};
    arr.data = ctk_alloc<type>(size);
    arr.size = size;
    return arr;
}

template<typename type>
static CTK_Array<type> ctk_create_array_full(u32 size) {
    auto arr = ctk_create_array<type>(size);
    arr.count = size;
    return arr;
}

template<typename type>
static CTK_Array<type> ctk_create_array(u32 size, CTK_Stack *stack) {
    CTK_Array<type> arr = {};
    arr.data = ctk_alloc<type>(stack, size);
    arr.size = size;
    return arr;
}

template<typename type>
static CTK_Array<type> ctk_create_array_full(u32 size, CTK_Stack *stack) {
    auto arr = ctk_create_array<type>(size, stack);
    arr.count = size;
    return arr;
}

template<typename type>
static void ctk_realloc(CTK_Array<type> *arr, u32 new_size) {
    arr->data = ctk_realloc(arr->data, arr->size, new_size);
    arr->size = new_size;
}

template<typename type>
static void ctk_free(CTK_Array<type> *arr) {
    free(arr->data);
}

template<typename type>
static type *ctk_push(CTK_Array<type> *arr, type elem) {
    if (arr->count + 1 > arr->size)
        CTK_FATAL("array (size=%u count=%u) cannot hold any more elements", arr->size, arr->count);
    type *new_elem = ctk_get(arr, arr->count++);
    *new_elem = elem;
    return new_elem;
}

template<typename type>
static type *ctk_push(CTK_Array<type> *arr) {
    return ctk_push(arr, {});
}

template<typename type>
static void ctk_push(CTK_Array<type> *arr, type *elems, u32 elem_count) {
    if (elem_count == 0)
        return;
    if (arr->size == 0)
        CTK_FATAL("pushing to unallocated array (size=0)");
    if (arr->count + elem_count > arr->size)
        CTK_FATAL("array (size=%u count=%u) cannot hold %u more elements", arr->size, arr->count, elem_count);
    memcpy(ctk_get(arr, arr->count), elems, sizeof(type) * elem_count);
    arr->count += elem_count;
}

template<typename type>
static u32 ctk_byte_size(CTK_Array<type> *arr) {
    return arr->size * sizeof(type);
}

template<typename type>
static u32 ctk_byte_count(CTK_Array<type> *arr) {
    return arr->count * sizeof(type);
}

template<typename type>
static type *ctk_get(CTK_Array<type> *arr, u32 i) {
    CTK_ASSERT(i < arr->size);
    return arr->data + i;
}

template<typename type>
type &CTK_Array<type>::operator[](u32 i) {
    CTK_ASSERT(i < size);
    return data[i];
}

template<typename type>
static type *operator+(CTK_Array<type> &arr, u32 i) {
    CTK_ASSERT(i < arr.size);
    return arr.data + i;
}

////////////////////////////////////////////////////////////
/// String
////////////////////////////////////////////////////////////
static CTK_String ctk_create_string(u32 init_size) {
    return ctk_create_array<char>(init_size);
}

static void ctk_free(CTK_String *str) {
    free(str->data);
}

template<typename ...arg_types>
static void ctk_write(CTK_String *str, cstr msg, arg_types... args) {
    u32 write_size = snprintf(str->data, str->size - str->count, msg, args...) + 1;
    if (str->count + write_size > str->size) {
        ctk_realloc(str, ctk_total_chunk_size(str->count + write_size, str->size));
        snprintf(str->data + str->count, str->size - str->count, msg, args...);
    }
    str->count += write_size;
}

static void _ctk_write_tabs(CTK_String *str, u32 tabs) {
    CTK_REPEAT(tabs)
        ctk_write(str, "    ");
}

template<typename ...arg_types>
static void ctk_write(CTK_String *str, u32 tabs, cstr msg, arg_types... args) {
    _ctk_write_tabs(str, tabs);
    ctk_write(str, msg, args...);
}

static bool ctk_strings_match(cstr a, cstr b) {
    return strcmp(a, b) == 0;
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

static void ctk_visualize_string(cstr str, u32 size, bool uniform_spacing = true) {
    for (u32 i = 0; i < size; ++i) {
        char c = str[i];
        if (c == '\r') {
            ctk_print("\\r");
            continue;
        } else if (c == '\0') {
            ctk_print("\\0");
        } else if (c == '\n') {
            ctk_print_line("\\n");
        } else {
            ctk_print("%c", c);
            if (uniform_spacing)
                ctk_print(" ");
        }
    }
    ctk_print_line();
}

static void ctk_visualize_string(CTK_String *str, bool uniform_spacing = true) {
    ctk_visualize_string(str->data, str->size, uniform_spacing);
}

////////////////////////////////////////////////////////////
/// Static Array
////////////////////////////////////////////////////////////
template<typename type, u32 size>
static u32 ctk_size(CTK_StaticArray<type, size> *_) {
    return size;
}

template<typename type, u32 size>
static type *ctk_push(CTK_StaticArray<type, size> *arr, type elem) {
    if (arr->count + 1 > ctk_size(arr))
        CTK_FATAL("static array (size=%u count=%u) cannot hold any more elements", ctk_size(arr), arr->count);
    type *new_elem = ctk_get(arr, arr->count++);
    *new_elem = elem;
    return new_elem;
}

template<typename type, u32 size>
static type *ctk_push(CTK_StaticArray<type, size> *arr) {
    return ctk_push(arr, {});
}

template<typename type, u32 size>
static void ctk_push(CTK_StaticArray<type, size> *arr, type *elems, u32 elem_count) {
    if (elem_count == 0)
        return;
    if (arr->count + elem_count > ctk_size(arr))
        CTK_FATAL("static array (size=%u count=%u) cannot hold %u more elements", ctk_size(arr), arr->count, elem_count);
    memcpy(ctk_get(arr, arr->count), elems, sizeof(type) * elem_count);
    arr->count += elem_count;
}

template<typename type, u32 size>
static type ctk_pop(CTK_StaticArray<type, size>* arr) {
    CTK_ASSERT(arr->count > 0);
    return arr->data[--arr->count];
}

template<typename type, u32 size>
static u32 ctk_byte_size(CTK_StaticArray<type, size> *arr) {
    return ctk_size(arr) * sizeof(type);
}

template<typename type, u32 size>
static u32 ctk_byte_count(CTK_StaticArray<type, size> *arr) {
    return arr->count * sizeof(type);
}

template<typename type, u32 size>
static type *ctk_get(CTK_StaticArray<type, size> *arr, u32 i) {
    CTK_ASSERT(i < ctk_size(arr));
    return arr->data + i;
}

template<typename type, u32 size>
type &CTK_StaticArray<type, size>::operator[](u32 i) {
    CTK_ASSERT(i < size);
    return data[i];
}

template<typename type, u32 size>
static type *operator+(CTK_StaticArray<type, size> &arr, u32 i) {
    CTK_ASSERT(i < size);
    return arr.data + i;
}

////////////////////////////////////////////////////////////
/// Static Map
////////////////////////////////////////////////////////////
template<typename type, u32 size, u32 key_size>
static u32 ctk_size(CTK_StaticMap<type, size, key_size> *_) {
    return size;
}

template<typename type, u32 size, u32 key_size>
static type *ctk_push(CTK_StaticMap<type, size, key_size> *map, cstr key, type val) {
    if (map->count + 1 > ctk_size(map))
        CTK_FATAL("static map (size=%u count=%u) cannot hold any more elements", ctk_size(map), map->count);
    if (strlen(key) >= key_size)
        CTK_FATAL("pushing key: \"%s\" (size=%u) which is longer than max key size: %u", key, strlen(key), key_size - 1);
    // Check if duplicate key.
    if (ctk_find(map, key) != NULL)
        CTK_FATAL("attempting to push key \"%s\" to static map that already has that key", key);

    strcpy(map->keys[map->count], key);
    map->values[map->count] = val;
    map->count++;
    return map->values + map->count - 1;
}

template<typename type, u32 size, u32 key_size>
static type *ctk_push(CTK_StaticMap<type, size, key_size> *map, cstr key) {
    return ctk_push(map, key, {});
}

template<typename type, u32 size, u32 key_size>
static type *ctk_find(CTK_StaticMap<type, size, key_size> *map, cstr key) {
    for (u32 i = 0; i < map->count; ++i)
        if (ctk_strings_match(key, (cstr)(map->keys + i)))
            return map->values + i;
    return NULL;
}

template<typename type, u32 size, u32 key_size>
static u32 ctk_find_index(CTK_StaticMap<type, size, key_size> *map, cstr key) {
    for (u32 i = 0; i < map->count; ++i)
        if (ctk_strings_match(key, (cstr)(map->keys + i)))
            return i;
    return CTK_U32_MAX;
}

template<typename type, u32 size, u32 key_size>
static type *ctk_get(CTK_StaticMap<type, size, key_size> *map, cstr key) {
    type *val = ctk_find(map, key);
    if (val == NULL)
        CTK_FATAL("failed to get entry for key \"%s\" in static map", key);
    return val;
}

template<typename type, u32 size, u32 key_size>
static u32 ctk_values_byte_size(CTK_StaticMap<type, size, key_size> *map) {
    return ctk_size(map) * sizeof(type);
}

template<typename type, u32 size, u32 key_size>
static u32 ctk_values_byte_count(CTK_StaticMap<type, size, key_size> *map) {
    return map->count * sizeof(type);
}

////////////////////////////////////////////////////////////
/// File IO
////////////////////////////////////////////////////////////
template<typename type>
static CTK_Array<type> ctk_read_file(cstr path) {
    CTK_ASSERT(path != NULL);
    CTK_Array<type> elems = {};
    FILE *f = fopen(path, "rb");
    if (f == NULL)
        CTK_FATAL("failed to open \"%s\"", path);
    fseek(f, 0, SEEK_END);
    u32 fsize = ftell(f);
    if (fsize > 0) {
        rewind(f);
        elems = ctk_create_array_full<type>(fsize);
        fread(elems.data, fsize, 1, f);
    }
    fclose(f);
    return elems;
}

////////////////////////////////////////////////////////////
/// Misc.
////////////////////////////////////////////////////////////

// Optional
template<typename type>
static type *ctk_set(CTK_Optional<type> *opt, type val) {
    opt->value = val;
    opt->set = true;
    return &opt->value;
}

template<typename type>
CTK_Optional<type> &CTK_Optional<type>::operator=(type val) {
    this->value = val;
    set = true;
    return *this;
}

// Pair
template<typename key_type, typename val_type>
static val_type ctk_find_value(key_type key, CTK_Pair<key_type, val_type> *pairs, u32 pair_count) {
    for (u32 i = 0; i < pair_count; ++i)
        if (pairs[i].key == key)
            return pairs[i].value;
    return {};
}

template<typename key_type, typename val_type>
static val_type ctk_get_value(key_type key, CTK_Pair<key_type, val_type> *pairs, u32 pair_count) {
    val_type res = ctk_find_value(key, pairs, pair_count);
    if (res == NULL)
        CTK_FATAL("failed to get value from pairs by key");
    return res;
}

template<typename key_type, typename val_type>
static key_type ctk_find_key(val_type value, CTK_Pair<key_type, val_type> *pairs, u32 pair_count) {
    for (u32 i = 0; i < pair_count; ++i)
        if (pairs[i].value == value)
            return pairs[i].key;
    return {};
}

template<typename key_type, typename val_type>
static key_type ctk_get_key(val_type value, CTK_Pair<key_type, val_type> *pairs, u32 pair_count) {
    key_type res = ctk_find_key(value, pairs, pair_count);
    if (res == NULL)
        CTK_FATAL("failed to get key from pairs by value");
    return res;
}
