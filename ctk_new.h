#pragma once

#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cmath>

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
using cstr = char const *;

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
    #define CTK_FATAL(MSG, ...) { \
        ctk_print_line(CTK_ERROR_TAG CTK_ANSI_HIGHLIGHT("file", SKY) ": %s", __FILE__); \
        ctk_print_line("       " CTK_ANSI_HIGHLIGHT("line", SKY) ": %i", __LINE__); \
        ctk_print_line("       " CTK_ANSI_HIGHLIGHT("func", SKY) ": %s()", __FUNCTION__); \
        ctk_print("       " CTK_ANSI_HIGHLIGHT("msg", SKY) ": "); \
        ctk_print_line(MSG, ## __VA_ARGS__); \
        throw 0; \
    }
    #define CTK_TODO(MSG, ...) { \
        ctk_print(CTK_ANSI_HIGHLIGHT("TODO", RED) "["); \
        ctk_print("file: " CTK_ANSI_HIGHLIGHT("%s", SKY) " ", __FILE__); \
        ctk_print("line: " CTK_ANSI_HIGHLIGHT("%i", SKY) " ", __LINE__); \
        ctk_print("func: " CTK_ANSI_HIGHLIGHT("%s()", SKY) "]: ", __FUNCTION__); \
        ctk_print_line(MSG, ## __VA_ARGS__); \
    }
#else
    #define CTK_FATAL(MSG, ...) { \
        ctk_print_line(CTK_ERROR_TAG CTK_ANSI_HIGHLIGHT("file", SKY) ": %s", __FILE__); \
        ctk_print_line("       " CTK_ANSI_HIGHLIGHT("line", SKY) ": %i", __LINE__); \
        ctk_print_line("       " CTK_ANSI_HIGHLIGHT("func", SKY) ": %s()", __FUNCTION__); \
        ctk_print("       " CTK_ANSI_HIGHLIGHT("msg", SKY) ": "); \
        ctk_print_line(MSG, __VA_ARGS__); \
        throw 0; \
    }
    #define CTK_TODO(MSG, ...) { \
        ctk_print(CTK_ANSI_HIGHLIGHT("TODO", RED) "["); \
        ctk_print("file: " CTK_ANSI_HIGHLIGHT("%s", SKY) " ", __FILE__); \
        ctk_print("line: " CTK_ANSI_HIGHLIGHT("%i", SKY) " ", __LINE__); \
        ctk_print("func: " CTK_ANSI_HIGHLIGHT("%s()", SKY) "]: ", __FUNCTION__); \
        ctk_print_line(MSG, __VA_ARGS__); \
    }
#endif

#define CTK_ARRAY_COUNT(ARRAY) sizeof(ARRAY) / sizeof(ARRAY[0])
#define CTK_ASSERT(STATEMENT) if (!(STATEMENT)) { CTK_FATAL("assertion \"%s\" failed", #STATEMENT) }
#define CTK_NAME_VALUE_PAIR(VALUE) { #VALUE, VALUE }
#define CTK_VALUE_NAME_PAIR(VALUE) { VALUE, #VALUE }
#define CTK_REPEAT(COUNT) for (u32 _ = 0; _ < COUNT; ++_)
#define CTK_EACH(TYPE, ITER, ARRAY) for (TYPE *ITER = ARRAY + 0; ITER < ARRAY.data + ARRAY.count; ++ITER)
#define CTK_KILOBYTE 1000
#define CTK_MEGABYTE 1000 * CTK_KILOBYTE
#define CTK_GIGABYTE 1000 * CTK_MEGABYTE

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

static void ctk_print_tabs(u32 tab_count) {
    CTK_REPEAT(tab_count)
        ctk_print("    ");
}

template<typename ...arg_types>
static void ctk_print(u32 tab_count, cstr msg, arg_types... args) {
    ctk_print_tabs(tab_count);
    ctk_print(msg, args...);
}

template<typename ...arg_types>
static void ctk_print_line(u32 tab_count, cstr msg, arg_types... args) {
    ctk_print_tabs(tab_count);
    ctk_print_line(msg, args...);
}

template<typename ...arg_types>
static void ctk_error(cstr msg, arg_types... args) {
    ctk_print(CTK_ERROR_TAG);
    ctk_print_line(msg, args...);
}

template<typename ...arg_types>
static void ctk_error(u32 tab_count, cstr msg, arg_types... args) {
    ctk_print(CTK_ERROR_TAG);
    ctk_print_tabs(tab_count);
    ctk_print_line(msg, args...);
}

