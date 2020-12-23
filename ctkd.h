#include "ctk/ctk_2.h"

////////////////////////////////////////////////////////////
/// DATA STRUCT
////////////////////////////////////////////////////////////
enum {
    CTKD_NODE_TYPE_SCALAR,
    CTKD_NODE_TYPE_STRUCT,
    CTKD_NODE_TYPE_ARRAY,
};

struct _ctkd_arena;

struct ctkd_node {
    _ctkd_arena *arena;
    s32 type;
    cstr key;
    union {
        cstr value;
        ctk_buffer<ctkd_node> children;
    };
};

struct _ctkd_arena {
    ctk_buffer<char> chars;
    ctk_buffer<ctkd_node> nodes;
};

static cstr _ctkd_alloc_string(_ctkd_arena *a, cstr str) {
    if (str == NULL)
        return NULL;
    u32 str_size = strlen(str) + 1; // Include null-terminator.
    if (a->chars.count + str_size > a->chars.size)
        CTK_FATAL("CTKD: out of memory for characters")
    char *new_str = a->chars + a->chars.count;
    a->chars.count += str_size;
    strcpy(new_str, str);
    return new_str;
}

static ctk_buffer<ctkd_node> _ctkd_alloc_child_buffer(_ctkd_arena *a, u32 size) {
    if (a->nodes.count + size > a->nodes.size)
        CTK_FATAL("CTKD: out of memory for nodes")
    ctk_buffer<ctkd_node> child_buf = {};
    child_buf.data = a->nodes + a->nodes.count;
    child_buf.size = size;
    a->nodes.count += size;
    return child_buf;
}

// static ctkd_node *ctkd_create(ctk_heap *heap) {

// }

// static ctkd_node *ctkd_create(u32 max_nodes, u32 max_chars, u32 max_root_child_count) {
//     auto arena = ctk_alloc_z<_ctkd_arena>();
//     arena->chars = ctk_create_buffer<char>(max_chars);
//     arena->nodes = ctk_create_buffer<ctkd_node>(max_nodes);
//     ctkd_node *root = ctk_push(&arena->nodes);
//     root->arena = arena;
//     root->type = CTKD_NODE_TYPE_STRUCT;
//     root->key = _ctkd_alloc_string(arena, "root");
//     root->children = _ctkd_alloc_child_buffer(arena, max_root_child_count);
//     return root;
// }

static ctkd_node *_ctkd_push_vector(ctkd_node *parent, s32 type, u32 max_child_count, cstr key = NULL) {
    CTK_ASSERT(type == CTKD_NODE_TYPE_STRUCT || type == CTKD_NODE_TYPE_ARRAY);
    ctkd_node *child = ctk_push(&parent->children);
    child->arena = parent->arena;
    child->type = type;
    child->key = _ctkd_alloc_string(parent->arena, key);
    child->children = _ctkd_alloc_child_buffer(parent->arena, max_child_count);
    return child;
}

static ctkd_node *ctkd_push_array(ctkd_node *parent, u32 max_child_count, cstr key = NULL) {
    return _ctkd_push_vector(parent, CTKD_NODE_TYPE_ARRAY, max_child_count, key);
}

static ctkd_node *ctkd_push_struct(ctkd_node *parent, u32 max_child_count, cstr key = NULL) {
    return _ctkd_push_vector(parent, CTKD_NODE_TYPE_STRUCT, max_child_count, key);
}

static void ctkd_push_string(ctkd_node *parent, cstr value, cstr key = NULL) {
    ctkd_node *child = ctk_push(&parent->children);
    child->arena = parent->arena;
    child->type = CTKD_NODE_TYPE_SCALAR;
    child->key = _ctkd_alloc_string(parent->arena, key);
    child->value = _ctkd_alloc_string(parent->arena, value);
}

static void ctkd_push_bool(ctkd_node *parent, bool v, cstr key = NULL) {
    ctkd_push_string(parent, v ? "true" : "false", key);
}

#define _CTKD_NUMBER_PUSH_DECL(TYPE, FORMAT)\
    static void ctkd_push_ ## TYPE(ctkd_node *parent, TYPE v, cstr key = NULL) {\
        char val_str[32] = {};\
        sprintf(val_str, FORMAT, v);\
        ctkd_push_string(parent, val_str, key);\
    }

_CTKD_NUMBER_PUSH_DECL(f32, "%f")
_CTKD_NUMBER_PUSH_DECL(f64, "%f")
_CTKD_NUMBER_PUSH_DECL(u32, "%u")
_CTKD_NUMBER_PUSH_DECL(u64, "%llu")
_CTKD_NUMBER_PUSH_DECL(s32, "%i")
_CTKD_NUMBER_PUSH_DECL(s64, "%lli")

