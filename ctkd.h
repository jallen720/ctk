#include "ctk/ctk_2.h"

////////////////////////////////////////////////////////////
/// Data
////////////////////////////////////////////////////////////
enum {
    _CTK_NODE_TYPE_SCALAR,
    _CTK_NODE_TYPE_STRUCT,
    _CTK_NODE_TYPE_ARRAY,
};

struct CTK_Node {
    CTK_Node *next;
    s32 type;
    CTK_String key;

    union {
        CTK_String value;

        struct {
            CTK_Node *list;
            u32 count;
        } children;
    };
};

static CTK_Pool<CTK_Node> *_CTK_NODE_POOL;

////////////////////////////////////////////////////////////
/// Debugging
////////////////////////////////////////////////////////////
static void _ctk_debug_node(CTK_Node *n, u32 tabs = 0) {
    ctk_print_line(tabs, "node %p:", n);
    ctk_print_line(tabs + 1, "next:  %p", n->next);
    ctk_print_line(tabs + 1, "type:  %s",
                   n->type == _CTK_NODE_TYPE_SCALAR ? "_CTK_NODE_TYPE_SCALAR" :
                   n->type == _CTK_NODE_TYPE_STRUCT ? "_CTK_NODE_TYPE_STRUCT" :
                   n->type == _CTK_NODE_TYPE_ARRAY ? "_CTK_NODE_TYPE_ARRAY" :
                   "unknown");
    ctk_print_line(tabs + 1, "key:   \"%s\"", n->key.data);

    if (n->type == _CTK_NODE_TYPE_SCALAR) {
        ctk_print_line(tabs + 1, "value: \"%s\"", n->value.data);
    } else {
        ctk_print_line(tabs + 1, "children:");
        CTK_Node *child = n->children.list;

        while (child) {
            _ctk_debug_node(child, tabs + 2);
            child = child->next;
        }
    }
}

static void ctk_print_node(CTK_Node *n, u32 tabs = 0) {
    _ctk_print_tabs(tabs);

    if (n->key.data)
        ctk_print("%s: ", n->key.data);

    if (n->type == _CTK_NODE_TYPE_SCALAR) {
        ctk_print("%s\n", n->value);
    } else {
        ctk_print("%s", n->type == _CTK_NODE_TYPE_ARRAY ? "[" : "{");

        if (n->children.count > 0) {
            ctk_print("\n");
            CTK_Node *child = n->children.list;

            while (child) {
                ctk_print_node(child, tabs + 1);
                child = child->next;
            }

            _ctk_print_tabs(tabs);
        }

        ctk_print("%s\n", n->type == _CTK_NODE_TYPE_ARRAY ? "]" : "}");
    }
}

static void ctk_print_node(CTK_String *buf, CTK_Node *n, u32 tabs = 0);

static void ctk_print_node_children(CTK_String *buf, CTK_Node *n, u32 tabs = 0) {
    CTK_Node *child = n->children.list;

    while (child) {
        ctk_print_node(buf, child, tabs);
        child = child->next;
    }
}

static void ctk_print_node(CTK_String *buf, CTK_Node *n, u32 tabs) {
    _ctk_print_tabs(buf, tabs);

    if (n->key.data)
        ctk_print(buf, "%s: ", n->key.data);

    if (n->type == _CTK_NODE_TYPE_SCALAR) {
        ctk_print(buf, "%s\n", n->value.data);
    } else {
        ctk_print(buf, "%s", n->type == _CTK_NODE_TYPE_ARRAY ? "[" : "{");

        if (n->children.count > 0) {
            ctk_print(buf, "\n");
            ctk_print_node_children(buf, n, tabs + 1);
            _ctk_print_tabs(buf, tabs);
        }

        ctk_print(buf, "%s\n", n->type == _CTK_NODE_TYPE_ARRAY ? "]" : "}");
    }
}

