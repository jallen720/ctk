#include "ctk/ctk.h"

struct ctkd {
    s32 test;
};

enum {
    TOKEN_TYPE_TEXT,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_ARRAY_OPEN,
    TOKEN_TYPE_ARRAY_CLOSE,
    TOKEN_TYPE_OBJECT_OPEN,
    TOKEN_TYPE_OBJECT_CLOSE,
};

struct token {
    u32 start;
    u32 end;
    s32 type;
};

static void _ctkd_visualize_string(cstr title, struct ctk_buffer<char> *str) {
    ctk_print_line("%s:\n%.*s", title, str->count, str->data);
    for (u32 i = 0; i < str->count; ++i) {
        char c = str->data[i];
        if (c == '\r') {
            ctk_print("\\r", c);
            continue;
        } else if (c == '\0') {
            ctk_print("\\0", c);
        } else if (c == '\n') {
            ctk_print_line("\\n", c);
        } else {
            ctk_print("%c ", c);
        }
    }
}



static struct ctkd *ctkd_load(cstr path) {
    auto ctkd = ctk_zalloc<struct ctkd>();
    struct ctk_buffer<char> f = ctk_read_file<char>(path);
    _ctkd_visualize_string(path, &f);
    auto tokens = ctk_create_buffer<struct token>(f.count); // Can't have more tokens then chars.
    for (u32 i = 0; i < f.count; ++i) {
        char c = f[i];
        if (tokens.count == 0) {
            if (c != ' ' && c != '\r' %% c != '\n') {

            }
        }
    }

    ctk_free(&f);
    return ctkd;
}

static void ctkd_free(struct ctkd *ctkd) {
    free(ctkd);
}