// enum {
//     _CTKD_SEARCH_TERM_TYPE_KEY,
//     _CTKD_SEARCH_TERM_TYPE_IDX,
// };

// struct _ctkd_search_term {
//     s32 type;
//     union {
//         struct {
//             u32 start;
//             u32 size;
//         } key;
//         u32 idx;
//     };
// };
// static ctkd_node *ctkd_get(ctkd_node *parent, u32 idx);

// template<typename ...arg_types>
// static ctkd_node *ctkd_find(ctkd_node *parent, cstr search_str, arg_types... args) {
//     ctk_array<_ctkd_search_term, 16> search_terms = {};
//     u32 term_index = 0;
//     char search[256] = {};
//     u32 search_size = sprintf(search, search_str, args...);
//     while (term_index < search_size) {
//         if (search[term_index] == '[') {
//             ++term_index;
//             char *end;
//             _ctkd_search_term *st = ctk_push(&search_terms);
//             st->type = _CTKD_SEARCH_TERM_TYPE_IDX;
//             st->idx = strtoul(search + term_index, &end, 10);
//             term_index = end - search + 1;
//         } else {
//             _ctkd_search_term *st = ctk_push(&search_terms);
//             st->type = _CTKD_SEARCH_TERM_TYPE_KEY;
//             st->key.start = term_index;
//             for (; term_index < search_size; ++term_index) {
//                 char c = search[term_index];
//                 if (c == '.' || c == '[')
//                     break;
//             }
//             st->key.size = term_index - st->key.start;
//         }
//         if (search[term_index] == '.')
//             ++term_index;
//     }
//     // CTK_EACH(_ctkd_search_term, term, search_terms) {
//     //     if (term->type == _CTKD_SEARCH_TERM_TYPE_KEY)
//     //         ctk_print_line("<_CTKD_SEARCH_TERM_TYPE_KEY> %.*s", term->key.size, search + term->key.start);
//     //     else if (term->type == _CTKD_SEARCH_TERM_TYPE_IDX)
//     //         ctk_print_line("<_CTKD_SEARCH_TERM_TYPE_IDX> %u", term->idx);
//     //     else
//     //         ctk_print_line("<unknown type>");
//     // }
//     ctkd_node *base = parent;
//     CTK_EACH(_ctkd_search_term, term, search_terms) {
//         if (term->type == _CTKD_SEARCH_TERM_TYPE_IDX) {
//             base = ctkd_get(base, term->idx);
//         } else {
//             CTK_EACH(ctkd_node, child, base->children) {
//                 if (ctk_strings_match(key, child->key)) {
//                     base = child;
//                     break;
//                 }
//             }
//         }
//     }
//     return base;
// }

static ctkd_node *ctkd_get(ctkd_node *parent, cstr key) {
    CTK_EACH(ctkd_node, child, parent->children)
        if (ctk_strings_match(child->key, key))
            return child;
    CTK_FATAL("failed to find child with key \"%s\" in parent \"%s\"", key, parent->key)
}

static ctkd_node *ctkd_get(ctkd_node *parent, u32 idx) {
    CTK_ASSERT(idx < parent->children.count)
    return parent->children + idx;
}

static f32 ctkd_f32(ctkd_node *parent, cstr key) {
    return ctk_f32(ctkd_get(parent, key)->value);
}

static f32 ctkd_f32(ctkd_node *parent, u32 idx) {
    return ctk_f32(ctkd_get(parent, idx)->value);
}

static f64 ctkd_f64(ctkd_node *parent, cstr key) {
    return ctk_f64(ctkd_get(parent, key)->value);
}

static f64 ctkd_f64(ctkd_node *parent, u32 idx) {
    return ctk_f64(ctkd_get(parent, idx)->value);
}

static s32 ctkd_s32(ctkd_node *parent, cstr key) {
    return ctk_s32(ctkd_get(parent, key)->value);
}

static s32 ctkd_s32(ctkd_node *parent, u32 idx) {
    return ctk_s32(ctkd_get(parent, idx)->value);
}

static s64 ctkd_s64(ctkd_node *parent, cstr key) {
    return ctk_s64(ctkd_get(parent, key)->value);
}

static s64 ctkd_s64(ctkd_node *parent, u32 idx) {
    return ctk_s64(ctkd_get(parent, idx)->value);
}

static u32 ctkd_u32(ctkd_node *parent, cstr key) {
    return ctk_u32(ctkd_get(parent, key)->value);
}

static u32 ctkd_u32(ctkd_node *parent, u32 idx) {
    return ctk_u32(ctkd_get(parent, idx)->value);
}