////////////////////////////////////////////////////////////
/// Interface
////////////////////////////////////////////////////////////
static CTK_Node *ctk_create_root_node() {
    if (!_CTK_NODE_POOL) {
        _CTK_NODE_POOL = ctk_alloc<CTK_Pool<CTK_Node>>(1);
        *_CTK_NODE_POOL = ctk_create_pool<CTK_Node>(64);
    }

    CTK_Node *root = ctk_push(_CTK_NODE_POOL);
    root->type = _CTK_NODE_TYPE_STRUCT;
    return root;
}

static CTK_Node *_ctk_push_child(CTK_Node *parent) {
    CTK_ASSERT(parent->type != _CTK_NODE_TYPE_SCALAR); // Can't push children to scalar.
    CTK_Node **end_child_ptr = &parent->children.list;

    while (*end_child_ptr)
        end_child_ptr = &(*end_child_ptr)->next;

    CTK_Node *new_child = ctk_push(_CTK_NODE_POOL);
    *end_child_ptr = new_child;
    ++parent->children.count;
    new_child->next = NULL;
    return new_child;
}

static CTK_Node *_ctk_push_scalar(CTK_Node *parent, cstr value) {
    CTK_Node *n = _ctk_push_child(parent);
    n->type = _CTK_NODE_TYPE_SCALAR;
    n->value = value ? ctk_create_string(value, CTK_CACHE_LINE) : ctk_create_string(CTK_CACHE_LINE, CTK_CACHE_LINE);
    return n;
}

static CTK_Node *ctk_push_string(CTK_Node *parent, cstr key, cstr value) {
    CTK_ASSERT(key);
    CTK_ASSERT(parent->type == _CTK_NODE_TYPE_STRUCT);
    CTK_Node *n = _ctk_push_scalar(parent, value);
    n->key = ctk_create_string(key, CTK_CACHE_LINE);
    return n;
}

static CTK_Node *ctk_push_string(CTK_Node *parent, cstr value) {
    CTK_ASSERT(parent->type == _CTK_NODE_TYPE_ARRAY);
    return _ctk_push_scalar(parent, value);
}

#define _CTK_NUMBER_PUSH_DECL(TYPE, FORMAT)\
    static CTK_Node *ctk_push_ ## TYPE(CTK_Node *parent, cstr key, TYPE value) {\
        CTK_Node *string = ctk_push_string(parent, key, NULL);\
        ctk_print(&string->value, FORMAT, value);\
        return string;\
    }\
    \
    static CTK_Node *ctk_push_ ## TYPE(CTK_Node *parent, TYPE value) {\
        CTK_Node *string = ctk_push_string(parent, NULL);\
        ctk_print(&string->value, FORMAT, value);\
        return string;\
    }

_CTK_NUMBER_PUSH_DECL(f32, "%f")
_CTK_NUMBER_PUSH_DECL(f64, "%f")
_CTK_NUMBER_PUSH_DECL(u32, "%u")
_CTK_NUMBER_PUSH_DECL(u64, "%llu")
_CTK_NUMBER_PUSH_DECL(s32, "%i")
_CTK_NUMBER_PUSH_DECL(s64, "%lli")

static CTK_Node *ctk_push_array(CTK_Node *parent) {
    CTK_ASSERT(parent->type == _CTK_NODE_TYPE_ARRAY)
    CTK_Node *n = _ctk_push_child(parent);
    n->type = _CTK_NODE_TYPE_ARRAY;
    return n;
}

static CTK_Node *ctk_push_array(CTK_Node *parent, cstr key) {
    CTK_ASSERT(key)
    CTK_ASSERT(parent->type == _CTK_NODE_TYPE_STRUCT);
    CTK_Node *n = _ctk_push_child(parent);
    n->type = _CTK_NODE_TYPE_ARRAY;
    n->key = ctk_create_string(key);
    return n;
}

static CTK_Node *ctk_push_struct(CTK_Node *parent) {
    CTK_ASSERT(parent->type == _CTK_NODE_TYPE_ARRAY)
    CTK_Node *n = _ctk_push_child(parent);
    n->type = _CTK_NODE_TYPE_STRUCT;
    return n;
}

