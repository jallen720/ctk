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
#define CTK_RANGE(INDEX, START, LIMIT) for (u32 INDEX = start; INDEX < LIMIT; ++INDEX)
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
    CTK_FATAL("string \"%s\" cannot be converted to a boolean value", s)
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
static u32 ctk_size(ctk_array<type, size> *_) {
    return size;
}

template<typename type, u32 size>
static type *ctk_push(ctk_array<type, size> *arr, type elem) {
    if (arr->count + 1 > ctk_size(arr))
        CTK_FATAL("static array (size=%u count=%u) cannot hold any more elements", ctk_size(arr), arr->count)
    type *new_elem = ctk_at(arr, arr->count++);
    *new_elem = elem;
    return new_elem;
}

template<typename type, u32 size>
static type *ctk_push(ctk_array<type, size> *arr) {
    return ctk_push(arr, {});
}

template<typename type, u32 size>
static void ctk_push(ctk_array<type, size> *arr, type *elems, u32 elem_count) {
    if (elem_count == 0)
        return;
    if (arr->count + elem_count > ctk_size(arr))
        CTK_FATAL("static array (size=%u count=%u) cannot hold %u more elements", ctk_size(arr), arr->count, elem_count)
    memcpy(ctk_at(arr, arr->count), elems, sizeof(type) * elem_count);
    arr->count += elem_count;
}

template<typename type, u32 size>
static type ctk_pop(ctk_array<type, size>* arr) {
    CTK_ASSERT(arr->count > 0);
    return arr->data[--arr->count];
}

template<typename type, u32 size>
static u32 ctk_byte_size(ctk_array<type, size> *arr) {
    return ctk_size(arr) * sizeof(type);
}

template<typename type, u32 size>
static u32 ctk_byte_count(ctk_array<type, size> *arr) {
    return arr->count * sizeof(type);
}

template<typename type, u32 size>
static type *ctk_at(ctk_array<type, size> *arr, u32 i) {
    CTK_ASSERT(i < ctk_size(arr))
    return arr->data + i;
}

template<typename type, u32 size>
type &ctk_array<type, size>::operator[](u32 i) {
    CTK_ASSERT(i < size)
    return data[i];
}

