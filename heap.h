#pragma once

#include "ctk/ctk_2.h"

struct alignas(_CTK_CACHE_LINE) _CTK_BlockHeader {
    u8 *mem;
    u32 size;
    bool free;
    _CTK_BlockHeader *prev;
    _CTK_BlockHeader *next;
    _CTK_BlockHeader *prev_free;
    _CTK_BlockHeader *next_free;
};

struct alignas(_CTK_CACHE_LINE) _CTK_ChunkHeader {
    u8 *mem;
    u32 size;
    _CTK_BlockHeader *block_list;
    _CTK_ChunkHeader *next;
};

struct CTK_Heap {
    u32 chunk_size;
    u32 num_chunks;
    _CTK_ChunkHeader *chunk_list;
    _CTK_BlockHeader *free_list;
    _CTK_BlockHeader *largest_free;
};

static void _ctk_dump_block(u32 tab, _CTK_BlockHeader *block) {
    ctk_print/*_line*/(tab, "block %p:", block);ctk_print(" ");ctk_print_bits(block);ctk_print_line();
    ctk_print_line(tab + 1, "mem:       %p", block->mem);
    ctk_print_line(tab + 1, "size:      %u (total=%u)", block->size, block->size + sizeof(_CTK_BlockHeader));
    ctk_print_line(tab + 1, "free:      %s", block->free ? "true" : "false");
    ctk_print_line(tab + 1, "prev:      %p", block->prev);
    ctk_print_line(tab + 1, "next:      %p", block->next);
    ctk_print_line(tab + 1, "prev_free: %p", block->prev_free);
    ctk_print_line(tab + 1, "next_free: %p", block->next_free);
}

static void _ctk_dump_heap(CTK_Heap *heap) {
    ctk_print_line("heap:");
    ctk_print_line(1, "chunk_size: %u", heap->chunk_size);
    ctk_print_line(1, "num_chunks: %u", heap->num_chunks);
    ctk_print_line(1, "chunk_list:");
    _CTK_ChunkHeader *chunk = heap->chunk_list;
    while (chunk) {
        ctk_print/*_line*/(2, "chunk %p:", chunk);ctk_print(" ");ctk_print_bits(chunk);ctk_print_line();
        ctk_print_line(3, "mem:       %p", chunk->mem);
        ctk_print_line(3, "size:      %u", chunk->size);
        ctk_print_line(3, "next:      %p", chunk->next);
        ctk_print_line(3, "block_list %p:", chunk->block_list);
        _CTK_BlockHeader *block = chunk->block_list;
        while (block) {
            _ctk_dump_block(4, block);
            block = block->next;
        }
        chunk = chunk->next;
    }
    ctk_print_line(1, "free_list:");
    _CTK_BlockHeader *free_block = heap->free_list;
    while (free_block) {
        _ctk_dump_block(2, free_block);
        free_block = free_block->next_free;
    }
    ctk_print_line(1, "largest_free:");
    if (heap->largest_free)
        _ctk_dump_block(2, heap->largest_free);
}

static void _ctk_push_free_block(CTK_Heap *heap, _CTK_BlockHeader *block) {
    // Find where to insert free block based on its size in ascending order.
    _CTK_BlockHeader *prev = NULL;
    _CTK_BlockHeader *next = heap->free_list;
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
        heap->free_list = block;
    if (next)
        next->prev_free = block;
    else
        heap->largest_free = block;

    block->free = true;
}

static _CTK_ChunkHeader *_ctk_push_chunk(CTK_Heap *heap) {
    // Add new chunk to heap.
    auto chunk = (_CTK_ChunkHeader *)ctk_alloc_aligned(_CTK_CACHE_LINE, heap->chunk_size + sizeof(_CTK_ChunkHeader));
    CTK_ASSERT(chunk);
    chunk->mem = (u8 *)(chunk + 1);
    chunk->size = heap->chunk_size;
    chunk->next = heap->chunk_list;
    heap->chunk_list = chunk;
    ++heap->num_chunks;

    // Add initial free block to chunk.
    auto init_block = (_CTK_BlockHeader *)chunk->mem;
    init_block->mem = (u8 *)(init_block + 1);
    init_block->size = chunk->size - sizeof(_CTK_BlockHeader);
    chunk->block_list = init_block;
    _ctk_push_free_block(heap, init_block);

    return chunk;
}

static inline u32 _ctk_cache_line_aligned_size(u32 size) {
    return (((size - 1) / _CTK_CACHE_LINE) + 1) * _CTK_CACHE_LINE;
}

static CTK_Heap ctk_create_heap(u32 min_chunk_size = 4 * CTK_MEGABYTE) {
    CTK_Heap heap = {};
    heap.chunk_size = _ctk_cache_line_aligned_size(min_chunk_size);
    heap.chunk_list = _ctk_push_chunk(&heap);
    return heap;
}

