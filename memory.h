#pragma once

#include <cstdlib>
#include "ctk/ctk.h"
#include "ctk/math.h"

////////////////////////////////////////////////////////////
/// Data
////////////////////////////////////////////////////////////
typedef void *(*ctk_allocator_alloc)(void *data, u32 size);
typedef void *(*ctk_allocator_realloc)(void *data, void *mem, u32 old_size, u32 new_size);
typedef void (*ctk_allocator_free)(void *data, void *mem);

struct CTK_Allocator {
    void *data;
    ctk_allocator_alloc alloc;
    ctk_allocator_realloc realloc;
    ctk_allocator_free free;
};

struct CTK_Stack {
    CTK_Allocator allocator;
    u8 *mem;
    u32 size;
    u32 count;
};

struct alignas(CTK_CACHE_LINE) _CTK_BlockHeader {
    u8 *mem;
    u32 size;
    bool free;
    _CTK_BlockHeader *prev;
    _CTK_BlockHeader *next;
    _CTK_BlockHeader *prev_free;
    _CTK_BlockHeader *next_free;
};

struct alignas(CTK_CACHE_LINE) _CTK_ChunkHeader {
    u8 *mem;
    u32 size;
    _CTK_BlockHeader *block_list;
    _CTK_ChunkHeader *next;
};

struct CTK_FreeList {
    CTK_Allocator allocator;
    _CTK_ChunkHeader *chunk_list;
    _CTK_BlockHeader *free_list;
    _CTK_BlockHeader *largest_free;
    u32 chunk_size;
    u32 num_chunks;
};

static void *_ctk_system_allocator_alloc(void *_, u32 size);
static void *_ctk_system_allocator_realloc(void *_, void *mem, u32 old_size, u32 new_size);
static void _ctk_system_allocator_free(void *_, void *mem);

static CTK_Allocator CTK_SYSTEM_ALLOCATOR = {
    NULL,
    _ctk_system_allocator_alloc,
    _ctk_system_allocator_realloc,
    _ctk_system_allocator_free,
};

static void *_ctk_stack_allocator_alloc(void *stack, u32 size);

static CTK_Allocator CTK_STACK_ALLOCATOR = {
    NULL,
    _ctk_stack_allocator_alloc,
    NULL,
    NULL,
};

static void *_ctk_free_list_allocator_alloc(void *free_list, u32 size);
// static void *_ctk_free_list_allocator_realloc(void *free_list, void *mem, u32 old_size, u32 new_size);
static void _ctk_free_list_allocator_free(void *free_list, void *mem);

static CTK_Allocator CTK_FREE_LIST_ALLOCATOR = {
    NULL,
    _ctk_free_list_allocator_alloc,
    NULL,
    _ctk_free_list_allocator_free,
};

////////////////////////////////////////////////////////////
/// Debugging
////////////////////////////////////////////////////////////
static void ctk_visualize_stack(CTK_Stack *stack, cstr title = NULL) {
    auto buf = (char *)malloc(stack->size + 1);
    memset(buf, '#', stack->count);
    memset(buf + stack->count, '_', stack->size - stack->count);
    buf[stack->size] = '\0';
    ctk_print_line("%s (size: %u, count: %u):", title ? title : "stack", stack->size, stack->count);
    ctk_print_line(buf);
    ctk_print_line();
    free(buf);
}

static void ctk_print_block(_CTK_BlockHeader *block, u32 tabs) {
    ctk_print(tabs, "block %p - ", block);
    ctk_print_bits(block);
    ctk_print_line(":");
    ctk_print_line(tabs + 1, "mem:       %p", block->mem);
    ctk_print_line(tabs + 1, "size:      %u (total=%u)", block->size, block->size + sizeof(_CTK_BlockHeader));
    ctk_print_line(tabs + 1, "free:      %s", block->free ? "true" : "false");
    ctk_print_line(tabs + 1, "prev:      %p", block->prev);
    ctk_print_line(tabs + 1, "next:      %p", block->next);
    ctk_print_line(tabs + 1, "prev_free: %p", block->prev_free);
    ctk_print_line(tabs + 1, "next_free: %p", block->next_free);
}

