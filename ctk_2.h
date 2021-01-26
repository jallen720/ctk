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

////////////////////////////////////////////////////////////
/// Data
////////////////////////////////////////////////////////////
template<typename Type>
struct CTK_Vector4 {
    Type x;
    Type y;
    Type z;
    Type w;
    Type operator[](u32 i);
};

template<typename Type>
struct CTK_Vector3 {
    Type x;
    Type y;
    Type z;
    Type operator[](u32 i);

    template<typename RType>
    CTK_Vector3<Type> &operator+=(CTK_Vector3<RType> const &r);

    template<typename RType>
    CTK_Vector3<Type> &operator*=(RType r);
};

template<typename Type>
struct CTK_Vector2 {
    Type x;
    Type y;
    Type operator[](u32 i);

    template<typename RType>
    CTK_Vector2<Type> &operator+=(CTK_Vector2<RType> const &r);
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

template<typename Type>
union _CTK_PoolNode {
    Type element;
    _CTK_PoolNode *next;
};

template<typename Type>
struct CTK_Pool {
    _CTK_PoolNode<Type> *next;
    u32 chunk_size;
};

template<typename Type>
struct CTK_Array {
    Type *data;
    u32 size;
    u32 chunk_size;
    u32 count;
    bool freeable;
    Type &operator[](u32 i);
};

typedef CTK_Array<char> CTK_String;

template<typename Type, u32 size>
struct CTK_StaticArray {
    Type data[size];
    u32 count;
    Type &operator[](u32 i);
};

template<typename Type, u32 size, u32 key_size = CTK_CACHE_LINE>
struct CTK_StaticMap {
    char keys[size][key_size];
    Type values[size];
    u32 count;
};

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
        for (s32 j = 7; j >= 0; --j) {
            ctk_print("%u", (val_bytes[i] >> j) & 1);
        }

        ctk_print(" ");
    }
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

            if (uniform_spacing) {
                ctk_print(" ");
            }
        }
    }

    ctk_print_line();
}

static void ctk_visualize_string(CTK_String *str, bool uniform_spacing = true) {
    ctk_visualize_string(str->data, str->size, uniform_spacing);
}

template<typename Type>
static void ctk_print_array(CTK_Array<Type> *array) {
    ctk_print("array:\n");
    ctk_print(1, "data:       %p\n", array->data);
    ctk_print(1, "size:       %u\n", array->size);
    ctk_print(1, "chunk_size: %u\n", array->chunk_size);
    ctk_print(1, "count:      %u\n", array->count);
    ctk_print(1, "freeable:   %s\n", array->freeable ? "true" : "false");
}

