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
    ctk_print_tabs(tabs);

    if (n->key.data) {
        ctk_print("%s: ", n->key.data);
    }

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

            ctk_print_tabs(tabs);
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
    ctk_print_tabs(buf, tabs);

    if (n->key.data) {
        ctk_print(buf, "%s: ", n->key.data);
    }

    if (n->type == _CTK_NODE_TYPE_SCALAR) {
        ctk_print(buf, "%s\n", n->value.data);
    } else {
        ctk_print(buf, "%s", n->type == _CTK_NODE_TYPE_ARRAY ? "[" : "{");

        if (n->children.count > 0) {
            ctk_print(buf, "\n");
            ctk_print_node_children(buf, n, tabs + 1);
            ctk_print_tabs(buf, tabs);
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

    while (*end_child_ptr) {
        end_child_ptr = &(*end_child_ptr)->next;
    }

    CTK_Node *new_child = ctk_push(_CTK_NODE_POOL);
    *end_child_ptr = new_child;
    ++parent->children.count;
    new_child->next = NULL;
    return new_child;
}

static CTK_Node *_ctk_push_scalar(CTK_Node *parent, CTK_CharRange value) {
    CTK_Node *n = _ctk_push_child(parent);
    n->type = _CTK_NODE_TYPE_SCALAR;
    n->value = value.data ? ctk_create_string(value, CTK_CACHE_LINE) : ctk_create_string(CTK_CACHE_LINE, CTK_CACHE_LINE);
    return n;
}

static CTK_Node *_ctk_push_scalar(CTK_Node *parent, cstr value = NULL) {
    return _ctk_push_scalar(parent, { value, value ? strlen(value) : 0 });
}

static CTK_Node *ctk_push_string(CTK_Node *parent, CTK_CharRange key, CTK_CharRange value) {
    CTK_ASSERT(parent->type == _CTK_NODE_TYPE_STRUCT);
    CTK_ASSERT(key.data != NULL);
    CTK_Node *n = _ctk_push_scalar(parent, value);
    n->key = ctk_create_string(key, CTK_CACHE_LINE);
    return n;
}

static CTK_Node *ctk_push_string(CTK_Node *parent, cstr key, cstr value) {
    CTK_ASSERT(key != NULL);
    return ctk_push_string(parent, { key, strlen(key) }, { value, value ? strlen(value) : 0 });
}

static CTK_Node *ctk_push_string(CTK_Node *parent, CTK_CharRange value) {
    CTK_ASSERT(parent->type == _CTK_NODE_TYPE_ARRAY);
    return _ctk_push_scalar(parent, value);
}

static CTK_Node *ctk_push_string(CTK_Node *parent, cstr value) {
    return ctk_push_string(parent, { value, value ? strlen(value) : 0 });
}

#define _CTK_NUMBER_PUSH_DECL(TYPE, FORMAT)\
    static CTK_Node *ctk_push_ ## TYPE(CTK_Node *parent, CTK_CharRange key, TYPE value) {\
        CTK_Node *string = ctk_push_string(parent, key, {});\
        ctk_print(&string->value, FORMAT, value);\
        return string;\
    }\
    \
    static CTK_Node *ctk_push_ ## TYPE(CTK_Node *parent, cstr key, TYPE value) {\
        return ctk_push_ ## TYPE(parent, { key, key ? strlen(key) : 0 }, value);\
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

static CTK_Node *ctk_push_array(CTK_Node *parent, CTK_CharRange key) {
    CTK_ASSERT(parent->type == _CTK_NODE_TYPE_STRUCT);
    CTK_ASSERT(key.data);
    CTK_Node *n = _ctk_push_child(parent);
    n->type = _CTK_NODE_TYPE_ARRAY;
    n->key = ctk_create_string(key);
}

static CTK_Node *ctk_push_array(CTK_Node *parent, cstr key) {
    return ctk_push_array(parent, { key, key ? strlen(key) : 0 });
}

static CTK_Node *ctk_push_struct(CTK_Node *parent) {
    CTK_ASSERT(parent->type == _CTK_NODE_TYPE_ARRAY)
    CTK_Node *n = _ctk_push_child(parent);
    n->type = _CTK_NODE_TYPE_STRUCT;
    return n;
}

