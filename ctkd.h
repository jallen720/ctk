#include "ctk/ctk.h"

////////////////////////////////////////////////////////////
/// DATA STRUCT
////////////////////////////////////////////////////////////
struct _ctkd_block {
    void *mem;
    u32 size;
    u32 next;
};

struct ctkd_heap {
    void *mem;
    u32 size;
    u32 pool;
    u32 used_head;
    u32 free_head;
    struct ctk_buffer<struct _ctkd_block> blocks;
};

#define _CTKD_BLOCK_POOL_CHUNK_COUNT 2

static u32 _ctkd_pop_pool(struct ctkd_heap *heap) {
    if (heap->pool != CTK_U32_MAX) {
        u32 ret = heap->pool;
        heap->pool = heap->blocks[heap->pool].next;
        return ret;
    } else {
        if (heap->blocks.count >= heap->blocks.size)
            ctk_realloc_z_buffer(&heap->blocks, heap->blocks.size + _CTKD_BLOCK_POOL_CHUNK_COUNT);
        return heap->blocks.count++;
    }
}

static struct ctkd_heap ctkd_create_heap(u32 size) {
    struct ctkd_heap heap = {};
    heap.mem = malloc(size);
    heap.size = size;
    CTK_ASSERT(heap.mem != NULL);
    heap.blocks = ctk_create_buffer<struct _ctkd_block>(_CTKD_BLOCK_POOL_CHUNK_COUNT);
    heap.pool = CTK_U32_MAX;
    heap.used_head = CTK_U32_MAX;
    heap.free_head = _ctkd_pop_pool(&heap);
    struct _ctkd_block *free_block = heap.blocks + heap.free_head;
    free_block->mem = heap.mem;
    free_block->size = size;
    free_block->next = CTK_U32_MAX;
    return heap;
}

static void _ctkd_remove_block(struct ctkd_heap *heap, u32 *head_idx, u32 parent_idx, u32 block_idx) {
    // Rebase block's next block to parent (or head if block was head).
    if (parent_idx != CTK_U32_MAX)
        heap->blocks[parent_idx].next = heap->blocks[block_idx].next;
    else
        *head_idx = heap->blocks[block_idx].next;
}

static void *ctkd_alloc(struct ctkd_heap *heap, u32 size) {
    // Find first free block with enough space for allocation.
    u32 parent_idx = CTK_U32_MAX;
    u32 selected_idx = heap->free_head;
    while (selected_idx != CTK_U32_MAX) {
        struct _ctkd_block selected_block = heap->blocks[selected_idx];
        if (selected_block.size >= size)
            break;
        parent_idx = selected_idx;
        selected_idx = selected_block.next;
    }
    if (selected_idx == CTK_U32_MAX)
        CTK_FATAL("failed to find memory block with size >= %u", size)

    // Insert new block between selected block and its next block to manage leftover space from allocation.
    if (heap->blocks[selected_idx].size > size) {
        u32 next_idx = _ctkd_pop_pool(heap);
        heap->blocks[next_idx].mem = (char *)heap->blocks[selected_idx].mem + size;
        heap->blocks[next_idx].size = heap->blocks[selected_idx].size - size;
        heap->blocks[next_idx].next = heap->blocks[selected_idx].next;
        heap->blocks[selected_idx].size = size;
        heap->blocks[selected_idx].next = next_idx;
    }

    // Remove block from free list.
    _ctkd_remove_block(heap, &heap->free_head, parent_idx, selected_idx);

    // Move selected block to used list.
    heap->blocks[selected_idx].next = CTK_U32_MAX;
    if (heap->used_head == CTK_U32_MAX) {
        heap->used_head = selected_idx;
    } else {
        u32 used_tail = heap->used_head;
        while (heap->blocks[used_tail].next != CTK_U32_MAX)
            used_tail = heap->blocks[used_tail].next;
        heap->blocks[used_tail].next = selected_idx;
    }

    return heap->blocks[selected_idx].mem;
}

static void _ctkd_check_merge_free_block_next(struct ctkd_heap *heap, u32 block_idx) {
    u32 next_block_idx = heap->blocks[block_idx].next;
    if (next_block_idx == CTK_U32_MAX)
        return;
    if ((char *)heap->blocks[block_idx].mem + heap->blocks[block_idx].size == heap->blocks[next_block_idx].mem) {
        heap->blocks[block_idx].size += heap->blocks[next_block_idx].size;
        _ctkd_remove_block(heap, &heap->free_head, block_idx, next_block_idx);

        // Push next block to pool for reuse later.
        heap->blocks[next_block_idx].next = heap->pool;
        heap->pool = next_block_idx;
    }
}

