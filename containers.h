#pragma once

#include <cstdlib>
#include <cstring>
#include "ctk/ctk.h"
#include "ctk/memory.h"

////////////////////////////////////////////////////////////
/// Data
////////////////////////////////////////////////////////////
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
    CTK_Allocator *allocator;
    Type *data;
    u32 size;
    u32 chunk_size;
    u32 count;

    Type &operator[](u32 i);
};

typedef CTK_Array<char> CTK_String;

struct CTK_CharRange {
    cstr data;
    u64 size;
};

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

////////////////////////////////////////////////////////////
/// Debugging
////////////////////////////////////////////////////////////
static void ctk_visualize_string(cstr str, u32 size, bool uniform_spacing = true) {
    for (u32 i = 0; i < size; ++i) {
        char c = str[i];
        if (c == '\r') {
            ctk_print("\\r");
            continue;
        }
        else if (c == '\0') {
            ctk_print("\\0");
        }
        else if (c == '\n') {
            ctk_print_line("\\n");
        }
        else {
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

template<typename Type>
static void ctk_print_array(CTK_Array<Type> *array, cstr title = NULL, u32 tabs = 0) {
    ctk_print(tabs, "%s:\n", title ? title : "array");
    ctk_print(tabs + 1, "data:       %p\n", array->data);
    ctk_print(tabs + 1, "size:       %u\n", array->size);
    ctk_print(tabs + 1, "chunk_size: %u\n", array->chunk_size);
    ctk_print(tabs + 1, "count:      %u\n", array->count);
    ctk_print(tabs + 1, "allocator:  %p\n", array->allocator);
}

////////////////////////////////////////////////////////////
/// Pool
////////////////////////////////////////////////////////////
template<typename Type>
static CTK_Pool<Type> ctk_create_pool(u32 chunk_size) {
    return { NULL, chunk_size };
}

template<typename Type>
static Type *ctk_push(CTK_Pool<Type> *pool) {
    if (!pool->next) {
        pool->next = ctk_alloc<_CTK_PoolNode<Type>>(pool->chunk_size);

        // Point each node except last to next node in allocated chunk.
        for (u32 i = 0; i < pool->chunk_size - 1; ++i)
            pool->next->next = pool->next + 1;
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

// Create Empty Array Overloads
template<typename Type>
static CTK_Array<Type> *ctk_create_array(u32 init_size, u32 chunk_size, CTK_Allocator *allocator) {
    CTK_ASSERT(init_size > 0);

    if (chunk_size > 0 && allocator->realloc == NULL) {
        ctk_warning("chunk_size passed to ctk_create_array() is > 0, but allocator passed does not support reallocation "
                    "so chunk_size is irrelevant");
    }

    u32 total_init_size = chunk_size ? ctk_total_chunk_size(init_size, chunk_size) : init_size;

    auto array = ctk_alloc<CTK_Array<Type>>(allocator, 1);
    array->data = ctk_alloc<Type>(allocator, total_init_size);
    array->size = total_init_size;
    array->chunk_size = chunk_size;
    array->allocator = allocator;

    return array;
}

template<typename Type>
static CTK_Array<Type> *ctk_create_array(u32 init_size) {
    return ctk_create_array<Type>(init_size, 0, &CTK_SYSTEM_ALLOCATOR);
}

template<typename Type>
static CTK_Array<Type> *ctk_create_array(u32 init_size, u32 chunk_size) {
    return ctk_create_array<Type>(init_size, chunk_size, &CTK_SYSTEM_ALLOCATOR);
}

template<typename Type>
static CTK_Array<Type> *ctk_create_array(u32 size, CTK_Allocator *allocator) {
    return ctk_create_array<Type>(size, 0, allocator);
}

// Create Full Array Overloads
template<typename Type>
static CTK_Array<Type> *ctk_create_array_full(u32 init_size, u32 chunk_size, CTK_Allocator *allocator) {
    auto array = ctk_create_array<Type>(init_size, chunk_size, allocator);
    array->count = array->size;
    return array;
}

template<typename Type>
static CTK_Array<Type> *ctk_create_array_full(u32 init_size) {
    return ctk_create_array_full<Type>(init_size, 0, &CTK_SYSTEM_ALLOCATOR);
}

template<typename Type>
static CTK_Array<Type> *ctk_create_array_full(u32 init_size, u32 chunk_size) {
    return ctk_create_array_full<Type>(init_size, chunk_size, &CTK_SYSTEM_ALLOCATOR);
}

template<typename Type>
static CTK_Array<Type> *ctk_create_array_full(u32 size, CTK_Allocator *allocator) {
    return ctk_create_array_full<Type>(size, 0, allocator);
}

// Interface
template<typename Type>
static void ctk_resize(CTK_Array<Type> *array, u32 new_size) {
    CTK_ASSERT(new_size > 0);

    if (array->chunk_size == 0)
        CTK_FATAL("can't resize CTK_Array where chunk_size == 0");

    u32 total_realloc_size = ctk_total_chunk_size(new_size, array->chunk_size);
    if (array->allocator->realloc) {
        array->data = ctk_realloc<Type>(array->allocator, array->data, array->size, total_realloc_size);
        array->size = total_realloc_size;
    }
    else {
        CTK_FATAL("can't resize CTK_Array; allocator does not provide realloc function");
    }
}

template<typename Type>
static void ctk_free(CTK_Array<Type> *array) {
    if (array->allocator->free) {
        ctk_free(array->allocator, array->data);
        ctk_free(array->allocator, array);
    }
    else {
        CTK_FATAL("can't free CTK_Array; allocator does not provide free function");
    }
}

template<typename Type>
static void _ctk_resize_if_needed(CTK_Array<Type> *array, u32 elem_count) {
    if (array->count + elem_count <= array->size)
        return;

    if (array->allocator->realloc == NULL)
        CTK_FATAL("can't resize CTK_Array; allocator does not provide realloc function");

    if (array->chunk_size == 0)
        CTK_FATAL("can't resize CTK_Array where chunk_size == 0");

    ctk_resize(array, array->count + elem_count);
}

template<typename Type>
static Type *ctk_push(CTK_Array<Type> *array, Type elem) {
    if (array->size == 0)
        CTK_FATAL("pushing to unallocated array (size=0)");

    _ctk_resize_if_needed(array, 1);
    Type *new_elem = array->data + array->count++;
    *new_elem = elem;
    return new_elem;
}

template<typename Type>
static Type *ctk_push(CTK_Array<Type> *array) {
    return ctk_push(array, {});
}

template<typename Type>
static void ctk_concat(CTK_Array<Type> *array, Type const *elems, u32 elem_count) {
    CTK_ASSERT(elems != NULL && elem_count > 0)
    if (array->size == 0)
        CTK_FATAL("pushing to unallocated array (size=0)");

    _ctk_resize_if_needed(array, elem_count);
    memcpy(array->data + array->count, elems, elem_count * sizeof(Type));
    array->count += elem_count;
}

template<typename Type>
static void ctk_concat(CTK_Array<Type> *array, CTK_Array<Type> *other) {
    ctk_concat(array, other->data, other->count);
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
static void ctk_concat(CTK_String *string, CTK_CharRange char_range);

// Fixed Size Overloads
static CTK_String *ctk_create_string(u32 init_size, u32 chunk_size, CTK_Allocator *allocator) {
    return ctk_create_array<char>(init_size, chunk_size, allocator);
}

static CTK_String *ctk_create_string(u32 init_size) {
    return ctk_create_string(init_size, 0, &CTK_SYSTEM_ALLOCATOR);
}

static CTK_String *ctk_create_string(u32 init_size, u32 chunk_size) {
    return ctk_create_string(init_size, chunk_size, &CTK_SYSTEM_ALLOCATOR);
}

static CTK_String *ctk_create_string(u32 init_size, CTK_Allocator *allocator) {
    return ctk_create_string(init_size, 0, allocator);
}

// CTK_CharRange Overloads
static CTK_String *ctk_create_string(CTK_CharRange char_range, u32 chunk_size, CTK_Allocator *allocator) {
    CTK_ASSERT(char_range.data != NULL && char_range.size > 0);

    // Include space for null-terminator.
    CTK_String *string = ctk_create_string(char_range.size + 1, chunk_size, allocator);

    ctk_concat(string, char_range);
    return string;
}

static CTK_String *ctk_create_string(CTK_CharRange char_range) {
    return ctk_create_string(char_range, 0, &CTK_SYSTEM_ALLOCATOR);
}

static CTK_String *ctk_create_string(CTK_CharRange char_range, u32 chunk_size) {
    return ctk_create_string(char_range, chunk_size, &CTK_SYSTEM_ALLOCATOR);
}

static CTK_String *ctk_create_string(CTK_CharRange char_range, CTK_Allocator *allocator) {
    return ctk_create_string(char_range, 0, allocator);
}

// C-String Overloads
static CTK_String *ctk_create_string(cstr str, u32 chunk_size, CTK_Allocator *allocator) {
    CTK_ASSERT(str != NULL);
    return ctk_create_string({ str, strlen(str) }, chunk_size, allocator);
}

static CTK_String *ctk_create_string(cstr str) {
    return ctk_create_string(str, 0, &CTK_SYSTEM_ALLOCATOR);
}

static CTK_String *ctk_create_string(cstr str, u32 chunk_size) {
    return ctk_create_string(str, chunk_size, &CTK_SYSTEM_ALLOCATOR);
}

static CTK_String *ctk_create_string(cstr str, CTK_Allocator *allocator) {
    return ctk_create_string(str, 0, allocator);
}

// Interface
static void _ctk_resize_string_if_needed(CTK_String *string, u32 new_char_count) {
    _ctk_resize_if_needed(string, new_char_count + 1); // Include room for null-terminator.
}

template<typename ...Args>
static void ctk_print(CTK_String *str, cstr msg, Args... args) {
    u32 msg_size = snprintf(NULL, 0, msg, args...);
    _ctk_resize_string_if_needed(str, msg_size); // Will include room for printed message and null-terminator.
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
    if (string->size == 0)
        CTK_FATAL("pushing to unallocated string (size=0)");

    _ctk_resize_string_if_needed(string, 1);
    char *new_char = string->data + string->count++;
    *new_char = c;
    return new_char;
}

static char *ctk_push(CTK_String *string) {
    return ctk_push(string, (char)0);
}

static void ctk_concat(CTK_String *string, CTK_CharRange char_range) {
    CTK_ASSERT(char_range.data != NULL && char_range.size > 0);
    if (string->size == 0)
        CTK_FATAL("pushing to unallocated string (size=0)");

    _ctk_resize_string_if_needed(string, char_range.size);
    memcpy(string->data + string->count, char_range.data, char_range.size);
    string->count += char_range.size;
}

static void ctk_concat(CTK_String *string, cstr other) {
    CTK_ASSERT(other != NULL);
    ctk_concat(string, { other, strlen(other) });
}

static void ctk_concat(CTK_String *string, CTK_String *other) {
    CTK_ASSERT(other != NULL);
    ctk_concat(string, { other->data, other->count });
}

static bool ctk_strings_match(cstr a, CTK_String *b) {
    return ctk_strings_match(a, strlen(a), b->data, b->count);
}

static bool ctk_strings_match(cstr a, CTK_CharRange b) {
    return ctk_strings_match(a, strlen(a), b.data, b.size);
}

static bool ctk_strings_match(CTK_String *a, cstr b) {
    return ctk_strings_match(a->data, a->count, b, strlen(b));
}

static bool ctk_strings_match(CTK_String *a, CTK_String *b) {
    return ctk_strings_match(a->data, a->count, b->data, b->count);
}

static bool ctk_strings_match(CTK_String *a, CTK_CharRange b) {
    return ctk_strings_match(a->data, a->count, b.data, b.size);
}

static bool ctk_strings_match(CTK_CharRange a, cstr b) {
    return ctk_strings_match(a.data, a.size, b, strlen(b));
}

static bool ctk_strings_match(CTK_CharRange a, CTK_String *b) {
    return ctk_strings_match(a.data, a.size, b->data, b->count);
}

static bool ctk_strings_match(CTK_CharRange a, CTK_CharRange b) {
    return ctk_strings_match(a.data, a.size, b.data, b.size);
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
    if (ctk_strings_match(s, "true"))
        return true;

    if (ctk_strings_match(s, "false"))
        return false;

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
    if (array->count + 1 > ctk_size(array))
        CTK_FATAL("static array (size=%u count=%u) cannot hold any more elements", ctk_size(array), array->count);

    Type *new_elem = array->data + array->count++;
    *new_elem = elem;
    return new_elem;
}

template<typename Type, u32 size>
static Type *ctk_push(CTK_StaticArray<Type, size> *array) {
    return ctk_push(array, {});
}

template<typename Type, u32 size>
static void ctk_push(CTK_StaticArray<Type, size> *array, Type *elems, u32 elem_count) {
    if (elem_count == 0)
        return;

    if (array->count + elem_count > ctk_size(array)) {
        CTK_FATAL("static array (size=%u count=%u) cannot hold %u more elements", ctk_size(array), array->count,
                  elem_count);
    }

    memcpy(array->data + array->count, elems, elem_count * sizeof(Type));
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
Type &CTK_StaticArray<Type, size>::operator[](u32 i) {
    CTK_ASSERT(i < size);
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

template<typename Type, u32 size, u32 key_size>
static Type *ctk_push(CTK_StaticMap<Type, size, key_size> *map, cstr key) {
    return ctk_push(map, key, {});
}

template<typename Type, u32 size, u32 key_size>
static Type *ctk_find(CTK_StaticMap<Type, size, key_size> *map, cstr key) {
    for (u32 i = 0; i < map->count; ++i)
        if (ctk_strings_match(key, (cstr)(map->keys + i)))
            return map->values + i;

    return NULL;
}

template<typename Type, u32 size, u32 key_size>
static u32 ctk_find_index(CTK_StaticMap<Type, size, key_size> *map, cstr key) {
    for (u32 i = 0; i < map->count; ++i)
        if (ctk_strings_match(key, (cstr)(map->keys + i)))
            return i;

    return CTK_U32_MAX;
}

template<typename Type, u32 size, u32 key_size>
static Type *ctk_get(CTK_StaticMap<Type, size, key_size> *map, cstr key) {
    Type *val = ctk_find(map, key);
    if (val == NULL)
        CTK_FATAL("failed to get entry for key \"%s\" in static map", key);
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