static CTK_Node *ctk_push_struct(CTK_Node *parent, CTK_CharRange key) {
    CTK_ASSERT(parent->type == _CTK_NODE_TYPE_STRUCT);
    CTK_ASSERT(key.data)
    CTK_Node *n = _ctk_push_child(parent);
    n->type = _CTK_NODE_TYPE_STRUCT;
    n->key = ctk_create_string(key);

}

static CTK_Node *ctk_push_struct(CTK_Node *parent, cstr key) {
    return ctk_push_struct(parent, { key, key ? strlen(key) : 0 });
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

// template<typename ...Args>
// static CTK_Node *ctk_find(CTK_Node *parent, cstr search_str, Args... args) {
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

//                 if (c == '.' || c == '[') {
//                     break;
//                 }
//             }

//             st->key.size = term_index - st->key.start;
//         }

//         if (search[term_index] == '.') {
//             ++term_index;
//         }
//     }
//     // CTK_EACH(_ctk_search_term, term, search_terms) {
//     //     if (term->type == _CTK_SEARCH_TERM_TYPE_KEY) {
//     //         ctk_print_line("<_CTK_SEARCH_TERM_TYPE_KEY> %.*s", term->key.size, search + term->key.start);
//     //     } else if (term->type == _CTK_SEARCH_TERM_TYPE_IDX) {
//     //         ctk_print_line("<_CTK_SEARCH_TERM_TYPE_IDX> %u", term->idx);
//     //     } else {
//     //         ctk_print_line("<unknown type>");
//     //     }
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

static CTK_Node *ctk_get(CTK_Node *parent, cstr key) {
    CTK_Node *child = parent->children.list;

    while (child) {
        if (ctk_strings_match(&child->key, key)) {
            return child;
        }

        child = child->next;
    }

    CTK_FATAL("failed to find child with key \"%s\" in parent \"%s\"", key, parent->key)
}

static CTK_Node *ctk_get(CTK_Node *parent, u32 idx) {
    CTK_ASSERT(idx < parent->children.count);
    CTK_Node *child = parent->children.list;
    u32 child_idx = 0;

    while (child) {
        if (child_idx == idx) {
            return child;
        }

        child = child->next;
        child_idx++;
    }

    CTK_FATAL("failed to find child at index %u in parent \"%s\"", idx, parent->key)
}

static f32 ctk_f32(CTK_Node *parent, cstr key) {
    return ctk_f32(&ctk_get(parent, key)->value);
}

static f32 ctk_f32(CTK_Node *parent, u32 idx) {
    return ctk_f32(&ctk_get(parent, idx)->value);
}

static f64 ctk_f64(CTK_Node *parent, cstr key) {
    return ctk_f64(&ctk_get(parent, key)->value);
}

static f64 ctk_f64(CTK_Node *parent, u32 idx) {
    return ctk_f64(&ctk_get(parent, idx)->value);
}

static s32 ctk_s32(CTK_Node *parent, cstr key) {
    return ctk_s32(&ctk_get(parent, key)->value);
}

static s32 ctk_s32(CTK_Node *parent, u32 idx) {
    return ctk_s32(&ctk_get(parent, idx)->value);
}

static s64 ctk_s64(CTK_Node *parent, cstr key) {
    return ctk_s64(&ctk_get(parent, key)->value);
}

static s64 ctk_s64(CTK_Node *parent, u32 idx) {
    return ctk_s64(&ctk_get(parent, idx)->value);
}

static u32 ctk_u32(CTK_Node *parent, cstr key) {
    return ctk_u32(&ctk_get(parent, key)->value);
}

static u32 ctk_u32(CTK_Node *parent, u32 idx) {
    return ctk_u32(&ctk_get(parent, idx)->value);
}

static u64 ctk_u64(CTK_Node *parent, cstr key) {
    return ctk_u64(&ctk_get(parent, key)->value);
}

static u64 ctk_u64(CTK_Node *parent, u32 idx) {
    return ctk_u64(&ctk_get(parent, idx)->value);
}

static bool ctk_bool(CTK_Node *parent, cstr key) {
    return ctk_bool(&ctk_get(parent, key)->value);
}

static bool ctk_bool(CTK_Node *parent, u32 idx) {
    return ctk_bool(&ctk_get(parent, idx)->value);
}

////////////////////////////////////////////////////////////
/// Parser
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
    CTK_CharRange char_range;
    s32 type;
};

