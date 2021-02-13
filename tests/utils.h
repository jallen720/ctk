#pragma once

#include "ctk/ctk.h"
#include "ctk/ctkd.h"
#include "ctk/containers.h"

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
static void write(Surface *s, u32 x, u32 y, cstr msg, Args... args) {
    u32 start = (y * s->width) + x;
    u32 print_count = snprintf(NULL, 0, msg, args...);
    if (start + print_count > s->size)
        CTK_FATAL("can't write to surface at %u, %u; not enough space to write %u characters", x, y, print_count);
    snprintf(s->data + start, s->size - start, msg, args...);
    s->data[start + print_count] = ' ';
}

static u32 idx_1d(u32 x, u32 y, u32 width) {
    return (y * width) + x;
}

static void print(Surface *s) {
    static u32 const HORIZONTAL_LINE_MAX_WIDTH = 512;
    static char horizontal_line[HORIZONTAL_LINE_MAX_WIDTH] = {};
    CTK_ASSERT(s->width <= HORIZONTAL_LINE_MAX_WIDTH - 2);
    memset(horizontal_line, 196, s->width);
    ctk_print("%c", 218);
    ctk_print("%.*s", s->width, horizontal_line);
    ctk_print("%c", 191);
    ctk_print("\n");
    for (u32 i = 0; i < s->height; ++i) {
        ctk_print("%c", 179);
        ctk_print("%.*s", s->width, s->data + (i * s->width));
        ctk_print("%c", 179);
        ctk_print("\n");
    }
    ctk_print("%c", 192);
    ctk_print("%.*s", s->width, horizontal_line);
    ctk_print("%c", 217);
    ctk_print("\n");
}