static CTK_Node *ctk_push_struct(CTK_Node *parent, cstr key) {
    CTK_ASSERT(key)
    CTK_ASSERT(parent->type == _CTK_NODE_TYPE_STRUCT);
    CTK_Node *n = _ctk_push_child(parent);
    n->type = _CTK_NODE_TYPE_STRUCT;
    n->key = ctk_create_string(key);
    return n;
}

// enum {
//     _CTK_SEARCH_TERM_TYPE_KEY,
//     _CTK_SEARCH_TERM_TYPE_IDX,
// };

// struct _ctk_search_term {
//     s32 type;
//     union {
//         struct {
//             u32 start;
//             u32 size;
//         } key;
//         u32 idx;
//     };
// };
// static CTK_Node *ctk_get(CTK_Node *parent, u32 idx);

// template<typename ...arg_types>
// static CTK_Node *ctk_find(CTK_Node *parent, cstr search_str, arg_types... args) {
//     CTK_FixedArray<_ctk_search_term, 16> search_terms = {};
//     u32 term_index = 0;
//     char search[256] = {};
//     u32 search_size = sprintf(search, search_str, args...);
//     while (term_index < search_size) {
//         if (search[term_index] == '[') {
//             ++term_index;
//             char *end;
//             _ctk_search_term *st = ctk_push(&search_terms);
//             st->type = _CTK_SEARCH_TERM_TYPE_IDX;
//             st->idx = strtoul(search + term_index, &end, 10);
//             term_index = end - search + 1;
//         } else {
//             _ctk_search_term *st = ctk_push(&search_terms);
//             st->type = _CTK_SEARCH_TERM_TYPE_KEY;
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
//     // CTK_EACH(_ctk_search_term, term, search_terms) {
//     //     if (term->type == _CTK_SEARCH_TERM_TYPE_KEY)
//     //         ctk_print_line("<_CTK_SEARCH_TERM_TYPE_KEY> %.*s", term->key.size, search + term->key.start);
//     //     else if (term->type == _CTK_SEARCH_TERM_TYPE_IDX)
//     //         ctk_print_line("<_CTK_SEARCH_TERM_TYPE_IDX> %u", term->idx);
//     //     else
//     //         ctk_print_line("<unknown type>");
//     // }
//     CTK_Node *base = parent;
//     CTK_EACH(_ctk_search_term, term, search_terms) {
//         if (term->type == _CTK_SEARCH_TERM_TYPE_IDX) {
//             base = ctk_get(base, term->idx);
//         } else {
//             CTK_EACH(CTK_Node, child, base->children) {
//                 if (ctk_strings_match(key, child->key)) {
//                     base = child;
//                     break;
//                 }
//             }
//         }
//     }
//     return base;
// }

// static CTK_Node *ctk_get(CTK_Node *parent, cstr key) {
//     CTK_EACH(CTK_Node, child, parent->children)
//         if (ctk_strings_match(child->key, key))
//             return child;
//     CTK_FATAL("failed to find child with key \"%s\" in parent \"%s\"", key, parent->key)
// }

// static CTK_Node *ctk_get(CTK_Node *parent, u32 idx) {
//     CTK_ASSERT(idx < parent->children.count)
//     return parent->children + idx;
// }

// static f32 ctk_f32(CTK_Node *parent, cstr key) {
//     return ctk_f32(ctk_get(parent, key)->value);
// }

// static f32 ctk_f32(CTK_Node *parent, u32 idx) {
//     return ctk_f32(ctk_get(parent, idx)->value);
// }

// static f64 ctk_f64(CTK_Node *parent, cstr key) {
//     return ctk_f64(ctk_get(parent, key)->value);
// }

// static f64 ctk_f64(CTK_Node *parent, u32 idx) {
//     return ctk_f64(ctk_get(parent, idx)->value);
// }

// static s32 ctk_s32(CTK_Node *parent, cstr key) {
//     return ctk_s32(ctk_get(parent, key)->value);
// }