static u64 ctkd_u64(ctkd_node *parent, cstr key) {
    return ctk_u64(ctkd_get(parent, key)->value);
}

static u64 ctkd_u64(ctkd_node *parent, u32 idx) {
    return ctk_u64(ctkd_get(parent, idx)->value);
}

static bool ctkd_bool(ctkd_node *parent, cstr key) {
    return ctk_bool(ctkd_get(parent, key)->value);
}

static bool ctkd_bool(ctkd_node *parent, u32 idx) {
    return ctk_bool(ctkd_get(parent, idx)->value);
}

/// DEBUG
static void _ctkd_print_arena(_ctkd_arena *a, u32 tab = 0) {
    ctk_print_line(tab + 0, "arena:");
    ctk_print_line(tab + 1, "chars: %u/%u %.2f%%", a->chars.count, a->chars.size, (a->chars.count / (f32)a->chars.size) * 100);
    ctk_print_line(tab + 1, "nodes: %u/%u %.2f%%", a->nodes.count, a->nodes.size, (a->nodes.count / (f32)a->nodes.size) * 100);
}

static void _ctkd_print_node(ctkd_node *n, u32 tab = 0);

static void _ctkd_debug_children(ctkd_node *n, u32 parent_tab) {
    if (n->children.count > 0) {
        ctk_print_line();
        for (u32 i = 0; i < n->children.count; ++i)
            _ctkd_print_node(ctkd_get(n, i), parent_tab + 1);
        ctk_print_tabs(parent_tab);
    }
}

static void _ctkd_print_node(ctkd_node *n, u32 tab) {
    ctk_print_tabs(tab);
    if (n->key != NULL)
        ctk_print("%s ", n->key);
    else
        ctk_print("<empty> ");

    if (n->type == CTKD_NODE_TYPE_STRUCT) {
        ctk_print("(child_count: %u) {", n->children.count);
        _ctkd_debug_children(n, tab);
        ctk_print_line("}");
    } else if (n->type == CTKD_NODE_TYPE_ARRAY) {
        ctk_print("(child_count: %u) [", n->children.count);
        _ctkd_debug_children(n, tab);
        ctk_print_line("]");
    } else {
        ctk_print_line("%s", n->value);
    }
}

static void _ctkd_debug_node(ctkd_node *n, u32 tab = 0) {
    ctk_print_line("============================================================================");
    _ctkd_print_node(n, tab);
    _ctkd_print_arena(n->arena, tab);
    ctk_print_line("============================================================================\n");
}

////////////////////////////////////////////////////////////
/// PARSER
////////////////////////////////////////////////////////////
enum {
    _CTKD_TOKEN_TYPE_TEXT,
    _CTKD_TOKEN_TYPE_STRING,
    _CTKD_TOKEN_TYPE_ARRAY_OPEN,
    _CTKD_TOKEN_TYPE_ARRAY_CLOSE,
    _CTKD_TOKEN_TYPE_STRUCT_OPEN,
    _CTKD_TOKEN_TYPE_STRUCT_CLOSE,
    _CTKD_TOKEN_TYPE_UNKNOWN,
};

struct _ctkd_token {
    u32 start;
    u32 size;
    s32 type;
};