static bool _ctk_contains_char(char c, cstr chars) {
    for (u32 i = 0; i < strlen(chars); ++i) {
        if (chars[i] == c) {
            return true;
        }
    }

    return false;
}

static bool _ctk_is_symbol(char c) {
    return _ctk_contains_char(c, "[]{}");
}

static bool _ctk_is_skippable(char c) {
    return _ctk_contains_char(c, " \r\n\t") || c == '\0';
}

static bool _ctk_is_escapable(char c) {
    return _ctk_contains_char(c, "\"\\");
}

static CTK_Array<_CTK_Token> _ctk_parse_tokens(CTK_String *string) {
    auto tokens = ctk_create_array<_CTK_Token>(CTK_KILOBYTE, CTK_KILOBYTE);

    for (u32 base_idx = 0; base_idx < string->count;) {
        char c = string->data[base_idx];

        if (_ctk_is_skippable(c)) {
            // SKIPPABLE
            ++base_idx;
        } else if (_ctk_is_symbol(c)) {
            // SYMBOL
            _CTK_Token *t = ctk_push<_CTK_Token>(&tokens);
            t->char_range = { string->data + base_idx, 1 };
            t->type = c == '[' ? _CTK_TOKEN_TYPE_ARRAY_OPEN :
                      c == ']' ? _CTK_TOKEN_TYPE_ARRAY_CLOSE :
                      c == '{' ? _CTK_TOKEN_TYPE_STRUCT_OPEN :
                      c == '}' ? _CTK_TOKEN_TYPE_STRUCT_CLOSE :
                      _CTK_TOKEN_TYPE_UNKNOWN;

            if (t->type == _CTK_TOKEN_TYPE_UNKNOWN) {
                CTK_FATAL("unknown symbol type for token char: %c", c)
            }

            ++base_idx;
        } else if (c == '"') {
            // STRING
            _CTK_Token *t = ctk_push(&tokens);
            t->type = _CTK_TOKEN_TYPE_STRING;
            u32 string_start = base_idx + 1;
            t->char_range.data = string->data + string_start;
            u32 str_char_idx = string_start;
            bool next_char_escaped = false;

            for (; str_char_idx < string->count; ++str_char_idx) {
                char text_char = string->data[str_char_idx];

                if (next_char_escaped) {
                    if (!_ctk_is_escapable(text_char)) {
                        CTK_FATAL("unsupported escape character: \\%c", text_char)
                    }

                    next_char_escaped = false;
                } else if (text_char == '\\') {
                    next_char_escaped = true;
                } else if (text_char == '"') {
                    ++str_char_idx;
                    break;
                }
            }

            t->char_range.size = str_char_idx - 1 - string_start;

            if (str_char_idx >= string->count) {
                CTK_FATAL("reached end of file while parsing string: %.*s", t->char_range.size, t->char_range.data)
            }

            base_idx = str_char_idx;
        } else if (c == '#') {
            // COMMENT
            while (string->data[++base_idx] != '\n'); // Skip to first newline.
            ++base_idx;
        } else {
            // TEXT
            _CTK_Token *t = ctk_push(&tokens);
            t->type = _CTK_TOKEN_TYPE_TEXT;
            t->char_range.data = string->data + base_idx;
            u32 text_char_idx = base_idx + 1;

            for (; text_char_idx < string->count; ++text_char_idx) {
                char text_char = string->data[text_char_idx];

                if (_ctk_is_skippable(text_char) || _ctk_is_symbol(text_char) || text_char == '"') {
                    break;
                }
            }

            t->char_range.size = text_char_idx - base_idx;
            base_idx = text_char_idx;
        }
    }

    CTK_EACH(_CTK_Token, t, tokens) {
        cstr type_name = t->type == _CTK_TOKEN_TYPE_TEXT ?           "          TEXT" :
                         t->type == _CTK_TOKEN_TYPE_STRING ?         "        STRING" :
                         t->type == _CTK_TOKEN_TYPE_ARRAY_OPEN ?     "    ARRAY_OPEN" :
                         t->type == _CTK_TOKEN_TYPE_ARRAY_CLOSE ?    "   ARRAY_CLOSE" :
                         t->type == _CTK_TOKEN_TYPE_STRUCT_OPEN ?    "   STRUCT_OPEN" :
                         t->type == _CTK_TOKEN_TYPE_STRUCT_CLOSE ?   "  STRUCT_CLOSE" :
                                                                     "       UNKNOWN";
        ctk_print_line("%s |%.*s|", type_name, t->char_range.size, t->char_range.data);
    }

    return tokens;
}