template<typename type, u32 size>
static type *operator+(ctk_array<type, size> &arr, u32 i) {
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
static u32 ctk_size(ctk_map<type, size> *_) {
    return size;
}

template<typename type, u32 size>
static type *ctk_push(ctk_map<type, size> *map, cstr key, type val) {
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
static type *ctk_push(ctk_map<type, size> *map, cstr key) {
    return ctk_push(map, key, {});
}

template<typename type, u32 size>
static type *ctk_find(ctk_map<type, size> *map, cstr key) {
    for (u32 i = 0; i < map->count; ++i)
        if (ctk_strings_match(key, (cstr)(map->keys + i)))
            return map->values + i;
    return NULL;
}

template<typename type, u32 size>
static u32 ctk_find_index(ctk_map<type, size> *map, cstr key) {
    for (u32 i = 0; i < map->count; ++i)
        if (ctk_strings_match(key, (cstr)(map->keys + i)))
            return i;
    return CTK_U32_MAX;
}

template<typename type, u32 size>
static type *ctk_at(ctk_map<type, size> *map, cstr key) {
    type *val = ctk_find(map, key);
    if (val == NULL)
        CTK_FATAL("failed to find entry for key \"%s\" in static map", key);
    return val;
}

template<typename type, u32 size>
static u32 ctk_values_byte_size(ctk_map<type, size> *map) {
    return ctk_size(map) * sizeof(type);
}

template<typename type, u32 size>
static u32 ctk_values_byte_count(ctk_map<type, size> *map) {
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
static void ctk_alloc_z_buffer(ctk_buffer<type> *buf, u32 size) {
    buf->data = ctk_alloc_z<type>(size);
    buf->size = size;
}

template<typename type>
static void ctk_realloc_z_buffer(ctk_buffer<type> *buf, u32 new_size) {
    buf->data = ctk_realloc_z(buf->data, buf->size, new_size);
    buf->size = new_size;
}

template<typename type>
static ctk_buffer<type> ctk_create_buffer(u32 size) {
    ctk_buffer<type> buf = {};
    ctk_alloc_z_buffer(&buf, size);
    return buf;
}

template<typename type>
static ctk_buffer<type> ctk_create_buffer_full(u32 size) {
    ctk_buffer<type> buf = {};
    ctk_alloc_z_buffer(&buf, size);
    buf.count = size;
    return buf;
}

template<typename type>
static void ctk_free(ctk_buffer<type> *buf) {
    if (buf->data != NULL) {
        free(buf->data);
        memset(buf, 0, sizeof(*buf));
    }
}

template<typename type>
static type *ctk_push(ctk_buffer<type> *buf, type elem) {
    if (buf->count + 1 > buf->size)
        CTK_FATAL("buffer (size=%u count=%u) cannot hold any more elements", buf->size, buf->count)
    type *new_elem = ctk_at(buf, buf->count++);
    *new_elem = elem;
    return new_elem;
}

template<typename type>
static type *ctk_push(ctk_buffer<type> *buf) {
    return ctk_push(buf, {});
}

template<typename type>
static void ctk_push(ctk_buffer<type> *buf, type *elems, u32 elem_count) {
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
static u32 ctk_byte_size(ctk_buffer<type> *buf) {
    return buf->size * sizeof(type);
}

template<typename type>
static u32 ctk_byte_count(ctk_buffer<type> *buf) {
    return buf->count * sizeof(type);
}

template<typename type>
static type *ctk_at(ctk_buffer<type> *buf, u32 i) {
    CTK_ASSERT(i < buf->size)
    return buf->data + i;
}

template<typename type>
type &ctk_buffer<type>::operator[](u32 i) {
    CTK_ASSERT(i < size)
    return data[i];
}

template<typename type>
static type *operator+(ctk_buffer<type> &buf, u32 i) {
    CTK_ASSERT(i < buf.size)
    return buf.data + i;
}

////////////////////////////////////////////////////////////
/// Heap
////////////////////////////////////////////////////////////
struct _ctk_block {
    void *mem;
    u32 size;
    u32 prev;
    u32 next;
    bool free;
};

struct ctk_heap {
    u32 size;
    u32 active;
    u32 pool;
    ctk_buffer<_ctk_block> blocks;
};

#define _CTK_BLOCK_POOL_CHUNK_COUNT 64

static u32 _ctk_create_block(ctk_heap *heap) {
    u32 new_block_idx = CTK_U32_MAX;
    if (heap->pool != CTK_U32_MAX) {
        new_block_idx = heap->pool;
        heap->pool = heap->blocks[heap->pool].next;
    } else {
        if (heap->blocks.count >= heap->blocks.size)
            ctk_realloc_z_buffer(&heap->blocks, heap->blocks.size + _CTK_BLOCK_POOL_CHUNK_COUNT);
        new_block_idx = heap->blocks.count++;
    }
    if (new_block_idx == CTK_U32_MAX)
        CTK_FATAL("failed to create new block")
    heap->blocks[new_block_idx].prev = CTK_U32_MAX;
    heap->blocks[new_block_idx].next = CTK_U32_MAX;
    return new_block_idx;
}

static void _ctk_destroy_block(ctk_heap *heap, u32 block_idx) {
    heap->blocks[heap->blocks[block_idx].prev].next = heap->blocks[block_idx].next;
    heap->blocks[block_idx].next = heap->pool;
    heap->pool = block_idx;
}

static ctk_heap *ctk_create_heap(u32 size) {
    auto heap = ctk_alloc_z<ctk_heap>();
    heap->size = size;
    heap->pool = CTK_U32_MAX;
    heap->blocks = ctk_create_buffer<_ctk_block>(_CTK_BLOCK_POOL_CHUNK_COUNT);
    heap->active = _ctk_create_block(heap);
    heap->blocks[heap->active].mem = malloc(size);
    CTK_ASSERT(heap->blocks[heap->active].mem != NULL);
    heap->blocks[heap->active].size = size;
    heap->blocks[heap->active].free = true;
    return heap;
}

static void _ctk_join(ctk_heap *heap, u32 block_a, u32 block_b) {
    heap->blocks[block_a].next = block_b;
    heap->blocks[block_b].prev = block_a;
}

static void *ctk_alloc(ctk_heap *heap, u32 size) {
    // Find smallest block large enough for allocation.
    u32 search_block_idx = heap->active;
    u32 selected_block_size = CTK_U32_MAX;
    u32 selected_block_idx = CTK_U32_MAX;
    while (search_block_idx != CTK_U32_MAX) {
        if (heap->blocks[search_block_idx].free && heap->blocks[search_block_idx].size >= size && heap->blocks[search_block_idx].size < selected_block_size) {
            selected_block_idx = search_block_idx;
            selected_block_size = heap->blocks[search_block_idx].size;
        }
        search_block_idx = heap->blocks[search_block_idx].next;
    }
    if (selected_block_idx == CTK_U32_MAX)
        CTK_FATAL("failed to find memory block with size >= %u", size)

    // Create block representing allocation.
    u32 alloc_block_idx = CTK_U32_MAX;
    if (heap->blocks[selected_block_idx].size == size) {
        alloc_block_idx = selected_block_idx;
    } else {
        // Create new block to represent allocation.
        alloc_block_idx = _ctk_create_block(heap);
        heap->blocks[alloc_block_idx].mem = heap->blocks[selected_block_idx].mem;
        heap->blocks[alloc_block_idx].size = size;

        // Insert alloc block before selected block, setting heap->active to new block if selected block was heap->active.
        if (heap->blocks[selected_block_idx].prev != CTK_U32_MAX)
            _ctk_join(heap, heap->blocks[selected_block_idx].prev, alloc_block_idx);
        else
            heap->active = alloc_block_idx;
        _ctk_join(heap, alloc_block_idx, selected_block_idx);

        // Resize block.
        heap->blocks[selected_block_idx].mem = (char *)heap->blocks[selected_block_idx].mem + size;
        heap->blocks[selected_block_idx].size -= size;
    }
    heap->blocks[alloc_block_idx].free = false;

    return heap->blocks[alloc_block_idx].mem;
}

static void *ctk_alloc_z(ctk_heap *heap, u32 size) {
    void *mem = ctk_alloc(heap, size);
    memset(mem, 0, size);
    return mem;
}

template<typename type>
static type *ctk_alloc(ctk_heap *heap, u32 count) {
    return (type *)ctk_alloc(heap, sizeof(type) * count);
}

template<typename type>
static type *ctk_alloc_z(ctk_heap *heap, u32 count) {
    return (type *)ctk_alloc_z(heap, sizeof(type) * count);
}

static void ctk_free(ctk_heap *heap, void *mem) {
    // Find block associated with memory.
    u32 block_idx = heap->active;
    while (block_idx != CTK_U32_MAX) {
        if (heap->blocks[block_idx].mem == mem)
            break;
        block_idx = heap->blocks[block_idx].next;
    }
    if (block_idx == CTK_U32_MAX)
        CTK_FATAL("failed to find memory block associated with addres %p", mem)
    heap->blocks[block_idx].free = true;

    // Merge block with next and prev if free.
    u32 next_idx = heap->blocks[block_idx].next;
    if (next_idx != CTK_U32_MAX && heap->blocks[next_idx].free) {
        heap->blocks[block_idx].size += heap->blocks[next_idx].size;
        _ctk_destroy_block(heap, next_idx);
    }
    u32 prev_idx = heap->blocks[block_idx].prev;
    if (prev_idx != CTK_U32_MAX && heap->blocks[prev_idx].free) {
        heap->blocks[prev_idx].size += heap->blocks[block_idx].size;
        _ctk_destroy_block(heap, block_idx);
    }
}

static void _ctk_visualize_heap(ctk_heap *heap) {
    auto buf = (char *)malloc(heap->size);
    memset(buf, '!', heap->size);
    char *buf_head = buf;
    u32 block_idx = heap->active;
    while (block_idx != CTK_U32_MAX) {
        u32 block_size = heap->blocks[block_idx].size;
        if (block_size == 1) {
            buf_head += sprintf(buf_head, "%c", heap->blocks[block_idx].free ? '_' : '#');
        } else if (block_size == 2) {
            buf_head += sprintf(buf_head, "[%c", heap->blocks[block_idx].free ? '_' : '#');
        } else {
            buf_head += sprintf(buf_head, "[");
            char c = heap->blocks[block_idx].free ? '_' : '#';
            memset(buf_head, heap->blocks[block_idx].free ? '_' : '#', block_size);
            buf_head += block_size - 2;
            buf_head += sprintf(buf_head, "]");
        }
        block_idx = heap->blocks[block_idx].next;
    }
    ctk_print_line("%.*s", heap->size, buf);
}

static void _ctk_dump_block_children(ctk_heap *heap, u32 block_idx, char *out_buf, u32 buf_size) {
    char *end = out_buf + buf_size;
    while (block_idx != CTK_U32_MAX) {
        u32 prev = heap->blocks[block_idx].prev;
        u32 next = heap->blocks[block_idx].next;

        out_buf += sprintf(out_buf, "    [%u]: {\n", block_idx);
        out_buf += sprintf(out_buf, "        status: %s\n", heap->blocks[block_idx].free ? "free" : "used");
        out_buf += sprintf(out_buf, "        mem:    %p\n", heap->blocks[block_idx].mem);
        out_buf += sprintf(out_buf, "        size:   %u\n", heap->blocks[block_idx].size);
        if (prev == CTK_U32_MAX)
            out_buf += sprintf(out_buf, "        prev:   CTK_U32_MAX\n");
        else
            out_buf += sprintf(out_buf, "        prev:   %u\n", prev);
        if (next == CTK_U32_MAX)
            out_buf += sprintf(out_buf, "        next:   CTK_U32_MAX\n");
        else
            out_buf += sprintf(out_buf, "        next:   %u\n", next);
        out_buf += sprintf(out_buf, "    }\n");

        if (out_buf >= end)
            CTK_FATAL("writing past end of buffer")
        block_idx = heap->blocks[block_idx].next;
    }
}

#define _CTK_MAX_OUTPUT_PER_BLOCK 256

static void _ctk_dump_heap(ctk_heap *heap) {
    u32 buf_size = heap->blocks.count * _CTK_MAX_OUTPUT_PER_BLOCK;
    auto buf = (char *)malloc(buf_size);
    memset(buf, 0, buf_size);
    _ctk_dump_block_children(heap, heap->active, buf, buf_size);
    ctk_print_line("active:\n%s", buf);
    memset(buf, 0, buf_size);
    _ctk_dump_block_children(heap, heap->pool, buf, buf_size);
    ctk_print_line("pool:\n%s", buf);
    ctk_print_line();
    free(buf);
}

////////////////////////////////////////////////////////////
/// File IO
////////////////////////////////////////////////////////////
template<typename type>
static ctk_buffer<type> ctk_read_file(cstr path) {
    CTK_ASSERT(path != NULL)
    ctk_buffer<type> elems = {};
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

// vec4
template<typename type>
struct ctk_vec4 {
    type x;
    type y;
    type z;
    type w;
    type operator[](u32 i);
};

template<typename type>
type ctk_vec4<type>::operator[](u32 i) {
    CTK_ASSERT(i < 4)
    return *(&x + i);
}

template<typename l_type, typename r_type>
static ctk_vec4<l_type> operator*(ctk_vec4<l_type> const &l, r_type r) {
    return {
        l.x * r,
        l.y * r,
        l.z * r,
        l.w * r,
    };
}

// vec3
template<typename type>
struct ctk_vec3 {
    type x;
    type y;
    type z;
    type operator[](u32 i);

    template<typename r_type>
    ctk_vec3<type> &operator+=(ctk_vec3<r_type> const &r);

    template<typename r_type>
    ctk_vec3<type> &operator*=(r_type r);
};

template<typename type>
type ctk_vec3<type>::operator[](u32 i) {
    CTK_ASSERT(i < 3)
    return *(&x + i);
}

template<typename type>
template<typename r_type>
ctk_vec3<type> &ctk_vec3<type>::operator+=(ctk_vec3<r_type> const &r) {
    x += r.x;
    y += r.y;
    z += r.z;
    return *this;
}

template<typename type>
template<typename r_type>
ctk_vec3<type> &ctk_vec3<type>::operator*=(r_type r) {
    x *= r;
    y *= r;
    z *= r;
    return *this;
}

template<typename l_type, typename r_type>
static ctk_vec3<l_type> operator+(ctk_vec3<l_type> const &l, ctk_vec3<r_type> const &r) {
    return {
        l.x + r.x,
        l.y + r.y,
        l.z + r.z,
    };
}

template<typename l_type, typename r_type>
static ctk_vec3<l_type> operator-(ctk_vec3<l_type> const &l, ctk_vec3<r_type> const &r) {
    return {
        l.x - r.x,
        l.y - r.y,
        l.z - r.z,
    };
}

template<typename l_type, typename r_type>
static ctk_vec3<l_type> operator*(ctk_vec3<l_type> const &l, r_type r) {
    return {
        l.x * r,
        l.y * r,
        l.z * r,
    };
}

template<typename l_type, typename r_type>
static ctk_vec3<l_type> operator*(ctk_vec3<l_type> const &l, ctk_vec3<r_type> const &r) {
    return {
        l.x * r.x,
        l.y * r.y,
        l.z * r.z,
    };
}

template<typename l_type, typename r_type>
static ctk_vec3<l_type> operator/(ctk_vec3<l_type> const &l, r_type r) {
    return {
        l.x / r,
        l.y / r,
        l.z / r,
    };
}

static f32 ctk_length(ctk_vec3<f32> v) {
    return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

static ctk_vec3<f32> ctk_normalize(ctk_vec3<f32> v) {
    return v / ctk_length(v);
}

// vec2
template<typename type>
struct ctk_vec2 {
    type x;
    type y;
    type operator[](u32 i);

    template<typename r_type>
    ctk_vec2<type> &operator+=(ctk_vec2<r_type> const &r);
};

template<typename type>
type ctk_vec2<type>::operator[](u32 i) {
    CTK_ASSERT(i < 2)
    return *(&x + i);
}

template<typename type>
template<typename r_type>
ctk_vec2<type> &ctk_vec2<type>::operator+=(ctk_vec2<r_type> const &r) {
    x += r.x;
    y += r.y;
    return *this;
}

template<typename l_type, typename r_type>
static ctk_vec2<l_type> operator+(ctk_vec2<l_type> const &l, ctk_vec2<r_type> const &r) {
    return {
        l.x + r.x,
        l.y + r.y,
    };
}

template<typename l_type, typename r_type>
static ctk_vec2<l_type> operator-(ctk_vec2<l_type> const &l, ctk_vec2<r_type> const &r) {
    return {
        l.x - r.x,
        l.y - r.y,
    };
}

template<typename l_type, typename r_type>
static ctk_vec2<l_type> operator*(ctk_vec2<l_type> const &l, r_type r) {
    return {
        l.x * r,
        l.y * r,
    };
}

template<typename l_type, typename r_type>
static bool operator==(ctk_vec2<l_type> const &l, ctk_vec2<r_type> const &r) {
    return l.x == r.x && l.y == r.y;
}

template<typename l_type, typename r_type>
static bool operator!=(ctk_vec2<l_type> const &l, ctk_vec2<r_type> const &r) {
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

static ctk_mtx4 operator*(ctk_mtx4 &l, ctk_mtx4 &r) {
    ctk_mtx4 res = {};
    for (u32 row = 0; row < 4; ++row)
    for (u32 col = 0; col < 4; ++col)
        for (u32 i = 0; i < 4; ++i)
            res[row][col] += l[i][col] * r[row][i];
    return res;
}

static ctk_mtx4 ctk_translate(ctk_mtx4 m, ctk_vec3<f32> v) {
    ctk_mtx4 res = m;
    for (u32 row = 0; row < 3; ++row)
    for (u32 col = 0; col < 3; ++col)
        res[3][col] += v[row] * res[row][col];
    return res;
}

static ctk_mtx4 ctk_scale(ctk_mtx4 m, ctk_vec3<f32> v) {
    ctk_mtx4 res = m;
    for (u32 row = 0; row < 3; ++row)
    for (u32 col = 0; col < 3; ++col)
        res[row][col] *= v[col];
    return res;
}

static ctk_mtx4 ctk_rotate(ctk_mtx4 m, f32 angle, ctk_vec3<f32> axis) {
    ctk_vec3<f32> norm_axis = ctk_normalize(axis);

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
static type *ctk_set(ctk_optional<type> *opt, type val) {
    opt->value = val;
    opt->set = true;
    return &opt->value;
}

template<typename type>
ctk_optional<type>::operator bool() {
    return set;
}

template<typename type>
ctk_optional<type> &ctk_optional<type>::operator=(type val) {
    this->value = val;
    set = true;
    return *this;
}