template<typename ...arg_types>
static void ctk_info(cstr msg, arg_types... args) {
    ctk_print(CTK_ANSI_HIGHLIGHT("INFO", GREEN) ": ");
    ctk_print_line(msg, args...);
}

template<typename ...arg_types>
static void ctk_info(u32 tab_count, cstr msg, arg_types... args) {
    ctk_print(CTK_ANSI_HIGHLIGHT("INFO", GREEN) ": ");
    ctk_print_tabs(tab_count);
    ctk_print_line(msg, args...);
}

template<typename ...arg_types>
static void ctk_warning(cstr msg, arg_types... args) {
    ctk_print(CTK_ANSI_HIGHLIGHT("WARNING", MAGENTA) ": ");
    ctk_print_line(msg, args...);
}

template<typename ...arg_types>
static void ctk_warning(u32 tab_count, cstr msg, arg_types... args) {
    ctk_print(CTK_ANSI_HIGHLIGHT("WARNING", MAGENTA) ": ");
    ctk_print_tabs(tab_count);
    ctk_print_line(msg, args...);
}

////////////////////////////////////////////////////////////
/// Memory
////////////////////////////////////////////////////////////
template<typename type>
type *ctk_alloc(u32 count = 1) {
    auto mem = (type *)malloc(sizeof(type) * count);
    CTK_ASSERT(mem != NULL)
    return mem;
}

void *ctk_alloc_z(u32 size) {
    void *mem = malloc(size);
    CTK_ASSERT(mem != NULL)
    memset(mem, 0, size);
    return mem;
}

template<typename type>
type *ctk_alloc_z(u32 count = 1) {
    return (type *)ctk_alloc_z(sizeof(type) * count);
}

template<typename type>
type *ctk_realloc(type *mem, u32 count) {
    realloc(mem, sizeof(type) * count);
    CTK_ASSERT(mem != NULL)
    return mem;
}

void *ctk_realloc_z(void *mem, u32 old_size, u32 new_size) {
    mem = realloc(mem, new_size);
    CTK_ASSERT(mem != NULL)
    if (new_size > old_size)
        memset((u8 *)mem + old_size, 0, new_size - old_size);
    return mem;
}

template<typename type>
type *ctk_realloc_z(type *mem, u32 old_count, u32 new_count) {
    return (type *)ctk_realloc_z((void *)mem, sizeof(type) * old_count, sizeof(type) * new_count);
}

////////////////////////////////////////////////////////////
/// String
////////////////////////////////////////////////////////////
static bool ctk_string_equal(cstr a, cstr b) {
    return strcmp(a, b) == 0;
}

////////////////////////////////////////////////////////////
/// Array
////////////////////////////////////////////////////////////
template<typename type, u32 size>
struct ctk_array {
    type data[size];
    u32 count;
    type &operator[](u32 i);
};

template<typename type, u32 size>
static u32 ctk_size(struct ctk_array<type, size> *_) {
    return size;
}

template<typename type, u32 size>
static type *ctk_push(struct ctk_array<type, size> *arr, type elem) {
    if (arr->count + 1 > ctk_size(arr))
        CTK_FATAL("static array (size=%u count=%u) cannot hold any more elements", ctk_size(arr), arr->count)
    type *new_elem = ctk_at(arr, arr->count++);
    *new_elem = elem;
    return new_elem;
}

template<typename type, u32 size>
static type *ctk_push(struct ctk_array<type, size> *arr) {
    return ctk_push(arr, {});
}

template<typename type, u32 size>
static void ctk_push(struct ctk_array<type, size> *arr, type *elems, u32 elem_count) {
    if (elem_count == 0)
        return;
    if (arr->count + elem_count > ctk_size(arr))
        CTK_FATAL("static array (size=%u count=%u) cannot hold %u more elements", ctk_size(arr), arr->count, elem_count)
    memcpy(ctk_at(arr, arr->count), elems, sizeof(type) * elem_count);
    arr->count += elem_count;
}

template<typename type, u32 size>
static type ctk_pop(struct ctk_array<type, size>* arr) {
    CTK_ASSERT(arr->count > 0);
    return arr->data[--arr->count];
}

template<typename type, u32 size>
static u32 ctk_byte_size(struct ctk_array<type, size> *arr) {
    return ctk_size(arr) * sizeof(type);
}

template<typename type, u32 size>
static u32 ctk_byte_count(struct ctk_array<type, size> *arr) {
    return arr->count * sizeof(type);
}