static void _ctk_remove_free_block(CTK_Heap *heap, _CTK_BlockHeader *free_block) {
    if (free_block == heap->largest_free)
        heap->largest_free = free_block->prev_free;

    if (free_block->prev_free)
        free_block->prev_free->next_free = free_block->next_free;
    else
        heap->free_list = free_block->next_free;

    if (free_block->next_free)
        free_block->next_free->prev_free = free_block->prev_free;

    free_block->free = false;
}

static _CTK_BlockHeader *_ctk_shrink_allocated_block(CTK_Heap *heap, _CTK_BlockHeader *block, u32 new_block_size) {
    // Shrinking a free block would simply cause the new free block added after it to be merged back into it.
    CTK_ASSERT(!block->free);

    // If space remaining after block shrinks isn't enough to hold a block header and >0 bytes of data, shrinking the
    // block is pointless.
    if (block->size - new_block_size < sizeof(_CTK_BlockHeader) + _CTK_CACHE_LINE)
        return NULL;

    // Create free block to manage space after shrunk block.
    auto free_block = (_CTK_BlockHeader *)(block->mem + new_block_size);
    free_block->mem = (u8 *)(free_block + 1);
    free_block->size = block->size - new_block_size - sizeof(_CTK_BlockHeader);
    free_block->prev = block;
    free_block->next = block->next;
    _ctk_push_free_block(heap, free_block);

    // Update block.
    block->size = new_block_size;
    block->next = free_block;

    return free_block;
}

static void *ctk_alloc(CTK_Heap *heap, u32 min_size) {
    // Blocks are aligned with cache-lines, so the effective size of an allocation is in intervals of cache-lines.
    u32 size = _ctk_cache_line_aligned_size(min_size);

    // If a large enough free block doesn't exist, allocate a new chunk to ensure a large enough block is available.
    if (!heap->largest_free || size > heap->largest_free->size)
        _ctk_push_chunk(heap);

    // Find first free block large enough to hold allocation.
    _CTK_BlockHeader *selected_block = heap->free_list;
    while (selected_block) {
        if (selected_block->size >= size)
            break;
        selected_block = selected_block->next_free;
    }
    CTK_ASSERT(selected_block);

    // Allocate block by removing it from free list and shrinking it to be atleast the requested size, as well as
    // creating a new free block after it if there is sufficient space.
    _ctk_remove_free_block(heap, selected_block);
    _ctk_shrink_allocated_block(heap, selected_block, size);

    return selected_block->mem;
}

template<typename type>
static type *ctk_alloc(CTK_Heap *heap, u32 count) {
    return (type *)ctk_alloc(heap, sizeof(type) * count);
}

static _CTK_BlockHeader *_ctk_find_block(CTK_Heap *heap, void *mem) {
    // Find chunk that contains mem address.
    _CTK_ChunkHeader *chunk = heap->chunk_list;
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

static _CTK_BlockHeader *_ctk_merge_free_neighbors(CTK_Heap *heap, _CTK_BlockHeader *block) {
    _CTK_BlockHeader *resulting_freed_block = block;
    if (block->next && block->next->free) {
        _ctk_remove_free_block(heap, block->next);
        _ctk_merge_next(block);
    }
    if (block->prev && block->prev->free) {
        _ctk_remove_free_block(heap, block->prev);
        _ctk_merge_next(block->prev);
        resulting_freed_block = block->prev; // Block being merged overwritten; push prev block to free list instead.
    }
    return resulting_freed_block;
}

static void *ctk_realloc(CTK_Heap *heap, void *mem, u32 min_new_size) {
    u32 new_size = _ctk_cache_line_aligned_size(min_new_size);
    _CTK_BlockHeader *block = _ctk_find_block(heap, mem);
    if (new_size > block->size) {

    } else if (new_size < block->size) {
        _CTK_BlockHeader *free_block = _ctk_shrink_allocated_block(heap, block, new_size);
        if (free_block)
            _ctk_merge_free_neighbors(heap, free_block);
    }
    return block->mem;
}

static void ctk_free(CTK_Heap *heap, void *mem) {
    _CTK_BlockHeader *block = _ctk_find_block(heap, mem);

    // Free block associated with mem address.
    block->free = true;
    block = _ctk_merge_free_neighbors(heap, block);

    // Push whatever block results from merging the freed block with its free neighbors.
    _ctk_push_free_block(heap, block);
}

static void ctk_free(CTK_Heap *heap) {
    _CTK_ChunkHeader *chunk = heap->chunk_list;
    while (chunk) {
        _CTK_ChunkHeader *next_chunk = chunk->next;
        ctk_free_aligned(chunk);
        chunk = next_chunk;
    }
}