static void _ctk_process_array_child_tokens(CTK_Node* parent, CTK_Array<_CTK_Token>* tokens, u32 *idx);

static void _ctk_process_struct_child_tokens(CTK_Node *parent, CTK_Array<_CTK_Token> *tokens, u32 *idx) {
    while (*idx < tokens->count && ctk_get(tokens, *idx)->type != _CTK_TOKEN_TYPE_STRUCT_CLOSE) {
        _CTK_Token *key = ctk_get(tokens, *idx);
        CTK_ASSERT(key->type == _CTK_TOKEN_TYPE_TEXT);
        _CTK_Token *value = ctk_get(tokens, ++(*idx));
        ctk_print("processing struct child: key: %.*s value: %.*s\n",
                  key->char_range.size, key->char_range.data,
                  value->char_range.size, value->char_range.data);

        ++(*idx);

        if (value->type == _CTK_TOKEN_TYPE_STRING || value->type == _CTK_TOKEN_TYPE_TEXT) {
            ctk_push_string(parent, key->char_range, value->char_range);
        } else if (value->type == _CTK_TOKEN_TYPE_STRUCT_OPEN) {
            CTK_Node *struct_node = ctk_push_struct(parent, key->char_range);
            _ctk_process_struct_child_tokens(struct_node, tokens, idx);
        } else if (value->type == _CTK_TOKEN_TYPE_ARRAY_OPEN) {
            CTK_Node *array_node = ctk_push_array(parent, key->char_range);
            _ctk_process_array_child_tokens(array_node, tokens, idx);
        }
    }
    ++(*idx);
}

static void _ctk_process_array_child_tokens(CTK_Node *parent, CTK_Array<_CTK_Token> *tokens, u32 *idx) {
    while (*idx < tokens->count && ctk_get(tokens, *idx)->type != _CTK_TOKEN_TYPE_ARRAY_CLOSE) {
        _CTK_Token *value = ctk_get(tokens, *idx);
        ctk_print("processing array child: value: %.*s\n",
                  value->char_range.size, value->char_range.data);

        ++(*idx);

        if (value->type == _CTK_TOKEN_TYPE_STRING || value->type == _CTK_TOKEN_TYPE_TEXT) {
            ctk_push_string(parent, value->char_range);
        } else if (value->type == _CTK_TOKEN_TYPE_STRUCT_OPEN) {
            CTK_Node *struct_node = ctk_push_struct(parent);
            _ctk_process_struct_child_tokens(struct_node, tokens, idx);
        } else if (value->type == _CTK_TOKEN_TYPE_ARRAY_OPEN) {
            CTK_Node *array_node = ctk_push_array(parent);
            _ctk_process_array_child_tokens(array_node, tokens, idx);
        }
    }
    ++(*idx);
}

static CTK_Node *ctk_read(cstr path) {
    CTK_String file_str = ctk_read_file<char>(path);
    ctk_print_line("%s:", path);
    ctk_visualize_string(file_str.data, file_str.size, false);

    CTK_Array<_CTK_Token> tokens = _ctk_parse_tokens(&file_str);
    CTK_Node *root = ctk_create_root_node();
    u32 idx = 0;
    _ctk_process_struct_child_tokens(root, &tokens, &idx);
    ctk_print("\n");

    ctk_free(&tokens);
    ctk_free(&file_str);
    return root;
}