static void ctk_print_free_list(CTK_FreeList *free_list, cstr title = NULL, u32 tabs = 0) {
    ctk_print_line(tabs, "%s:", title ? title : "free_list");
    ctk_print_line(tabs + 1, "chunk_size: %u", free_list->chunk_size);
    ctk_print_line(tabs + 1, "num_chunks: %u", free_list->num_chunks);
    ctk_print_line(tabs + 1, "chunk_list:");
    _CTK_ChunkHeader *chunk = free_list->chunk_list;

    while (chunk) {
        ctk_print(tabs + 2, "chunk %p - ", chunk);
        ctk_print_bits(chunk);
        ctk_print_line(":");
        ctk_print_line(tabs + 3, "mem:       %p", chunk->mem);
        ctk_print_line(tabs + 3, "size:      %u", chunk->size);
        ctk_print_line(tabs + 3, "next:      %p", chunk->next);
        ctk_print_line(tabs + 3, "block_list %p:", chunk->block_list);
        _CTK_BlockHeader *block = chunk->block_list;

        while (block) {
            ctk_print_block(block, tabs + 4);
            block = block->next;
        }

        chunk = chunk->next;
    }

    ctk_print_line(tabs + 1, "free_list:");
    _CTK_BlockHeader *free_block = free_list->free_list;

    while (free_block) {
        ctk_print_block(free_block, tabs + 2);
        free_block = free_block->next_free;
    }

    ctk_print_line(tabs + 1, "largest_free:");

    if (free_list->largest_free)
        ctk_print_block(free_list->largest_free, tabs + 2);
}

////////////////////////////////////////////////////////////
/// System Memory
////////////////////////////////////////////////////////////
#ifdef __GNUC__
    static void *ctk_alloc_aligned(u32 size, u32 alignment) {
        CTK_ASSERT(size > 0);
        CTK_ASSERT(alignment > 0);
        void *mem = aligned_alloc(alignment, size);
        CTK_ASSERT(mem != NULL);
        memset(mem, 0, size);
        return mem;
    }

    static void ctk_free_aligned(void *mem) {
        free(mem);
    }
#else
    static void *ctk_alloc_aligned(u32 size, u32 alignment) {
        CTK_ASSERT(size > 0);
        CTK_ASSERT(alignment > 0);
        void *mem = _aligned_malloc(size, alignment);
        CTK_ASSERT(mem != NULL);
        memset(mem, 0, size);
        return mem;
    }

    static void ctk_free_aligned(void *mem) {
        _aligned_free(mem);
    }
#endif

template<typename Type>
static Type *ctk_alloc_aligned(u32 size, u32 alignment) {
    return (Type *)ctk_alloc_aligned(size * sizeof(Type), alignment);
}

static void *ctk_alloc(u32 size) {
    CTK_ASSERT(size > 0);
    void *mem = malloc(size);
    CTK_ASSERT(mem != NULL);
    memset(mem, 0, size);
    return mem;
}

static void *_ctk_system_allocator_alloc(void *_, u32 size) {
    return ctk_alloc(size);
}

template<typename Type>
static Type *ctk_alloc(u32 size) {
    return (Type *)ctk_alloc(size * sizeof(Type));
}

static void *ctk_realloc(void *mem, u32 old_size, u32 new_size) {
    mem = realloc(mem, new_size);
    CTK_ASSERT(mem != NULL);

    // Zero newly allocated memory.
    if (new_size > old_size)
        memset((u8 *)mem + old_size, 0, new_size - old_size);

    return mem;
}

static void *_ctk_system_allocator_realloc(void *_, void *mem, u32 old_size, u32 new_size) {
    return ctk_realloc(mem, old_size, new_size);
}

template<typename Type>
static Type *ctk_realloc(Type *mem, u32 old_size, u32 new_size) {
    return ctk_realloc(mem, old_size * sizeof(Type), new_size * sizeof(Type));
}

static void _ctk_system_allocator_free(void *_, void *mem) {
    free(mem);
}

////////////////////////////////////////////////////////////
/// Allocator
////////////////////////////////////////////////////////////
static void *ctk_alloc(CTK_Allocator *allocator, u32 size) {
    return allocator->alloc(allocator->data, size);
}

template<typename Type>
static Type *ctk_alloc(CTK_Allocator *allocator, u32 size) {
    return (Type *)ctk_alloc(allocator, size * sizeof(Type));
}

static void *ctk_realloc(CTK_Allocator *allocator, void *mem, u32 old_size, u32 new_size) {
    return allocator->realloc(allocator->data, mem, old_size, new_size);
}

template<typename Type>
static Type *ctk_realloc(CTK_Allocator *allocator, Type *mem, u32 old_size, u32 new_size) {
    return (Type *)ctk_realloc(allocator, (void *)mem, old_size * sizeof(Type), new_size * sizeof(Type));
}

static void ctk_free(CTK_Allocator *allocator, void *mem) {
    allocator->free(allocator->data, mem);
}

////////////////////////////////////////////////////////////
/// Stack
////////////////////////////////////////////////////////////
static u8 *ctk_alloc(CTK_Stack *stack, u32 size) {
    CTK_ASSERT(stack->count + size <= stack->size);
    u8 *mem = stack->mem + stack->count;
    memset(mem, 0, size);
    stack->count += size;
    return mem;
}