template<typename type, u32 size>
static type *ctk_at(struct ctk_array<type, size> *arr, u32 i) {
    CTK_ASSERT(i < ctk_size(arr))
    return arr->data + i;
}

template<typename type, u32 size>
type &ctk_array<type, size>::operator[](u32 i) {
    CTK_ASSERT(i < size)
    return data[i];
}

template<typename type, u32 size>
static type *operator+(struct ctk_array<type, size> &arr, u32 i) {
    CTK_ASSERT(i < size)
    return arr.data + i;
}

////////////////////////////////////////////////////////////
/// Map
////////////////////////////////////////////////////////////
typedef char ctk_map_key[64];

template<typename type, u32 size>
struct ctk_map {
    ctk_map_key keys[size];
    type values[size];
    u32 count;
};

template<typename type, u32 size>
static u32 ctk_size(struct ctk_map<type, size> *_) {
    return size;
}

template<typename type, u32 size>
static type *ctk_push(struct ctk_map<type, size> *map, cstr key, type val) {
    if (map->count + 1 > ctk_size(map))
        CTK_FATAL("static map (size=%u count=%u) cannot hold any more elements", ctk_size(map), map->count)
    if (strlen(key) >= sizeof(ctk_map_key))
        CTK_FATAL("pushing key \"%s\" (size=%u) which is longer than max key size of %u", key, strlen(key), sizeof(ctk_map_key) - 1)
    // Check if duplicate key.
    if (ctk_find(map, key) != NULL)
        CTK_FATAL("attempting to push key \"%s\" to static map that already has that key", key);

    strcpy(map->keys[map->count], key);
    map->values[map->count] = val;
    map->count++;
    return map->values + map->count - 1;
}

template<typename type, u32 size>
static type *ctk_push(struct ctk_map<type, size> *map, cstr key) {
    return ctk_push(map, key, {});
}

template<typename type, u32 size>
static type *ctk_find(struct ctk_map<type, size> *map, cstr key) {
    for (u32 i = 0; i < map->count; ++i)
        if (ctk_string_equal(key, (cstr)(map->keys + i)))
            return map->values + i;
    return NULL;
}

template<typename type, u32 size>
static u32 ctk_find_index(struct ctk_map<type, size> *map, cstr key) {
    for (u32 i = 0; i < map->count; ++i)
        if (ctk_string_equal(key, (cstr)(map->keys + i)))
            return i;
    return CTK_U32_MAX;
}

template<typename type, u32 size>
static type *ctk_at(struct ctk_map<type, size> *map, cstr key) {
    type *val = ctk_find(map, key);
    if (val == NULL)
        CTK_FATAL("failed to find entry for key \"%s\" in static map", key);
    return val;
}

template<typename type, u32 size>
static u32 ctk_values_byte_size(struct ctk_map<type, size> *map) {
    return ctk_size(map) * sizeof(type);
}

template<typename type, u32 size>
static u32 ctk_values_byte_count(struct ctk_map<type, size> *map) {
    return map->count * sizeof(type);
}

////////////////////////////////////////////////////////////
/// Buffer
////////////////////////////////////////////////////////////
template<typename type>
struct ctk_buffer {
    type *data;
    u32 size;
    u32 count;
    type &operator[](u32 i);
};

template<typename type>
static void ctk_alloc_z_buffer(struct ctk_buffer<type> *buf, u32 size) {
    buf->data = ctk_alloc_z<type>(size);
    buf->size = size;
}

template<typename type>
static void ctk_realloc_z_buffer(struct ctk_buffer<type> *buf, u32 new_size) {
    buf->data = ctk_realloc_z(buf->data, buf->size, new_size);
    buf->size = new_size;
}

template<typename type>
static struct ctk_buffer<type> ctk_create_buffer(u32 size) {
    struct ctk_buffer<type> buf = {};
    ctk_alloc_z_buffer(&buf, size);
    return buf;
}

template<typename type>
static struct ctk_buffer<type> ctk_create_buffer_full(u32 size) {
    struct ctk_buffer<type> buf = {};
    ctk_alloc_z_buffer(&buf, size);
    buf.count = size;
    return buf;
}

template<typename type>
static void ctk_free(struct ctk_buffer<type> *buf) {
    if (buf->data != NULL) {
        free(buf->data);
        memset(buf, 0, sizeof(*buf));
    }
}