static void ctk_visualize_stack(CTK_Stack *stack, cstr title = NULL) {
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
/// Math
////////////////////////////////////////////////////////////

// Vector4
template<typename Type>
Type CTK_Vector4<Type>::operator[](u32 i) {
    CTK_ASSERT(i < 4);
    return *(&this->x + i);
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
template<typename Type>
Type CTK_Vector3<Type>::operator[](u32 i) {
    CTK_ASSERT(i < 3);
    return *(&this->x + i);
}

template<typename Type>
template<typename RType>
CTK_Vector3<Type> &CTK_Vector3<Type>::operator+=(CTK_Vector3<RType> const &r) {
    this->x += r.x;
    this->y += r.y;
    this->z += r.z;
    return *this;
}

template<typename Type>
template<typename RType>
CTK_Vector3<Type> &CTK_Vector3<Type>::operator*=(RType r) {
    this->x *= r;
    this->y *= r;
    this->z *= r;
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
template<typename Type>
Type CTK_Vector2<Type>::operator[](u32 i) {
    CTK_ASSERT(i < 2);
    return *(&this->x + i);
}

template<typename Type>
template<typename RType>
CTK_Vector2<Type> &CTK_Vector2<Type>::operator+=(CTK_Vector2<RType> const &r) {
    this->x += r.x;
    this->y += r.y;
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
    return this->data + (row * 4);
}

static CTK_Matrix const CTK_MATRIX_ID = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
};

static CTK_Matrix operator*(CTK_Matrix &l, CTK_Matrix &r) {
    CTK_Matrix res = {};

    for (u32 row = 0; row < 4; ++row) {
        for (u32 col = 0; col < 4; ++col) {
            for (u32 i = 0; i < 4; ++i) {
                res[row][col] += l[i][col] * r[row][i];
            }
        }
    }

    return res;
}

static CTK_Matrix ctk_translate(CTK_Matrix m, CTK_Vector3<f32> v) {
    CTK_Matrix res = m;

    for (u32 row = 0; row < 3; ++row) {
        for (u32 col = 0; col < 3; ++col) {
            res[3][col] += v[row] * res[row][col];
        }
    }

    return res;
}

static CTK_Matrix ctk_scale(CTK_Matrix m, CTK_Vector3<f32> v) {
    CTK_Matrix res = m;

    for (u32 row = 0; row < 3; ++row) {
        for (u32 col = 0; col < 3; ++col) {
            res[row][col] *= v[col];
        }
    }

    return res;
}

static CTK_Matrix ctk_rotate(CTK_Matrix m, f32 angle, CTK_Vector3<f32> axis) {
    CTK_Vector3<f32> norm_axis = ctk_normalize(axis);

}

// General Math Functions
template<typename Type>
static Type ctk_max(Type a, Type b) {
    return a > b ? a : b;
}

template<typename Type>
static Type ctk_min(Type a, Type b) {
    return a < b ? a : b;
}

template<typename Type>
static Type ctk_clamp(Type val, Type min, Type max) {
    return ctk_min(ctk_max(val, min), max);
}

template<typename Type>
static Type ctk_random_range(Type min, Type max) {
    return (max <= min) ? min : ((rand() % (max - min)) + min);
}

static inline u32 ctk_total_chunk_size(u32 min_size, u32 chunk_size) {
    return chunk_size ? (((min_size - 1) / chunk_size) + 1) * chunk_size : min_size;
}

////////////////////////////////////////////////////////////
/// System Memory
////////////////////////////////////////////////////////////
#ifdef __GNUC__
    static void *ctk_alloc_aligned(u32 size, u32 alignment) {
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
    static void *ctk_alloc_aligned(u32 size, u32 alignment) {
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

template<typename Type>
static Type *ctk_alloc_aligned(u32 count, u32 alignment) {
    return (Type *)ctk_alloc_aligned(sizeof(Type) * count, alignment);
}

static void *ctk_alloc(u32 size) {
    CTK_ASSERT(size > 0);
    void *mem = malloc(size);
    CTK_ASSERT(mem != NULL);
    memset(mem, 0, size);
    return mem;
}

template<typename Type>
static Type *ctk_alloc(u32 count) {
    return (Type *)ctk_alloc(sizeof(Type) * count);
}

static void *ctk_realloc(void *mem, u32 old_size, u32 new_size) {
    mem = realloc(mem, new_size);
    CTK_ASSERT(mem != NULL);

    if (new_size > old_size) {
        memset((u8 *)mem + old_size, 0, new_size - old_size);
    }

    return mem;
}

template<typename Type>
static Type *ctk_realloc(Type *mem, u32 old_count, u32 new_count) {
    return (Type *)ctk_realloc((void *)mem, sizeof(Type) * old_count, sizeof(Type) * new_count);
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

template<typename Type>
static Type *ctk_push(CTK_Stack *stack, u32 count) {
    return (Type *)ctk_push(stack, sizeof(Type) * count);
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

////////////////////////////////////////////////////////////
/// Pool
////////////////////////////////////////////////////////////
template<typename Type>
static CTK_Pool<Type> ctk_create_pool(u32 chunk_size) {
    if (sizeof(Type) < sizeof(void *)) {
        CTK_FATAL("size of Type for CTK_Pool must be >= 8");
    }

    return { NULL, chunk_size };
}

template<typename Type>
static Type *ctk_push(CTK_Pool<Type> *pool) {
    if (!pool->next) {
        pool->next = ctk_alloc<_CTK_PoolNode<Type>>(pool->chunk_size);

        // Point each node except last to next node in allocated chunk.
        for (u32 i = 0; i < pool->chunk_size - 1; ++i) {
            pool->next->next = pool->next + 1;
        }
    }

    Type *element = &pool->next->element;
    pool->next = pool->next->next;
    memset(element, 0, sizeof(Type));
    return element;
}

template<typename Type>
static void ctk_free(CTK_Pool<Type> *pool, void *mem) {
    auto node = (_CTK_PoolNode<Type> *)mem;
    node->next = pool->next;
    pool->next = node;
}

////////////////////////////////////////////////////////////
/// Array
////////////////////////////////////////////////////////////

// System Allocators
template<typename Type>
static CTK_Array<Type> ctk_create_array(u32 init_size, u32 chunk_size = 0) {
    CTK_ASSERT(init_size > 0);
    CTK_Array<Type> array = {};
    u32 total_init_size = ctk_total_chunk_size(init_size, chunk_size);
    array.data = ctk_alloc<Type>(total_init_size);
    array.size = total_init_size;
    array.chunk_size = chunk_size;
    array.freeable = true;
    return array;
}

template<typename Type>
static CTK_Array<Type> ctk_create_array_full(u32 init_size, u32 chunk_size = 0) {
    auto array = ctk_create_array<Type>(init_size, chunk_size);
    array.count = array.size;
    return array;
}

// Stack Allocators
template<typename Type>
static CTK_Array<Type> ctk_create_array(CTK_Stack *stack, u32 size) {
    CTK_ASSERT(size > 0);
    CTK_Array<Type> array = {};
    array.data = ctk_push<Type>(stack, size);
    array.size = size;
    return array;
}

template<typename Type>
static CTK_Array<Type> ctk_create_array_full(CTK_Stack *stack, u32 size) {
    auto array = ctk_create_array<Type>(stack, size);
    array.count = size;
    return array;
}

// Interface
template<typename Type>
static void ctk_realloc(CTK_Array<Type> *array, u32 new_size) {
    CTK_ASSERT(new_size > 0);
    CTK_ASSERT(array->chunk_size != 0);
    u32 total_realloc_size = ctk_total_chunk_size(new_size, array->chunk_size);
    array->data = ctk_realloc(array->data, array->size, total_realloc_size);

    if (!array->data) {
        CTK_FATAL("array reallocation failed");
    }

    array->size = total_realloc_size;
}

template<typename Type>
static void ctk_free(CTK_Array<Type> *array) {
    CTK_ASSERT(array->freeable);
    free(array->data);
}

template<typename Type>
static void _ctk_check_realloc(CTK_Array<Type> *array, u32 new_elem_count) {
    if (array->count + new_elem_count <= array->size) {
        return;
    }

    if (array->chunk_size == 0) {
        CTK_FATAL("array (size=%u count=%u) cannot hold %u more element(s), and cannot be resized as it was "
                  "created with a chunk_size of 0", array->size, array->count, new_elem_count);
    }

    ctk_realloc(array, array->count + new_elem_count);
}

template<typename Type>
static Type *ctk_push(CTK_Array<Type> *array, Type elem) {
    if (array->size == 0) {
        CTK_FATAL("pushing to unallocated array (size=0)");
    }

    _ctk_check_realloc(array, 1);
    Type *new_elem = ctk_get(array, array->count++);
    *new_elem = elem;
    return new_elem;
}

template<typename Type>
static Type *ctk_push(CTK_Array<Type> *array) {
    return ctk_push(array, {});
}

template<typename Type>
static void ctk_push(CTK_Array<Type> *array, Type const *elems, u32 elem_count) {
    CTK_ASSERT(elems && elem_count > 0)

    if (array->size == 0) {
        CTK_FATAL("pushing to unallocated array (size=0)");
    }

    _ctk_check_realloc(array, elem_count);
    memcpy(ctk_get(array, array->count), elems, sizeof(Type) * elem_count);
    array->count += elem_count;
}

template<typename Type>
static void ctk_push(CTK_Array<Type> *array, CTK_Array<Type> *other) {
    ctk_push(array, other->data, other->count);
}

template<typename Type>
static void ctk_clear(CTK_Array<Type> *array) {
    array->count = 0;
}

template<typename Type>
static u32 ctk_byte_size(CTK_Array<Type> *array) {
    return array->size * sizeof(Type);
}

template<typename Type>
static u32 ctk_byte_count(CTK_Array<Type> *array) {
    return array->count * sizeof(Type);
}

template<typename Type>
static Type *ctk_get(CTK_Array<Type> *array, u32 i) {
    CTK_ASSERT(i < array->size);
    return array->data + i;
}

template<typename Type>
Type &CTK_Array<Type>::operator[](u32 i) {
    CTK_ASSERT(i < this->size);
    return this->data[i];
}

template<typename Type>
static Type *operator+(CTK_Array<Type> &array, u32 i) {
    CTK_ASSERT(i < array.size);
    return array.data + i;
}

////////////////////////////////////////////////////////////
/// String
////////////////////////////////////////////////////////////

// C-String
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
    if (ctk_strings_match(s, "true")) {
        return true;
    }

    if (ctk_strings_match(s, "false")) {
        return false;
    }

    CTK_FATAL("string \"%s\" cannot be converted to a boolean value", s);
}

// CTK_String
static void _ctk_check_realloc(CTK_String *string, u32 new_char_count) {
    // Must always be room for null-terminator at the end.
    if (string->count + new_char_count + 1 <= string->size) {
        return;
    }

    if (string->chunk_size == 0) {
        CTK_FATAL("string (size=%u count=%u) including null-terminator cannot hold %u more character(s), and cannot be "
                  "resized as it was created with a chunk_size of 0", string->size, string->count, new_char_count);
    }

    ctk_realloc(string, string->count + new_char_count + 1);
}

static void ctk_push(CTK_String *string, cstr chars, u32 char_count = 0) {
    if (!char_count) {
        char_count = strlen(chars);
    }

    CTK_ASSERT(chars && char_count > 0)

    if (string->size == 0) {
        CTK_FATAL("pushing to unallocated string (size=0)");
    }

    _ctk_check_realloc(string, char_count);
    memcpy(ctk_get(string, string->count), chars, sizeof(char) * char_count);
    string->count += char_count;
}

// System Allocator
static CTK_String ctk_create_string(u32 init_size, u32 chunk_size = 0) {
    return ctk_create_array<char>(init_size, chunk_size);
}

static CTK_String ctk_create_string(cstr str, u32 chunk_size = 0) {
    CTK_ASSERT(str != NULL);
    CTK_String string = ctk_create_string(strlen(str) + 1, chunk_size); // Include space for null-terminator.
    ctk_push(&string, str);
    return string;
}

// Stack Allocator
static CTK_String ctk_create_string(CTK_Stack *stack, u32 size) {
    return ctk_create_array<char>(stack, size);
}

static CTK_String ctk_create_string(CTK_Stack *stack, cstr str, u32 size = 0) {
    CTK_ASSERT(str != NULL);

    if (size && size < strlen(str) + 1) {
        CTK_FATAL("string of size %u can't be created from c-string with size (including null-terminator) %u", size,
                  strlen(str) + 1);
    }

    CTK_String string = ctk_create_string(stack, size ? size : strlen(str) + 1);
    ctk_push(&string, str);
    return string;
}

// Interface
template<typename ...Args>
static void ctk_print(CTK_String *str, cstr msg, Args... args) {
    u32 msg_size = snprintf(NULL, 0, msg, args...);
    _ctk_check_realloc(str, msg_size); // Will ensure room for printed message and null-terminator.
    snprintf(str->data + str->count, msg_size + 1, msg, args...);
    str->count += msg_size;
}

static void ctk_print_tabs(CTK_String *str, u32 tabs) {
    CTK_REPEAT(tabs) {
        ctk_print(str, "    ");
    }
}

template<typename ...Args>
static void ctk_print(CTK_String *str, u32 tabs, cstr msg, Args... args) {
    ctk_print_tabs(str, tabs);
    ctk_print(str, msg, args...);
}

static char *ctk_push(CTK_String *string, char c) {
    if (string->size == 0) {
        CTK_FATAL("pushing to unallocated string (size=0)");
    }

    _ctk_check_realloc(string, 1);
    char *new_char = ctk_get(string, string->count++);
    *new_char = c;
    return new_char;
}

static char *ctk_push(CTK_String *string) {
    return ctk_push(string, (char)0);
}

static void ctk_push(CTK_String *string, CTK_String *other) {
    ctk_push(string, other->data, other->count);
}

static bool ctk_strings_match(CTK_String *a, CTK_String *b) {
    return strcmp(a->data, b->data) == 0;
}

static f32 ctk_f32(CTK_String *s) {
    return strtof(s->data, NULL);
}

static f32 ctk_f64(CTK_String *s) {
    return strtod(s->data, NULL);
}

static s32 ctk_s32(CTK_String *s) {
    return strtol(s->data, NULL, 10);
}

static s32 ctk_s64(CTK_String *s) {
    return strtoll(s->data, NULL, 10);
}

static u32 ctk_u32(CTK_String *s) {
    return strtoul(s->data, NULL, 10);
}

static u32 ctk_u64(CTK_String *s) {
    return strtoull(s->data, NULL, 10);
}

static bool ctk_bool(CTK_String *s) {
    if (ctk_strings_match(s->data, "true")) {
        return true;
    }

    if (ctk_strings_match(s->data, "false")) {
        return false;
    }

    CTK_FATAL("string \"%s\" cannot be converted to a boolean value", s->data);
}

////////////////////////////////////////////////////////////
/// Static Array
////////////////////////////////////////////////////////////
template<typename Type, u32 size>
static u32 ctk_size(CTK_StaticArray<Type, size> *_) {
    return size;
}

template<typename Type, u32 size>
static Type *ctk_push(CTK_StaticArray<Type, size> *array, Type elem) {
    if (array->count + 1 > ctk_size(array)) {
        CTK_FATAL("static array (size=%u count=%u) cannot hold any more elements", ctk_size(array), array->count);
    }

    Type *new_elem = ctk_get(array, array->count++);
    *new_elem = elem;
    return new_elem;
}

template<typename Type, u32 size>
static Type *ctk_push(CTK_StaticArray<Type, size> *array) {
    return ctk_push(array, {});
}

template<typename Type, u32 size>
static void ctk_push(CTK_StaticArray<Type, size> *array, Type *elems, u32 elem_count) {
    if (elem_count == 0) {
        return;
    }

    if (array->count + elem_count > ctk_size(array)) {
        CTK_FATAL("static array (size=%u count=%u) cannot hold %u more elements", ctk_size(array), array->count,
                  elem_count);
    }

    memcpy(ctk_get(array, array->count), elems, sizeof(Type) * elem_count);
    array->count += elem_count;
}

template<typename Type, u32 size>
static Type ctk_pop(CTK_StaticArray<Type, size>* array) {
    CTK_ASSERT(array->count > 0);
    return array->data[--array->count];
}

template<typename Type, u32 size>
static u32 ctk_byte_size(CTK_StaticArray<Type, size> *array) {
    return ctk_size(array) * sizeof(Type);
}

template<typename Type, u32 size>
static u32 ctk_byte_count(CTK_StaticArray<Type, size> *array) {
    return array->count * sizeof(Type);
}

template<typename Type, u32 size>
static Type *ctk_get(CTK_StaticArray<Type, size> *array, u32 i) {
    CTK_ASSERT(i < ctk_size(array));
    return array->data + i;
}

template<typename Type, u32 size>
Type &CTK_StaticArray<Type, size>::operator[](u32 i) {
    CTK_ASSERT(i < this->size);
    return this->data[i];
}

template<typename Type, u32 size>
static Type *operator+(CTK_StaticArray<Type, size> &array, u32 i) {
    CTK_ASSERT(i < size);
    return array.data + i;
}

////////////////////////////////////////////////////////////
/// Static Map
////////////////////////////////////////////////////////////
template<typename Type, u32 size, u32 key_size>
static u32 ctk_size(CTK_StaticMap<Type, size, key_size> *_) {
    return size;
}

template<typename Type, u32 size, u32 key_size>
static Type *ctk_push(CTK_StaticMap<Type, size, key_size> *map, cstr key, Type val) {
    if (map->count + 1 > ctk_size(map)) {
        CTK_FATAL("static map (size=%u count=%u) cannot hold any more elements", ctk_size(map), map->count);
    }

    if (strlen(key) >= key_size) {
        CTK_FATAL("pushing key: \"%s\" (size=%u) which is longer than max key size: %u", key, strlen(key), key_size - 1);
    }

    // Check if duplicate key.
    if (ctk_find(map, key) != NULL) {
        CTK_FATAL("attempting to push key \"%s\" to static map that already has that key", key);
    }

    strcpy(map->keys[map->count], key);
    map->values[map->count] = val;
    map->count++;
    return map->values + map->count - 1;
}

template<typename Type, u32 size, u32 key_size>
static Type *ctk_push(CTK_StaticMap<Type, size, key_size> *map, cstr key) {
    return ctk_push(map, key, {});
}

template<typename Type, u32 size, u32 key_size>
static Type *ctk_find(CTK_StaticMap<Type, size, key_size> *map, cstr key) {
    for (u32 i = 0; i < map->count; ++i) {
        if (ctk_strings_match(key, (cstr)(map->keys + i))) {
            return map->values + i;
        }
    }

    return NULL;
}

template<typename Type, u32 size, u32 key_size>
static u32 ctk_find_index(CTK_StaticMap<Type, size, key_size> *map, cstr key) {
    for (u32 i = 0; i < map->count; ++i) {
        if (ctk_strings_match(key, (cstr)(map->keys + i))) {
            return i;
        }
    }

    return CTK_U32_MAX;
}

template<typename Type, u32 size, u32 key_size>
static Type *ctk_get(CTK_StaticMap<Type, size, key_size> *map, cstr key) {
    Type *val = ctk_find(map, key);

    if (val == NULL) {
        CTK_FATAL("failed to get entry for key \"%s\" in static map", key);
    }

    return val;
}

template<typename Type, u32 size, u32 key_size>
static u32 ctk_values_byte_size(CTK_StaticMap<Type, size, key_size> *map) {
    return ctk_size(map) * sizeof(Type);
}

template<typename Type, u32 size, u32 key_size>
static u32 ctk_values_byte_count(CTK_StaticMap<Type, size, key_size> *map) {
    return map->count * sizeof(Type);
}

////////////////////////////////////////////////////////////
/// File IO
////////////////////////////////////////////////////////////
template<typename Type>
static CTK_Array<Type> ctk_read_file(cstr path) {
    CTK_ASSERT(path != NULL);
    CTK_Array<Type> elems = {};
    FILE *f = fopen(path, "rb");

    if (f == NULL) {
        CTK_FATAL("failed to open \"%s\"", path);
    }

    fseek(f, 0, SEEK_END);
    u32 fsize = ftell(f);

    if (fsize > 0) {
        rewind(f);
        elems = ctk_create_array_full<Type>(fsize);
        fread(elems.data, fsize, 1, f);
    }

    fclose(f);
    return elems;
}

////////////////////////////////////////////////////////////
/// Misc.
////////////////////////////////////////////////////////////

// Optional
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

// Pair
template<typename Key, typename Value>
static Value ctk_find_value(Key key, CTK_Pair<Key, Value> *pairs, u32 pair_count) {
    for (u32 i = 0; i < pair_count; ++i) {
        if (pairs[i].key == key) {
            return pairs[i].value;
        }
    }

    return {};
}

template<typename Key, typename Value>
static Value ctk_get_value(Key key, CTK_Pair<Key, Value> *pairs, u32 pair_count) {
    Value res = ctk_find_value(key, pairs, pair_count);

    if (res == NULL) {
        CTK_FATAL("failed to get value from pairs by key");
    }

    return res;
}

template<typename Key, typename Value>
static Key ctk_find_key(Value value, CTK_Pair<Key, Value> *pairs, u32 pair_count) {
    for (u32 i = 0; i < pair_count; ++i) {
        if (pairs[i].value == value) {
            return pairs[i].key;
        }
    }

    return {};
}

template<typename Key, typename Value>
static Key ctk_get_key(Value value, CTK_Pair<Key, Value> *pairs, u32 pair_count) {
    Key res = ctk_find_key(value, pairs, pair_count);

    if (res == NULL) {
        CTK_FATAL("failed to get key from pairs by value");
    }

    return res;
}