static void *_ctk_stack_allocator_alloc(void *stack, u32 size) {
    return ctk_alloc((CTK_Stack *)stack, size);
}

template<typename Type>
static Type *ctk_alloc(CTK_Stack *stack, u32 size) {
    return (Type *)ctk_alloc(stack, size * sizeof(Type));
}

static CTK_Stack *ctk_create_stack(u32 size) {
    auto stack = ctk_alloc<CTK_Stack>(1);
    stack->size = size;
    stack->mem = ctk_alloc<u8>(size);
    stack->allocator = CTK_STACK_ALLOCATOR;
    stack->allocator.data = stack;
    return stack;
}

static CTK_Stack *ctk_create_stack(u32 size, CTK_Allocator *allocator) {
    auto stack = ctk_alloc<CTK_Stack>(allocator, 1);
    stack->size = size;
    stack->mem = ctk_alloc<u8>(allocator, size);
    stack->allocator = CTK_STACK_ALLOCATOR;
    stack->allocator.data = stack;
    return stack;
}

static u32 ctk_begin_region(CTK_Stack *stack) {
    return stack->count;
}

static void ctk_end_region(CTK_Stack *stack, u32 region) {
    stack->count = region;
}

////////////////////////////////////////////////////////////
/// Free List
////////////////////////////////////////////////////////////
static void _ctk_push_free_block(CTK_FreeList *free_list, _CTK_BlockHeader *block) {
    // Find where to insert free block based on its size in ascending order.
    _CTK_BlockHeader *prev = NULL;
    _CTK_BlockHeader *next = free_list->free_list;

    while (next) {
        bool should_succeed_prev = !prev || prev->size <= block->size;
        bool should_precede_next = !next || next->size >= block->size;
        if (should_succeed_prev && should_precede_next)
            break;

        prev = next;
        next = next->next_free;
    }

    // Join block with prev and next blocks if they exist, setting block as free_list head if prev was NULL.
    block->prev_free = prev;
    block->next_free = next;

    if (prev)
        prev->next_free = block;
    else
        free_list->free_list = block;

    if (next)
        next->prev_free = block;
    else
        free_list->largest_free = block;

    block->free = true;
}

static _CTK_ChunkHeader *_ctk_push_chunk(CTK_FreeList *free_list) {
    // Add new chunk to free-list.
    auto chunk = (_CTK_ChunkHeader *)ctk_alloc_aligned(free_list->chunk_size + sizeof(_CTK_ChunkHeader), CTK_CACHE_LINE);
    CTK_ASSERT(chunk != NULL);
    chunk->mem = (u8 *)(chunk + 1);
    chunk->size = free_list->chunk_size;
    chunk->next = free_list->chunk_list;
    free_list->chunk_list = chunk;
    ++free_list->num_chunks;

    // Add initial free block to chunk.
    auto init_block = (_CTK_BlockHeader *)chunk->mem;
    init_block->mem = (u8 *)(init_block + 1);
    init_block->size = chunk->size - sizeof(_CTK_BlockHeader);
    chunk->block_list = init_block;
    _ctk_push_free_block(free_list, init_block);

    return chunk;
}

static CTK_FreeList *ctk_create_free_list(u32 min_chunk_size) {
    auto free_list = ctk_alloc<CTK_FreeList>(1);
    free_list->chunk_size = ctk_total_chunk_size(min_chunk_size, CTK_CACHE_LINE);
    free_list->chunk_list = _ctk_push_chunk(free_list);
    free_list->allocator = CTK_FREE_LIST_ALLOCATOR;
    free_list->allocator.data = free_list;
    return free_list;
}

static void _ctk_remove_free_block(CTK_FreeList *free_list, _CTK_BlockHeader *free_block) {
    if (free_block == free_list->largest_free)
        free_list->largest_free = free_block->prev_free;

    if (free_block->prev_free)
        free_block->prev_free->next_free = free_block->next_free;
    else
        free_list->free_list = free_block->next_free;

    if (free_block->next_free)
        free_block->next_free->prev_free = free_block->prev_free;

    free_block->free = false;
}

