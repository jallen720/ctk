#include "ctk/ctk.h"

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
    struct _ctkd_arena *arena;
    s32 type;
    cstr key;
    union {
        cstr value;
        struct ctk_buffer<struct ctkd_node> children;
    };
};

struct _ctkd_arena {
    struct ctk_buffer<char> chars;
    struct ctk_buffer<struct ctkd_node> nodes;
};

static cstr _ctkd_alloc_string(struct _ctkd_arena *a, cstr str) {
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

static struct ctk_buffer<struct ctkd_node> _ctkd_alloc_child_buffer(struct _ctkd_arena *a, u32 size) {
    if (a->nodes.count + size > a->nodes.size)
        CTK_FATAL("CTKD: out of memory for nodes")
    struct ctk_buffer<struct ctkd_node> child_buf = {};
    child_buf.data = a->nodes + a->nodes.count;
    child_buf.size = size;
    a->nodes.count += size;
    return child_buf;
}

static struct ctkd_node *ctkd_create(u32 max_nodes, u32 max_chars, u32 max_root_child_count) {
    auto arena = ctk_alloc_z<struct _ctkd_arena>();
    arena->chars = ctk_create_buffer<char>(max_chars);
    arena->nodes = ctk_create_buffer<struct ctkd_node>(max_nodes);
    struct ctkd_node *root = ctk_push(&arena->nodes);
    root->arena = arena;
    root->type = CTKD_NODE_TYPE_STRUCT;
    root->key = _ctkd_alloc_string(arena, "root");
    root->children = _ctkd_alloc_child_buffer(arena, max_root_child_count);
    return root;
}

static struct ctkd_node *_ctkd_push_vector(struct ctkd_node *parent, s32 type, u32 max_child_count, cstr key = NULL) {
    CTK_ASSERT(type == CTKD_NODE_TYPE_STRUCT || type == CTKD_NODE_TYPE_ARRAY);
    struct ctkd_node *child = ctk_push(&parent->children);
    child->arena = parent->arena;
    child->type = type;
    child->key = _ctkd_alloc_string(parent->arena, key);
    child->children = _ctkd_alloc_child_buffer(parent->arena, max_child_count);
    return child;
}

static struct ctkd_node *ctkd_push_array(struct ctkd_node *parent, u32 max_child_count, cstr key = NULL) {
    return _ctkd_push_vector(parent, CTKD_NODE_TYPE_ARRAY, max_child_count, key);
}

static struct ctkd_node *ctkd_push_struct(struct ctkd_node *parent, u32 max_child_count, cstr key = NULL) {
    return _ctkd_push_vector(parent, CTKD_NODE_TYPE_STRUCT, max_child_count, key);
}

static void ctkd_push_string(struct ctkd_node *parent, cstr value, cstr key = NULL) {
    struct ctkd_node *child = ctk_push(&parent->children);
    child->arena = parent->arena;
    child->type = CTKD_NODE_TYPE_SCALAR;
    child->key = _ctkd_alloc_string(parent->arena, key);
    child->value = _ctkd_alloc_string(parent->arena, value);
}

static void ctkd_push_bool(struct ctkd_node *parent, bool v, cstr key = NULL) {
    ctkd_push_string(parent, v ? "true" : "false", key);
}

#define _CTKD_NUMBER_PUSH_DECL(TYPE, FORMAT)\
    static void ctkd_push_ ## TYPE(struct ctkd_node *parent, TYPE v, cstr key = NULL) {\
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

static struct ctkd_node *ctkd_find(struct ctkd_node *parent, cstr key) {
    for (u32 i = 0; i < parent->children.count; ++i) {
        struct ctkd_node *child = parent->children + i;
        if (ctk_string_equal(child->key, key))
            return child;
    }
    return NULL;
}

#define CTKD_GET_BY_KEY_FATAL CTK_FATAL("failed to find child with key \"%s\" in parent \"%s\"", key, parent->key);

static struct ctkd_node *ctkd_get(struct ctkd_node *parent, u32 child_idx) {
    CTK_ASSERT(child_idx < parent->children.count)
    return parent->children + child_idx;
}

static struct ctkd_node *ctkd_get(struct ctkd_node *parent, cstr key) {
    struct ctkd_node *child = ctkd_find(parent, key);
    if (child)
        return child;
    CTKD_GET_BY_KEY_FATAL
}

static f32 ctkd_f32(struct ctkd_node *parent, u32 child_idx) {
    CTK_ASSERT(child_idx < parent->children.count);
    return strtof(parent->children[child_idx].value, NULL);
}

static f32 ctkd_f32(struct ctkd_node *parent, cstr key) {
    struct ctkd_node *child = ctkd_find(parent, key);
    if (child)
        return strtof(child->value, NULL);
    CTKD_GET_BY_KEY_FATAL
}

/// DEBUG
static void _ctkd_debug_arena(struct _ctkd_arena *a) {
    ctk_print_line(0u, "arena:");
    ctk_print_line(1u, "chars: %u/%u %.2f%%", a->chars.count, a->chars.size, (a->chars.count / (f32)a->chars.size) * 100);
    ctk_print_line(1u, "nodes: %u/%u %.2f%%", a->nodes.count, a->nodes.size, (a->nodes.count / (f32)a->nodes.size) * 100);
}

static void _ctkd_debug_node(struct ctkd_node *n, u32 tab = 0);

static void _ctkd_debug_children(struct ctkd_node *n, u32 parent_tab) {
    if (n->children.count > 0) {
        ctk_print_line();
        for (u32 i = 0; i < n->children.count; ++i)
            _ctkd_debug_node(ctkd_get(n, i), parent_tab + 1);
        ctk_print_tabs(parent_tab);
    }
}

static void _ctkd_debug_node(struct ctkd_node *n, u32 tab) {
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

////////////////////////////////////////////////////////////
/// PARSER
////////////////////////////////////////////////////////////
enum {
    CTKD_TOKEN_TYPE_TEXT,
    CTKD_TOKEN_TYPE_STRING,
    CTKD_TOKEN_TYPE_ARRAY_OPEN,
    CTKD_TOKEN_TYPE_ARRAY_CLOSE,
    CTKD_TOKEN_TYPE_STRUCT_OPEN,
    CTKD_TOKEN_TYPE_STRUCT_CLOSE,
    CTKD_TOKEN_TYPE_UNKNOWN,
};

struct token {
    u32 start;
    u32 size;
    s32 type;
};

static void _ctkd_visualize_string(cstr title, struct ctk_buffer<char> *str) {
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
            ctk_print("%c ", c);
        }
    }
}