static void ctkd_free(struct ctkd_heap *heap, void *mem) {
    // Find and remove block associated with memory from used list.
    u32 used_parent_idx = CTK_U32_MAX;
    u32 used_idx = heap->used_head;
    while (used_idx != CTK_U32_MAX) {
        struct _ctkd_block used_block = heap->blocks[used_idx];
        if (used_block.mem == mem)
            break;
        used_parent_idx = used_idx;
        used_idx = used_block.next;
    }
    if (used_idx == CTK_U32_MAX)
        CTK_FATAL("failed to find memory block associated with addres %p", mem)
    _ctkd_remove_block(heap, &heap->used_head, used_parent_idx, used_idx);

    // Insert block back into free list, merging with adjacent neighbor blocks.
    u32 free_parent_idx = heap->free_head;
    if (free_parent_idx == CTK_U32_MAX) {
        heap->free_head = used_idx;
    } else if (heap->blocks[free_parent_idx].mem > mem) {
        heap->blocks[used_idx].next = heap->free_head;
        heap->free_head = used_idx;
        _ctkd_check_merge_free_block_next(heap, used_idx);
    } else {
        while (free_parent_idx != CTK_U32_MAX) {
            bool after_block = heap->blocks[free_parent_idx].mem < mem;
            u32 next_block_idx = heap->blocks[free_parent_idx].next;
            bool before_next_block = next_block_idx != CTK_U32_MAX || heap->blocks[next_block_idx].mem > mem;
            if (after_block && before_next_block) {
                heap->blocks[used_idx].next = next_block_idx;
                heap->blocks[free_parent_idx].next = used_idx;
                _ctkd_check_merge_free_block_next(heap, used_idx);
                _ctkd_check_merge_free_block_next(heap, free_parent_idx);
            }
            free_parent_idx = next_block_idx;
        }
        if (free_parent_idx == CTK_U32_MAX)
            CTK_FATAL("failed to free block")
    }
}

static void _ctkd_visualize_blocks(struct ctkd_heap *heap, u32 head_idx, char *buf, char char_vis) {
    u32 block_idx = head_idx;
    while (block_idx != CTK_U32_MAX) {
        u32 block_mem_start = (char *)heap->blocks[block_idx].mem - heap->mem;
        struct _ctkd_block *block = heap->blocks + block_idx;
        buf[block_mem_start] = '|';
        for (u32 i = 1; i < block->size; ++i)
            buf[block_mem_start + i] = char_vis;
        block_idx = block->next;
    }
}

static void _ctkd_visualize_heap(struct ctkd_heap *heap) {
    auto buf = (char *)malloc(heap->size);
    memset(buf, '!', heap->size);
    _ctkd_visualize_blocks(heap, heap->free_head, buf, '-');
    _ctkd_visualize_blocks(heap, heap->used_head, buf, '#');
    ctk_print_line("[%.*s]", heap->size, buf);
    free(buf);
}

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
// static struct ctkd_node *ctkd_get(struct ctkd_node *parent, u32 idx);

// template<typename ...arg_types>
// static ctkd_node *ctkd_find(struct ctkd_node *parent, cstr search_str, arg_types... args) {
//     struct ctk_array<_ctkd_search_term, 16> search_terms = {};
//     u32 term_index = 0;
//     char search[256] = {};
//     u32 search_size = sprintf(search, search_str, args...);
//     while (term_index < search_size) {
//         if (search[term_index] == '[') {
//             ++term_index;
//             char *end;
//             struct _ctkd_search_term *st = ctk_push(&search_terms);
//             st->type = _CTKD_SEARCH_TERM_TYPE_IDX;
//             st->idx = strtoul(search + term_index, &end, 10);
//             term_index = end - search + 1;
//         } else {
//             struct _ctkd_search_term *st = ctk_push(&search_terms);
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
//     // CTK_EACH(struct _ctkd_search_term, term, search_terms) {
//     //     if (term->type == _CTKD_SEARCH_TERM_TYPE_KEY)
//     //         ctk_print_line("<_CTKD_SEARCH_TERM_TYPE_KEY> %.*s", term->key.size, search + term->key.start);
//     //     else if (term->type == _CTKD_SEARCH_TERM_TYPE_IDX)
//     //         ctk_print_line("<_CTKD_SEARCH_TERM_TYPE_IDX> %u", term->idx);
//     //     else
//     //         ctk_print_line("<unknown type>");
//     // }
//     struct ctkd_node *base = parent;
//     CTK_EACH(struct _ctkd_search_term, term, search_terms) {
//         if (term->type == _CTKD_SEARCH_TERM_TYPE_IDX) {
//             base = ctkd_get(base, term->idx);
//         } else {
//             CTK_EACH(struct ctkd_node, child, base->children) {
//                 if (ctk_strings_match(key, child->key)) {
//                     base = child;
//                     break;
//                 }
//             }
//         }
//     }
//     return base;
// }