// static s32 ctk_s32(CTK_Node *parent, u32 idx) {
//     return ctk_s32(ctk_get(parent, idx)->value);
// }

// static s64 ctk_s64(CTK_Node *parent, cstr key) {
//     return ctk_s64(ctk_get(parent, key)->value);
// }

// static s64 ctk_s64(CTK_Node *parent, u32 idx) {
//     return ctk_s64(ctk_get(parent, idx)->value);
// }

// static u32 ctk_u32(CTK_Node *parent, cstr key) {
//     return ctk_u32(ctk_get(parent, key)->value);
// }

// static u32 ctk_u32(CTK_Node *parent, u32 idx) {
//     return ctk_u32(ctk_get(parent, idx)->value);
// }

// static u64 ctk_u64(CTK_Node *parent, cstr key) {
//     return ctk_u64(ctk_get(parent, key)->value);
// }

// static u64 ctk_u64(CTK_Node *parent, u32 idx) {
//     return ctk_u64(ctk_get(parent, idx)->value);
// }

// static bool ctk_bool(CTK_Node *parent, cstr key) {
//     return ctk_bool(ctk_get(parent, key)->value);
// }

// static bool ctk_bool(CTK_Node *parent, u32 idx) {
//     return ctk_bool(ctk_get(parent, idx)->value);
// }

////////////////////////////////////////////////////////////
/// PARSER
////////////////////////////////////////////////////////////
enum {
    _CTK_TOKEN_TYPE_TEXT,
    _CTK_TOKEN_TYPE_STRING,
    _CTK_TOKEN_TYPE_ARRAY_OPEN,
    _CTK_TOKEN_TYPE_ARRAY_CLOSE,
    _CTK_TOKEN_TYPE_STRUCT_OPEN,
    _CTK_TOKEN_TYPE_STRUCT_CLOSE,
    _CTK_TOKEN_TYPE_UNKNOWN,
};

struct _CTK_Token {
    u32 start;
    u32 size;
    s32 type;
};

static char const SYMBOLS[] = { '[', ']', '{', '}' };
static char const SKIPPABLES[] = { ' ', '\0', '\r', '\n' };
static char const ESCAPABLE[] = { '"', '\\' };

#define _CTK_CHAR_LOOKUP(ARR)\
    for (u32 i = 0; i < CTK_ARRAY_COUNT(ARR); ++i)\
        if (ARR[i] == c)\
            return true;\
    \
    return false;

static bool is_symbol(char c) {
    _CTK_CHAR_LOOKUP(SYMBOLS)
}

static bool is_skippable(char c) {
    _CTK_CHAR_LOOKUP(SKIPPABLES)
}

static bool is_escapable(char c) {
    _CTK_CHAR_LOOKUP(ESCAPABLE)
}