static char const SYMBOLS[] = { '[', ']', '{', '}' };
static char const SKIPPABLES[] = { ' ', '\0', '\r', '\n' };
static char const ESCAPABLE[] = { '"', '\\' };

#define CHAR_LOOKUP(ARR)\
    for (u32 i = 0; i < CTK_ARRAY_COUNT(ARR); ++i)\
        if (ARR[i] == c)\
            return true;\
    return false;

static bool is_symbol(char c) {
    CHAR_LOOKUP(SYMBOLS)
}

static bool is_skippable(char c) {
    CHAR_LOOKUP(SKIPPABLES)
}

static bool is_escapable(char c) {
    CHAR_LOOKUP(ESCAPABLE)
}

static struct ctkd_node ctkd_read(cstr path) {
    struct ctk_buffer<char> file_str = ctk_read_file<char>(path);
    _ctkd_visualize_string(path, &file_str);
    auto tokens = ctk_create_buffer<struct token>(file_str.count); // Can't have more tokens than chars.
    for (u32 base_idx = 0; base_idx < file_str.count;) {
        char c = file_str[base_idx];
        if (is_skippable(c)) {
            // SKIPPABLE
            ++base_idx;
            continue;
        } else if (is_symbol(c)) {
            // SYMBOL
            struct token *t = ctk_push(&tokens);
            t->start = base_idx;
            t->size = 1;
            t->type = c == '[' ? CTKD_TOKEN_TYPE_ARRAY_OPEN :
                      c == ']' ? CTKD_TOKEN_TYPE_ARRAY_CLOSE :
                      c == '{' ? CTKD_TOKEN_TYPE_STRUCT_OPEN :
                      c == '}' ? CTKD_TOKEN_TYPE_STRUCT_CLOSE :
                      CTKD_TOKEN_TYPE_UNKNOWN;
            if (t->type == CTKD_TOKEN_TYPE_UNKNOWN)
                CTK_FATAL("unknown symbol type for token char: %c", c)
            ++base_idx;
        } else if (c == '"') {
            // STRING
            struct token *t = ctk_push(&tokens);
            t->type = CTKD_TOKEN_TYPE_STRING;
            t->start = base_idx;
            u32 str_char_idx = base_idx + 1;
            bool next_char_escaped = false;
            for (; str_char_idx < file_str.count; ++str_char_idx) {
                char text_char = file_str[str_char_idx];
                if (next_char_escaped) {
                    if (!is_escapable(text_char))
                        CTK_FATAL("unsupported escape character: \\%c", text_char);
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
                CTK_FATAL("reached end of file while parsing string: %.*s", t->size, file_str + t->start);
            base_idx = str_char_idx;
        } else {
            // TEXT
            struct token *t = ctk_push(&tokens);
            t->type = CTKD_TOKEN_TYPE_TEXT;
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

    for (u32 i = 0; i < tokens.count; ++i) {
        struct token *t = tokens + i;
        cstr type_name = t->type == CTKD_TOKEN_TYPE_TEXT ?           "          TEXT" :
                         t->type == CTKD_TOKEN_TYPE_STRING ?         "        STRING" :
                         t->type == CTKD_TOKEN_TYPE_ARRAY_OPEN ?     "    ARRAY_OPEN" :
                         t->type == CTKD_TOKEN_TYPE_ARRAY_CLOSE ?    "   ARRAY_CLOSE" :
                         t->type == CTKD_TOKEN_TYPE_STRUCT_OPEN ?    "   STRUCT_OPEN" :
                         t->type == CTKD_TOKEN_TYPE_STRUCT_CLOSE ?   "  STRUCT_CLOSE" :
                                                                     "       UNKNOWN";
        ctk_print_line("%s |%.*s|", type_name, t->size, file_str + t->start);
    }

    struct ctkd_node ctkd = {};

    ctk_free(&file_str);
    return ctkd;
}