static struct ctkd_node *ctkd_get(struct ctkd_node *parent, cstr key) {
    CTK_EACH(struct ctkd_node, child, parent->children)
        if (ctk_strings_match(child->key, key))
            return child;
    CTK_FATAL("failed to find child with key \"%s\" in parent \"%s\"", key, parent->key)
}

static struct ctkd_node *ctkd_get(struct ctkd_node *parent, u32 idx) {
    CTK_ASSERT(idx < parent->children.count)
    return parent->children + idx;
}

static f32 ctkd_f32(struct ctkd_node *parent, cstr key) {
    return ctk_f32(ctkd_get(parent, key)->value);
}

static f32 ctkd_f32(struct ctkd_node *parent, u32 idx) {
    return ctk_f32(ctkd_get(parent, idx)->value);
}

static f64 ctkd_f64(struct ctkd_node *parent, cstr key) {
    return ctk_f64(ctkd_get(parent, key)->value);
}

static f64 ctkd_f64(struct ctkd_node *parent, u32 idx) {
    return ctk_f64(ctkd_get(parent, idx)->value);
}

static s32 ctkd_s32(struct ctkd_node *parent, cstr key) {
    return ctk_s32(ctkd_get(parent, key)->value);
}

static s32 ctkd_s32(struct ctkd_node *parent, u32 idx) {
    return ctk_s32(ctkd_get(parent, idx)->value);
}

static s64 ctkd_s64(struct ctkd_node *parent, cstr key) {
    return ctk_s64(ctkd_get(parent, key)->value);
}

static s64 ctkd_s64(struct ctkd_node *parent, u32 idx) {
    return ctk_s64(ctkd_get(parent, idx)->value);
}

static u32 ctkd_u32(struct ctkd_node *parent, cstr key) {
    return ctk_u32(ctkd_get(parent, key)->value);
}

static u32 ctkd_u32(struct ctkd_node *parent, u32 idx) {
    return ctk_u32(ctkd_get(parent, idx)->value);
}

static u64 ctkd_u64(struct ctkd_node *parent, cstr key) {
    return ctk_u64(ctkd_get(parent, key)->value);
}

static u64 ctkd_u64(struct ctkd_node *parent, u32 idx) {
    return ctk_u64(ctkd_get(parent, idx)->value);
}

static bool ctkd_bool(struct ctkd_node *parent, cstr key) {
    return ctk_bool(ctkd_get(parent, key)->value);
}

static bool ctkd_bool(struct ctkd_node *parent, u32 idx) {
    return ctk_bool(ctkd_get(parent, idx)->value);
}

/// DEBUG
static void _ctkd_print_arena(struct _ctkd_arena *a, u32 tab = 0) {
    ctk_print_line(tab + 0, "arena:");
    ctk_print_line(tab + 1, "chars: %u/%u %.2f%%", a->chars.count, a->chars.size, (a->chars.count / (f32)a->chars.size) * 100);
    ctk_print_line(tab + 1, "nodes: %u/%u %.2f%%", a->nodes.count, a->nodes.size, (a->nodes.count / (f32)a->nodes.size) * 100);
}

static void _ctkd_print_node(struct ctkd_node *n, u32 tab = 0);

static void _ctkd_debug_children(struct ctkd_node *n, u32 parent_tab) {
    if (n->children.count > 0) {
        ctk_print_line();
        for (u32 i = 0; i < n->children.count; ++i)
            _ctkd_print_node(ctkd_get(n, i), parent_tab + 1);
        ctk_print_tabs(parent_tab);
    }
}

static void _ctkd_print_node(struct ctkd_node *n, u32 tab) {
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

static void _ctkd_debug_node(struct ctkd_node *n, u32 tab = 0) {
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

struct token {
    u32 start;
    u32 size;
    s32 type;
};

static void _ctkd_visualize_string(cstr title, struct ctk_buffer<char> *str, bool uniform_spacing = true) {
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

static struct ctkd_node *ctkd_read(cstr path) {
    struct ctk_buffer<char> file_str = ctk_read_file<char>(path);
    _ctkd_visualize_string(path, &file_str, false);
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
            struct token *t = ctk_push(&tokens);
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
            struct token *t = ctk_push(&tokens);
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

    CTK_EACH(struct token, t, tokens) {
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
    //     struct _ctkd_token *curr = tokens + token_idx;
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

    // struct ctkd_node *root = ctkd_create(total_node_count, total_char_count, root_child_count);
    // _ctkd_debug_node(root);

    ctk_free(&file_str);
    return NULL;//root;
}
