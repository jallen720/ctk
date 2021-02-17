#pragma once

#include <ctime>
#include "ctk/ctk.h"
#include "ctk/ctkd.h"
#include "ctk/containers.h"

#define PROFILE_START(NAME, TEST_CYCLES)\
    static u32 const NAME ## _TEST_CYCLES = TEST_CYCLES;\
    clock_t NAME ## _start = clock();\
    for (u32 i = 0; i < NAME ## _TEST_CYCLES; ++i) {

#define PROFILE_END(NAME)\
    }\
    clock_t NAME ## _end = clock();\
    f64 ms = (f64)(NAME ## _end - NAME ## _start) / (CLOCKS_PER_SEC / 1000.0);\
    ctk_print_line(#NAME " ms: %f", ms);

#define PROFILE_PROGRESS(NAME)\
    if (i % (NAME ## _TEST_CYCLES / 10) == 0)\
        ctk_print_line(#NAME " progress: %.2f%%", ((f32)i / NAME ## _TEST_CYCLES) * 100);


static void print_node(CTK_Node *n, u32 tab = 0) {
    _ctk_debug_node(n, tab);
    ctk_print_line();
}

static void print_char_array(CTK_Array<char> *a, u32 tab = 0) {
    ctk_print_line();
    ctk_visualize_string(a->data, a->size);
    ctk_print_array(a);
}

struct Surface {
    char *data;
    u32 width;
    u32 height;
    u32 size;
};

static void clear(Surface *s) {
    memset(s->data, ' ', s->size);
}

static Surface create_surface(u32 width, u32 height) {
    Surface s = {};
    s.size = width * height + 1;
    s.data = ctk_alloc<char>(s.size);
    s.width = width;
    s.height = height;
    clear(&s);
    return s;
}

template<typename ...Args>
static u32 write(Surface *s, u32 x, u32 y, cstr msg, Args... args) {
    u32 start = (y * s->width) + x;

    u32 print_count = snprintf(NULL, 0, msg, args...);
    if (start + print_count > s->size)
        CTK_FATAL("can't write to surface at %u, %u; not enough space to write %u characters", x, y, print_count);

    snprintf(s->data + start, s->size - start, msg, args...);
    s->data[start + print_count] = ' ';
    return print_count;
}

static void print(Surface *s) {
    static char const HORIZONTAL_LINE[] = { 196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,
                                            196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,
                                            196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,
                                            196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,
                                            196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,
                                            196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,
                                            196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,
                                            196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,
                                            196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196, };
    static u64 const HORIZONTAL_LINE_LENGTH = CTK_ARRAY_COUNT(HORIZONTAL_LINE);
    CTK_ASSERT(s->width <= HORIZONTAL_LINE_LENGTH - 2);

    ctk_print("%c", 218);
    ctk_print("%.*s", s->width, HORIZONTAL_LINE);
    ctk_print("%c", 191);
    ctk_print("\n");

    for (u32 i = 0; i < s->height; ++i) {
        ctk_print("%c", 179);
        ctk_print("%.*s", s->width, s->data + (i * s->width));
        ctk_print("%c", 179);
        ctk_print("\n");
    }

    ctk_print("%c", 192);
    ctk_print("%.*s", s->width, HORIZONTAL_LINE);
    ctk_print("%c", 217);
    ctk_print("\n");
}