static void _ctkd_visualize_string(cstr title, ctk_buffer<char> *str, bool uniform_spacing = true) {
    ctk_print_line("%s:\n%.*s", title, str->count, str->data);
    for (u32 i = 0; i < str->count; ++i) {
        char c = str->data[i];
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
}

static char const SYMBOLS[] = { '[', ']', '{', '}' };
static char const SKIPPABLES[] = { ' ', '\0', '\r', '\n' };
static char const ESCAPABLE[] = { '"', '\\' };

#define _CTKD_CHAR_LOOKUP(ARR)\
    for (u32 i = 0; i < CTK_ARRAY_COUNT(ARR); ++i)\
        if (ARR[i] == c)\
            return true;\
    return false;

static bool is_symbol(char c) {
    _CTKD_CHAR_LOOKUP(SYMBOLS)
}

static bool is_skippable(char c) {
    _CTKD_CHAR_LOOKUP(SKIPPABLES)
}

static bool is_escapable(char c) {
    _CTKD_CHAR_LOOKUP(ESCAPABLE)
}

// static u32 _ctkd_child_node_count(u32 ) {

// }

static ctkd_node *ctkd_create() {

}

static ctkd_node *ctkd_read(cstr path) {
    ctk_buffer<char> file_str = ctk_read_file<char>(path);
    _ctkd_visualize_string(path, &file_str, false);
    auto tokens = ctk_create_buffer<_ctkd_token>(file_str.count); // Can't have more tokens than chars.
    for (u32 base_idx = 0; base_idx < file_str.count;) {
        char c = file_str[base_idx];
        if (is_skippable(c)) {
            // SKIPPABLE
            ++base_idx;
            continue;
        } else if (is_symbol(c)) {
            // SYMBOL
            _ctkd_token *t = ctk_push(&tokens);
            t->start = base_idx;
            t->size = 1;
            t->type = c == '[' ? _CTKD_TOKEN_TYPE_ARRAY_OPEN :
                      c == ']' ? _CTKD_TOKEN_TYPE_ARRAY_CLOSE :
                      c == '{' ? _CTKD_TOKEN_TYPE_STRUCT_OPEN :
                      c == '}' ? _CTKD_TOKEN_TYPE_STRUCT_CLOSE :
                      _CTKD_TOKEN_TYPE_UNKNOWN;
            if (t->type == _CTKD_TOKEN_TYPE_UNKNOWN)
                CTK_FATAL("unknown symbol type for token char: %c", c)
            ++base_idx;
        } else if (c == '"') {
            // STRING
            _ctkd_token *t = ctk_push(&tokens);
            t->type = _CTKD_TOKEN_TYPE_STRING;
            t->start = base_idx;
            u32 str_char_idx = base_idx + 1;
            bool next_char_escaped = false;
            for (; str_char_idx < file_str.count; ++str_char_idx) {
                char text_char = file_str[str_char_idx];
                if (next_char_escaped) {
                    if (!is_escapable(text_char))
                        CTK_FATAL("unsupported escape character: \\%c", text_char)
                    next_char_escaped = false;
                } else if (text_char == '\\') {
                    next_char_escaped = true;
                } else if (text_char == '"') {
                    ++str_char_idx;
                    break;
                }
            }
            t->size = str_char_idx - t->start;
            if (str_char_idx >= file_str.count)
                CTK_FATAL("reached end of file while parsing string: %.*s", t->size, file_str + t->start)
            base_idx = str_char_idx;
        } else {
            // TEXT
            _ctkd_token *t = ctk_push(&tokens);
            t->type = _CTKD_TOKEN_TYPE_TEXT;
            t->start = base_idx;
            u32 text_char_idx = base_idx + 1;
            for (; text_char_idx < file_str.count; ++text_char_idx) {
                char text_char = file_str[text_char_idx];
                if (is_skippable(text_char) || is_symbol(text_char) || text_char == '"')
                    break;
            }
            t->size = text_char_idx - t->start;
            base_idx = text_char_idx;
        }
    }

    CTK_EACH(_ctkd_token, t, tokens) {
        cstr type_name = t->type == _CTKD_TOKEN_TYPE_TEXT ?           "          TEXT" :
                         t->type == _CTKD_TOKEN_TYPE_STRING ?         "        STRING" :
                         t->type == _CTKD_TOKEN_TYPE_ARRAY_OPEN ?     "    ARRAY_OPEN" :
                         t->type == _CTKD_TOKEN_TYPE_ARRAY_CLOSE ?    "   ARRAY_CLOSE" :
                         t->type == _CTKD_TOKEN_TYPE_STRUCT_OPEN ?    "   STRUCT_OPEN" :
                         t->type == _CTKD_TOKEN_TYPE_STRUCT_CLOSE ?   "  STRUCT_CLOSE" :
                                                                     "       UNKNOWN";
        ctk_print_line("%s |%.*s|", type_name, t->size, file_str + t->start);
    }

    // // Count nodes and chars for allocating memory.
    // u32 total_node_count = 0;
    // u32 total_char_count = 0;
    // u32 root_child_count = 0;
    // u32 token_idx = 0;
    // u32 nest_level = 0;
    // s32 parent_type = CTKD_NODE_TYPE_STRUCT;
    // while (token_idx < tokens.count) {
    //     _ctkd_token *curr = tokens + token_idx;
    //     ++total_node_count;
    //     if (nest_level == 0)
    //         ++root_child_count;
    //     if (parent_type == CTKD_NODE_TYPE_STRUCT) {
    //         ++token_idx;
    //         if (curr->type == _CTKD_TOKEN_TYPE_TEXT) {

    //         } else if (curr->type == _CTKD_TOKEN_TYPE_ARRAY_OPEN) {

    //         } else if (curr->type == _CTKD_TOKEN_TYPE_STRUCT_OPEN) {

    //         }
    //     }
    // }

    // ctkd_node *root = ctkd_create(total_node_count, total_char_count, root_child_count);
    // _ctkd_debug_node(root);

    ctk_free(&file_str);
    return NULL;//root;
}