template<typename type>
static type *ctk_push(struct ctk_buffer<type> *buf, type elem) {
    if (buf->count + 1 > buf->size)
        CTK_FATAL("buffer (size=%u count=%u) cannot hold any more elements", buf->size, buf->count)
    type *new_elem = ctk_at(buf, buf->count++);
    *new_elem = elem;
    return new_elem;
}

template<typename type>
static type *ctk_push(struct ctk_buffer<type> *buf) {
    return ctk_push(buf, {});
}

template<typename type>
static void ctk_push(struct ctk_buffer<type> *buf, type *elems, u32 elem_count) {
    if (elem_count == 0)
        return;
    if (buf->size == 0)
        CTK_FATAL("pushing to unallocated buffer (size=0)")
    if (buf->count + elem_count > buf->size)
        CTK_FATAL("buffer (size=%u count=%u) cannot hold %u more elements", buf->size, buf->count, elem_count)
    memcpy(ctk_at(buf, buf->count), elems, sizeof(type) * elem_count);
    buf->count += elem_count;
}

template<typename type>
static u32 ctk_byte_size(struct ctk_buffer<type> *buf) {
    return buf->size * sizeof(type);
}

template<typename type>
static u32 ctk_byte_count(struct ctk_buffer<type> *buf) {
    return buf->count * sizeof(type);
}

template<typename type>
static type *ctk_at(struct ctk_buffer<type> *buf, u32 i) {
    CTK_ASSERT(i < buf->size)
    return buf->data + i;
}

template<typename type>
type &ctk_buffer<type>::operator[](u32 i) {
    CTK_ASSERT(i < size)
    return data[i];
}

template<typename type>
static type *operator+(struct ctk_buffer<type> &buf, u32 i) {
    CTK_ASSERT(i < buf.size)
    return buf.data + i;
}

////////////////////////////////////////////////////////////
/// File IO
////////////////////////////////////////////////////////////
template<typename type>
static struct ctk_buffer<type> ctk_read_file(cstr path) {
    CTK_ASSERT(path != NULL)
    struct ctk_buffer<type> elems = {};
    FILE *f = fopen(path, "rb");
    if (f == NULL)
        CTK_FATAL("failed to open \"%s\"", path)
    fseek(f, 0, SEEK_END);
    u32 fsize = ftell(f);
    if (fsize > 0) {
        rewind(f);
        elems = ctk_create_buffer_full<type>(fsize);
        fread(elems.data, fsize, 1, f);
    }
    fclose(f);
    return elems;
}

////////////////////////////////////////////////////////////
/// Math
////////////////////////////////////////////////////////////

// v4
template<typename type>
struct ctk_v4 {
    type x;
    type y;
    type z;
    type w;
    type operator[](u32 i);
};

template<typename type>
type ctk_v4<type>::operator[](u32 i) {
    CTK_ASSERT(i < 4)
    return *(&x + i);
}

template<typename l_type, typename r_type>
static struct ctk_v4<l_type> operator*(struct ctk_v4<l_type> const &l, r_type r) {
    return {
        l.x * r,
        l.y * r,
        l.z * r,
        l.w * r,
    };
}

// v3
template<typename type>
struct ctk_v3 {
    type x;
    type y;
    type z;
    type operator[](u32 i);

    template<typename r_type>
    struct ctk_v3<type> &operator+=(struct ctk_v3<r_type> const &r);

    template<typename r_type>
    struct ctk_v3<type> &operator*=(r_type r);
};

template<typename type>
type ctk_v3<type>::operator[](u32 i) {
    CTK_ASSERT(i < 3)
    return *(&x + i);
}

template<typename type>
template<typename r_type>
struct ctk_v3<type> &ctk_v3<type>::operator+=(struct ctk_v3<r_type> const &r) {
    x += r.x;
    y += r.y;
    z += r.z;
    return *this;
}

template<typename type>
template<typename r_type>
struct ctk_v3<type> &ctk_v3<type>::operator*=(r_type r) {
    x *= r;
    y *= r;
    z *= r;
    return *this;
}

template<typename l_type, typename r_type>
static struct ctk_v3<l_type> operator+(struct ctk_v3<l_type> const &l, struct ctk_v3<r_type> const &r) {
    return {
        l.x + r.x,
        l.y + r.y,
        l.z + r.z,
    };
}

template<typename l_type, typename r_type>
static struct ctk_v3<l_type> operator-(struct ctk_v3<l_type> const &l, struct ctk_v3<r_type> const &r) {
    return {
        l.x - r.x,
        l.y - r.y,
        l.z - r.z,
    };
}