static _CTK_BlockHeader *_ctk_shrink_allocated_block(CTK_FreeList *free_list, _CTK_BlockHeader *block,
                                                     u32 new_block_size) {
    // Shrinking a free block would simply cause the new free block added after it to be merged back into it.
    CTK_ASSERT(!block->free);

    // If space remaining after block shrinks isn't enough to hold a block header and >0 bytes of data, shrinking the
    // block is pointless.
    if (block->size - new_block_size < sizeof(_CTK_BlockHeader) + CTK_CACHE_LINE)
        return NULL;

    // Create free block to manage space after shrunk block.
    auto free_block = (_CTK_BlockHeader *)(block->mem + new_block_size);
    free_block->mem = (u8 *)(free_block + 1);
    free_block->size = block->size - new_block_size - sizeof(_CTK_BlockHeader);
    free_block->prev = block;
    free_block->next = block->next;
    _ctk_push_free_block(free_list, free_block);

    // Update block.
    block->size = new_block_size;
    block->next = free_block;

    return free_block;
}

static u8 *ctk_alloc(CTK_FreeList *free_list, u32 size) {
    // Blocks are aligned with cache-lines, so the effective size of an allocation is in intervals of cache-lines.
    u32 total_size = ctk_total_chunk_size(size, CTK_CACHE_LINE);

    // If a large enough free block doesn't exist, allocate a new chunk to ensure a large enough block is available.
    if (!free_list->largest_free || total_size > free_list->largest_free->size)
        _ctk_push_chunk(free_list);

    // Find first free block large enough to hold allocation.
    _CTK_BlockHeader *selected_block = free_list->free_list;

    while (selected_block) {
        if (selected_block->size >= total_size)
            break;

        selected_block = selected_block->next_free;
    }

    if (selected_block == NULL)
        CTK_FATAL("failed to find block in free-list large enough for allocation of size %u", size);

    // Allocate block by removing it from free list and shrinking it to be atleast the requested size, as well as
    // creating a new free block after it if there is sufficient space.
    _ctk_remove_free_block(free_list, selected_block);
    _ctk_shrink_allocated_block(free_list, selected_block, total_size);

    return selected_block->mem;
}

static void *_ctk_free_list_allocator_alloc(void *free_list, u32 size) {
    return ctk_alloc((CTK_FreeList *)free_list, size);
}

template<typename Type>
static Type *ctk_alloc(CTK_FreeList *free_list, u32 size) {
    return (Type *)ctk_alloc(free_list, size * sizeof(Type));
}

static _CTK_BlockHeader *_ctk_find_block(CTK_FreeList *free_list, void *mem) {
    // Find chunk that contains mem address.
    _CTK_ChunkHeader *chunk = free_list->chunk_list;

    while (chunk) {
        if (mem >= chunk->mem && mem < chunk->mem + chunk->size)
            break;

        chunk = chunk->next;
    }

    if (chunk == NULL)
        CTK_FATAL("failed to find memory chunk containing pointer %p", mem);

    // If mem is the beginning of a block, look at where the block would be to verify that the supposed block's mem
    // pointer is identical.
    auto block = ((_CTK_BlockHeader *)mem) - 1;

    if (block->mem != mem)
        CTK_FATAL("failed to find memory block for pointer %p", mem);

    if (block->free)
        CTK_FATAL("block for pointer %p is already free", mem);

    return block;
}

static void _ctk_merge_next(_CTK_BlockHeader *block) {
    // Overwrite entire next block.
    block->size += block->next->size + sizeof(_CTK_BlockHeader);

    // Join with block after next (if there is one).
    block->next = block->next->next;
    if (block->next)
        block->next->prev = block;
}

static _CTK_BlockHeader *_ctk_merge_free_neighbors(CTK_FreeList *free_list, _CTK_BlockHeader *block) {
    _CTK_BlockHeader *resulting_freed_block = block;

    if (block->next && block->next->free) {
        _ctk_remove_free_block(free_list, block->next);
        _ctk_merge_next(block);
    }

    if (block->prev && block->prev->free) {
        _ctk_remove_free_block(free_list, block->prev);
        _ctk_merge_next(block->prev);
        resulting_freed_block = block->prev; // Block being merged overwritten; push prev block to free list instead.
    }

    return resulting_freed_block;
}

static void ctk_free(CTK_FreeList *free_list, void *mem) {
    _CTK_BlockHeader *block = _ctk_find_block(free_list, mem);

    // Free block associated with mem address.
    block->free = true;
    block = _ctk_merge_free_neighbors(free_list, block);

    // Push whatever block results from merging the freed block with its free neighbors.
    _ctk_push_free_block(free_list, block);
}

static void _ctk_free_list_allocator_free(void *free_list, void *mem) {
    ctk_free((CTK_FreeList *)free_list, mem);
}

static void ctk_free(CTK_FreeList *free_list) {
    _CTK_ChunkHeader *chunk = free_list->chunk_list;
    while (chunk) {
        _CTK_ChunkHeader *next_chunk = chunk->next;
        ctk_free_aligned(chunk);
        chunk = next_chunk;
    }
}