static CTK_Node *ctk_read(cstr path) {
    // CTK_Array<char> file_str = ctk_read_file<char>(path);
    // ctk_print_line("%s:", path);
    // ctk_visualize_string(file_str.data, file_str.size, false);
    // auto tokens = ctk_create_array<_CTK_Token>(file_str.count); // Can't have more tokens than chars.

    // for (u32 base_idx = 0; base_idx < file_str.count;) {
    //     char c = file_str[base_idx];

    //     if (is_skippable(c)) {
    //         // SKIPPABLE
    //         ++base_idx;
    //         continue;
    //     } else if (is_symbol(c)) {
    //         // SYMBOL
    //         _CTK_Token *t = ctk_push(&tokens);
    //         t->start = base_idx;
    //         t->size = 1;
    //         t->type = c == '[' ? _CTK_TOKEN_TYPE_ARRAY_OPEN :
    //                   c == ']' ? _CTK_TOKEN_TYPE_ARRAY_CLOSE :
    //                   c == '{' ? _CTK_TOKEN_TYPE_STRUCT_OPEN :
    //                   c == '}' ? _CTK_TOKEN_TYPE_STRUCT_CLOSE :
    //                   _CTK_TOKEN_TYPE_UNKNOWN;

    //         if (t->type == _CTK_TOKEN_TYPE_UNKNOWN)
    //             CTK_FATAL("unknown symbol type for token char: %c", c)

    //         ++base_idx;
    //     } else if (c == '"') {
    //         // STRING
    //         _CTK_Token *t = ctk_push(&tokens);
    //         t->type = _CTK_TOKEN_TYPE_STRING;
    //         t->start = base_idx;
    //         u32 str_char_idx = base_idx + 1;
    //         bool next_char_escaped = false;

    //         for (; str_char_idx < file_str.count; ++str_char_idx) {
    //             char text_char = file_str[str_char_idx];

    //             if (next_char_escaped) {
    //                 if (!is_escapable(text_char))
    //                     CTK_FATAL("unsupported escape character: \\%c", text_char)

    //                 next_char_escaped = false;
    //             } else if (text_char == '\\') {
    //                 next_char_escaped = true;
    //             } else if (text_char == '"') {
    //                 ++str_char_idx;
    //                 break;
    //             }
    //         }

    //         t->size = str_char_idx - t->start;

    //         if (str_char_idx >= file_str.count)
    //             CTK_FATAL("reached end of file while parsing string: %.*s", t->size, file_str + t->start)

    //         base_idx = str_char_idx;
    //     } else {
    //         // TEXT
    //         _CTK_Token *t = ctk_push(&tokens);
    //         t->type = _CTK_TOKEN_TYPE_TEXT;
    //         t->start = base_idx;
    //         u32 text_char_idx = base_idx + 1;

    //         for (; text_char_idx < file_str.count; ++text_char_idx) {
    //             char text_char = file_str[text_char_idx];

    //             if (is_skippable(text_char) || is_symbol(text_char) || text_char == '"')
    //                 break;
    //         }

    //         t->size = text_char_idx - t->start;
    //         base_idx = text_char_idx;
    //     }
    // }

    // CTK_EACH(_CTK_Token, t, tokens) {
    //     cstr type_name = t->type == _CTK_TOKEN_TYPE_TEXT ?           "          TEXT" :
    //                      t->type == _CTK_TOKEN_TYPE_STRING ?         "        STRING" :
    //                      t->type == _CTK_TOKEN_TYPE_ARRAY_OPEN ?     "    ARRAY_OPEN" :
    //                      t->type == _CTK_TOKEN_TYPE_ARRAY_CLOSE ?    "   ARRAY_CLOSE" :
    //                      t->type == _CTK_TOKEN_TYPE_STRUCT_OPEN ?    "   STRUCT_OPEN" :
    //                      t->type == _CTK_TOKEN_TYPE_STRUCT_CLOSE ?   "  STRUCT_CLOSE" :
    //                                                                  "       UNKNOWN";
    //     ctk_print_line("%s |%.*s|", type_name, t->size, file_str + t->start);
    // }

    // // Count nodes and chars for allocating memory.
    // u32 total_node_count = 0;
    // u32 total_char_count = 0;
    // u32 root_child_count = 0;
    // u32 token_idx = 0;
    // u32 nest_level = 0;
    // s32 parent_type = _CTK_NODE_TYPE_STRUCT;

    // while (token_idx < tokens.count) {
    //     _CTK_Token *curr = tokens + token_idx;
    //     ++total_node_count;

    //     if (nest_level == 0)
    //         ++root_child_count;

    //     if (parent_type == _CTK_NODE_TYPE_STRUCT) {
    //         ++token_idx;

    //         if (curr->type == _CTK_TOKEN_TYPE_TEXT) {

    //         } else if (curr->type == _CTK_TOKEN_TYPE_ARRAY_OPEN) {

    //         } else if (curr->type == _CTK_TOKEN_TYPE_STRUCT_OPEN) {

    //         }
    //     }
    // }

    // CTK_Node *root = ctk_create(total_node_count, total_char_count, root_child_count);
    // _ctk_debug_node(root);

    // ctk_free(&file_str);
    return NULL;//root;
}