template<typename l_type, typename r_type>
static struct ctk_v3<l_type> operator*(struct ctk_v3<l_type> const &l, r_type r) {
    return {
        l.x * r,
        l.y * r,
        l.z * r,
    };
}

template<typename l_type, typename r_type>
static struct ctk_v3<l_type> operator*(struct ctk_v3<l_type> const &l, struct ctk_v3<r_type> const &r) {
    return {
        l.x * r.x,
        l.y * r.y,
        l.z * r.z,
    };
}

template<typename l_type, typename r_type>
static struct ctk_v3<l_type> operator/(struct ctk_v3<l_type> const &l, r_type r) {
    return {
        l.x / r,
        l.y / r,
        l.z / r,
    };
}

static f32 ctk_length(struct ctk_v3<f32> v) {
    return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

static struct ctk_v3<f32> ctk_normalize(struct ctk_v3<f32> v) {
    return v / ctk_length(v);
}

// v2
template<typename type>
struct ctk_v2 {
    type x;
    type y;
    type operator[](u32 i);

    template<typename r_type>
    struct ctk_v2<type> &operator+=(struct ctk_v2<r_type> const &r);
};

template<typename type>
type ctk_v2<type>::operator[](u32 i) {
    CTK_ASSERT(i < 2)
    return *(&x + i);
}

template<typename type>
template<typename r_type>
struct ctk_v2<type> &ctk_v2<type>::operator+=(struct ctk_v2<r_type> const &r) {
    x += r.x;
    y += r.y;
    return *this;
}

template<typename l_type, typename r_type>
static struct ctk_v2<l_type> operator+(struct ctk_v2<l_type> const &l, struct ctk_v2<r_type> const &r) {
    return {
        l.x + r.x,
        l.y + r.y,
    };
}

template<typename l_type, typename r_type>
static struct ctk_v2<l_type> operator-(struct ctk_v2<l_type> const &l, struct ctk_v2<r_type> const &r) {
    return {
        l.x - r.x,
        l.y - r.y,
    };
}

template<typename l_type, typename r_type>
static struct ctk_v2<l_type> operator*(struct ctk_v2<l_type> const &l, r_type r) {
    return {
        l.x * r,
        l.y * r,
    };
}

template<typename l_type, typename r_type>
static bool operator==(struct ctk_v2<l_type> const &l, struct ctk_v2<r_type> const &r) {
    return l.x == r.x && l.y == r.y;
}

template<typename l_type, typename r_type>
static bool operator!=(struct ctk_v2<l_type> const &l, struct ctk_v2<r_type> const &r) {
    return l.x != r.x || l.y != r.y;
}

// mtx4
struct ctk_mtx4 {
    f32 data[16];
    f32 *operator[](u32 row);
};

f32 *ctk_mtx4::operator[](u32 row) {
    return data + (row * 4);
}

static ctk_mtx4 const CTK_MTX4_ID = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
};

static struct ctk_mtx4 operator*(struct ctk_mtx4 &l, struct ctk_mtx4 &r) {
    struct ctk_mtx4 res = {};
    for (u32 row = 0; row < 4; ++row)
    for (u32 col = 0; col < 4; ++col)
        for (u32 i = 0; i < 4; ++i)
            res[row][col] += l[i][col] * r[row][i];
    return res;
}

static struct ctk_mtx4 ctk_translate(struct ctk_mtx4 m, struct ctk_v3<f32> v) {
    struct ctk_mtx4 res = m;
    for (u32 row = 0; row < 3; ++row)
    for (u32 col = 0; col < 3; ++col)
        res[3][col] += v[row] * res[row][col];
    return res;
}

static struct ctk_mtx4 ctk_scale(struct ctk_mtx4 m, struct ctk_v3<f32> v) {
    struct ctk_mtx4 res = m;
    for (u32 row = 0; row < 3; ++row)
    for (u32 col = 0; col < 3; ++col)
        res[row][col] *= v[col];
    return res;
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

////////////////////////////////////////////////////////////
/// Optional
////////////////////////////////////////////////////////////
template<typename type>
struct ctk_optional {
    bool set;
    type value;
    operator bool();
    ctk_optional<type> &operator=(type value);
};

template<typename type>
static type *ctk_set(struct ctk_optional<type> *opt, type val) {
    opt->value = val;
    opt->set = true;
    return &opt->value;
}

template<typename type>
ctk_optional<type>::operator bool() {
    return set;
}

template<typename type>
struct ctk_optional<type> &ctk_optional<type>::operator=(type val) {
    this->value = val;
    set = true;
    return *this;
}
