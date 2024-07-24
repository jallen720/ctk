/// Data
////////////////////////////////////////////////////////////
union RangeKey
{
    // For free ranges.
    struct
    {
        uint32 byte_index;
        uint32 byte_size;
    };
    // For used ranges.
    struct
    {
        uint32 mem_byte_index;
        uint32 alignment;
    };
};

struct Range
{
    uint32 byte_index;
    uint32 byte_size;
    uint32 prev_range_index;
    uint32 next_range_index;
};

struct FreeListInfo
{
    uint32 max_range_count;
};

struct FreeList
{
    uint8* mem;
    uint32 byte_size;
    uint32 first_range_index;

    Range*    ranges;
    RangeKey* range_keys;
    uint32    used_range_count;
    uint32    free_range_count;
    uint32    max_range_count;
};

/// Utils
////////////////////////////////////////////////////////////
uint32 GetFreeRangesFirstIndex(FreeList* free_list)
{
    return free_list->max_range_count - free_list->free_range_count;
}

bool IsFreeRangeIndex(FreeList* free_list, uint32 range_index)
{
    return range_index >= GetFreeRangesFirstIndex(free_list);
}

bool IsUsedRangeIndex(FreeList* free_list, uint32 range_index)
{
    return range_index < free_list->used_range_count;
}

uint8* GetRangeMem(FreeList* free_list, uint32 range_byte_index)
{
    return &free_list->mem[range_byte_index];
}

uint32 AddUsedRange(FreeList* free_list, Range range, RangeKey range_key)
{
    if (free_list->used_range_count + free_list->free_range_count >= free_list->max_range_count)
    {
        CTK_FATAL("can't add used-range; free-list is already at max-total-range-count (%u)",
                  free_list->max_range_count);
    }

    uint32 range_index = free_list->used_range_count;
    free_list->ranges    [range_index] = range;
    free_list->range_keys[range_index] = range_key;
    free_list->used_range_count += 1;

    return range_index;
}

uint32 AddFreeRange(FreeList* free_list, Range range)
{
    if (free_list->used_range_count + free_list->free_range_count >= free_list->max_range_count)
    {
        CTK_FATAL("can't add free-range; free-list is already at max-total-range-count (%u)",
                  free_list->max_range_count);
    }

    uint32 range_index = free_list->max_range_count - free_list->free_range_count - 1;
    free_list->ranges    [range_index] = range;
    free_list->range_keys[range_index] = { .byte_index = range.byte_index, .byte_size = range.byte_size };
    free_list->free_range_count += 1;

    return range_index;
}

void UpdateNeighborLinks(FreeList* free_list, uint32 range_index)
{
    Range* range = &free_list->ranges[range_index];
    if (range->prev_range_index != UINT32_MAX)
    {
        free_list->ranges[range->prev_range_index].next_range_index = range_index;
    }
    if (range->next_range_index != UINT32_MAX)
    {
        free_list->ranges[range->next_range_index].prev_range_index = range_index;
    }
}

void LinkInsertedPrevRange(FreeList* free_list, Range* range, uint32 inserted_prev_range_index)
{
    if (range->prev_range_index != UINT32_MAX)
    {
        free_list->ranges[range->prev_range_index].next_range_index = inserted_prev_range_index;
    }
    range->prev_range_index = inserted_prev_range_index;
}

void LinkInsertedNextRange(FreeList* free_list, Range* range, uint32 inserted_next_range_index)
{
    if (range->next_range_index != UINT32_MAX)
    {
        free_list->ranges[range->next_range_index].prev_range_index = inserted_next_range_index;
    }
    range->next_range_index = inserted_next_range_index;
}

void LinkNextRange(FreeList* free_list, Range* range, uint32 range_index, uint32 next_range_index)
{
    if (next_range_index != UINT32_MAX)
    {
        free_list->ranges[next_range_index].prev_range_index = range_index;
    }
    range->next_range_index = next_range_index;
}

void RemoveUsedRange(FreeList* free_list, uint32 used_range_index)
{
    uint32 used_ranges_last_index = free_list->used_range_count - 1;
    if (free_list->used_range_count > 1 && used_range_index != used_ranges_last_index)
    {
        free_list->range_keys[used_range_index] = free_list->range_keys[used_ranges_last_index];
        free_list->ranges    [used_range_index] = free_list->ranges    [used_ranges_last_index];

        // Update neighbor links of range moved into removed range's slot, as it has a new index now.
        UpdateNeighborLinks(free_list, used_range_index);
    }

    // Update used-range count.
    --free_list->used_range_count;
}

void RemoveFreeRange(FreeList* free_list, uint32 free_range_index)
{
    uint32 free_ranges_first_index = GetFreeRangesFirstIndex(free_list);
    if (free_list->free_range_count > 1 && free_range_index != free_ranges_first_index)
    {
        free_list->range_keys[free_range_index] = free_list->range_keys[free_ranges_first_index];
        free_list->ranges    [free_range_index] = free_list->ranges    [free_ranges_first_index];

        // Update neighbor links of range moved into removed range's slot, as it has a new index now.
        UpdateNeighborLinks(free_list, free_range_index);
    }

    // Update free-range count.
    --free_list->free_range_count;
}

uint32 FindUsedRangeIndex(FreeList* free_list, void* mem)
{
    // Verify mem is owned by free-list.
    if (mem < free_list->mem)
    {
        CTK_FATAL("can't find range for memory @ 0x%p; memory is before free-list memory start (0x%p)", mem,
                  free_list->mem);
    }

    uint8* free_list_mem_end = free_list->mem + free_list->byte_size;
    if (mem >= free_list_mem_end)
    {
        CTK_FATAL("can't find range for memory @ 0x%p; memory is at/after free-list memory end (0x%p)", mem,
                  free_list_mem_end);
    }

    // Find mem's range.
    uint32 mem_byte_index = (uint32)((uint8*)mem - free_list->mem);
    CTK_ITER_PTR(used_range_key, &free_list->range_keys[0], free_list->used_range_count)
    {
        if (used_range_key->mem_byte_index == mem_byte_index)
        {
            return (uint32)(used_range_key - free_list->range_keys);
        }
    }

    return UINT32_MAX;
}

/// Internals
////////////////////////////////////////////////////////////
uint8* InternalAllocate(FreeList* free_list, uint32 mem_byte_size, uint32 alignment)
{
    // Ensure there is atleast 1 free-range available for search.
    if (free_list->free_range_count == 0)
    {
        CTK_FATAL("can't allocate from free-list: no free-ranges available");
    }

    // Find large enough range for allocation.
    uint32 free_range_index = UINT32_MAX;
    uint32 mem_byte_index   = 0;
    uint32 range_byte_size  = 0;

    CTK_ITER_PTR(free_range_key, &free_list->range_keys[GetFreeRangesFirstIndex(free_list)],
                 free_list->free_range_count)
    {
        uint8* range_mem        = GetRangeMem(free_list, free_range_key->byte_index);
        uint32 alignment_offset = (uint32)(Align(range_mem, alignment) - range_mem);

        mem_byte_index  = free_range_key->byte_index + alignment_offset;
        range_byte_size = alignment_offset + mem_byte_size;

        if (free_range_key->byte_size >= range_byte_size)
        {
            free_range_index = free_range_key - free_list->range_keys;
            break;
        }
    }

    if (free_range_index == UINT32_MAX)
    {
        CTK_FATAL("can't allocate %u bytes aligned to %u from free-list: no free-ranges are large enough",
                  mem_byte_size, alignment);
    }

    // Allocate range.
    Range* free_range = &free_list->ranges[free_range_index];
    if (free_range->byte_size == range_byte_size)
    {
        // Convert whole free-range to used-range for allocation; links are already what they should be.
        uint32 allocated_range_index = AddUsedRange(free_list, *free_range, { mem_byte_index, alignment });
        RemoveFreeRange(free_list, free_range_index);

        // Update neighbor links since free-range was converted to used-range (will have new index).
        UpdateNeighborLinks(free_list, allocated_range_index);
    }
    else
    {
        // Add allocated-range at free-range's byte-index.
        uint32 allocated_range_index =
            AddUsedRange(free_list,
                         {
                             .byte_index       = free_range->byte_index,
                             .byte_size        = range_byte_size,
                             .prev_range_index = free_range->prev_range_index,
                             .next_range_index = free_range_index,
                         },
                         {
                             .mem_byte_index = mem_byte_index,
                             .alignment      = alignment,
                         });

        // Link allocated-range between free-range and free-range's prev-range.
        LinkInsertedPrevRange(free_list, free_range, allocated_range_index);

        // Move free-range to end of allocated range, resize, then update free-range map with new size.
        free_range->byte_index += range_byte_size;
        free_range->byte_size  -= range_byte_size;
        free_list->range_keys[free_range_index] =
        {
            .byte_index = free_range->byte_index,
            .byte_size  = free_range->byte_size,
        };
    }

    // Return pointer to allocated range.
    return GetRangeMem(free_list, mem_byte_index);
}

void InternalDeallocate(FreeList* free_list, Range* used_range, uint32 used_range_index)
{
    // Deallocate used-range.
    uint32 prev_range_index = used_range->prev_range_index;
    uint32 next_range_index = used_range->next_range_index;

    Range* new_free_range       = NULL;
    uint32 new_free_range_index = UINT32_MAX;

    // Check if new-free-range will be the used-range for the allocation or used-range's prev-range, which used-range
    // will be merged into if it is free.
    if (prev_range_index != UINT32_MAX && IsFreeRangeIndex(free_list, prev_range_index))
    {
        // Set new-free-range to prev-range.
        new_free_range_index = prev_range_index;
        new_free_range       = &free_list->ranges[new_free_range_index];

        // Merge used-range's byte-size into new-free-range and update free-range map with new byte-size.
        new_free_range->byte_size += used_range->byte_size;
        free_list->range_keys[new_free_range_index] =
        {
            .byte_index = new_free_range->byte_index,
            .byte_size  = new_free_range->byte_size,
        };

        // Link new-free-range to used-range's next-range, since used-range has been merged (will be removed later).
        LinkNextRange(free_list, new_free_range, new_free_range_index, next_range_index);
    }
    else
    {
        // Convert used-range to free-range and use as new-free-range; links are already what they should be.
        new_free_range_index = AddFreeRange(free_list, *used_range);
        new_free_range       = &free_list->ranges[new_free_range_index];

        // Update neighbor links since used-range was converted to free-range (will have new index).
        UpdateNeighborLinks(free_list, new_free_range_index);
    }

    // Remove allocation's used-range as it was either merged with free prev-range or added to free-range map.
    RemoveUsedRange(free_list, used_range_index);

    // Check if next range is free and needs to be merged.
    if (next_range_index != UINT32_MAX && IsFreeRangeIndex(free_list, next_range_index))
    {
        Range* next_range = &free_list->ranges[next_range_index];

        // Merge next-range's byte-size into new-free-range and update free-range map with new byte-size.
        new_free_range->byte_size += next_range->byte_size;
        free_list->range_keys[new_free_range_index] =
        {
            .byte_index = new_free_range->byte_index,
            .byte_size  = new_free_range->byte_size,
        };

        // Link new-free-range to next-range's next-range, then remove next-range as it was merged into new-free-range.
        LinkNextRange(free_list, new_free_range, new_free_range_index, next_range->next_range_index);
        RemoveFreeRange(free_list, next_range_index);
    }
}

void MoveToNewAllocation(FreeList* free_list, Range* used_range, uint32 used_range_index,
                                uint32 reallocate_byte_size, uint32 alignment, uint8* mem, uint8** reallocated_mem)
{
    // Cache used-range byte size before it gets deallocated for moving memory to newly allocated range.
    uint32 original_used_range_byte_size = used_range->byte_size;

    // Allocate new range with required space, then move memory from original used-range to it.
    InternalDeallocate(free_list, used_range, used_range_index);
    *reallocated_mem = InternalAllocate(free_list, reallocate_byte_size, alignment);
    memmove(*reallocated_mem, mem, original_used_range_byte_size);
}

uint8* InternalReallocate(FreeList* free_list, uint32 used_range_index, uint32 reallocate_byte_size,
                                 uint32 alignment, uint8* mem)
{
    Range* used_range = &free_list->ranges[used_range_index];
    uint8* reallocated_mem = mem;
    uint32 next_range_index = used_range->next_range_index;
    if (alignment > free_list->range_keys[used_range_index].alignment)
    {
        // Memory needs re-aligned; deallocate/allocate new range with new alignment.
        MoveToNewAllocation(free_list, used_range, used_range_index, reallocate_byte_size, alignment, mem,
                            &reallocated_mem);
    }
    else if (reallocate_byte_size < used_range->byte_size)
    {
        uint32 new_free_space_byte_size = used_range->byte_size - reallocate_byte_size;

        // Resize used-range to reallocation-byte-size.
        used_range->byte_size = reallocate_byte_size;

        if (next_range_index == UINT32_MAX || !IsFreeRangeIndex(free_list, next_range_index))
        {
            // Next-range doesn't exist or is used.

            // Add new free-range at end of used-range's new byte-size to cover deallocated free space.
            uint32 new_free_range_index =
                AddFreeRange(free_list,
                             {
                                 .byte_index       = used_range->byte_index + reallocate_byte_size,
                                 .byte_size        = new_free_space_byte_size,
                                 .prev_range_index = used_range_index,
                                 .next_range_index = used_range->next_range_index,
                             });

            // Link new-free-range between used-range and used-range's next-range.
            LinkInsertedNextRange(free_list, used_range, new_free_range_index);
        }
        else
        {
            // Next range is free.

            // Move next-range back and expand it to cover deallocated free space, then update key with new byte-size.
            Range* next_free_range = &free_list->ranges[next_range_index];
            next_free_range->byte_index -= new_free_space_byte_size;
            next_free_range->byte_size  += new_free_space_byte_size;
            free_list->range_keys[next_range_index] =
            {
                .byte_index = next_free_range->byte_index,
                .byte_size  = next_free_range->byte_size,
            };
        }
    }
    else if (reallocate_byte_size > used_range->byte_size)
    {
        // Calculate how much new free space will be needed for reallocation; this will be used to check if neighboring
        // ranges can supply the required new free space.
        uint32 new_used_space_byte_size = reallocate_byte_size - used_range->byte_size;

        if (next_range_index != UINT32_MAX &&
            IsFreeRangeIndex(free_list, next_range_index) &&
            free_list->ranges[next_range_index].byte_size >= new_used_space_byte_size)
        {
            // Next range is free and has enough space to satisfy reallocation.

            // Check how to manage next free range.
            Range* next_free_range = &free_list->ranges[next_range_index];
            if (used_range->byte_size + next_free_range->byte_size == reallocate_byte_size)
            {
                // Next free range has exactly the required space for reallocation.

                // Link used-range to next-range's next-range, then remove next-range as it was merged into used-range.
                LinkNextRange(free_list, used_range, used_range_index, next_free_range->next_range_index);
                RemoveFreeRange(free_list, next_range_index);
            }
            else
            {
                // Next free range has more than the required space for reallocation.

                // Move next-range forward and shrink it, then update key with new byte-size.
                next_free_range->byte_index += new_used_space_byte_size;
                next_free_range->byte_size  -= new_used_space_byte_size;
                free_list->range_keys[next_range_index] =
                {
                    .byte_index = next_free_range->byte_index,
                    .byte_size  = next_free_range->byte_size,
                };
            }

            // Add new space to used-range.
            used_range->byte_size += new_used_space_byte_size;
        }
        else
        {
            // No neighboring ranges can satisfy required space for reallocation.
            MoveToNewAllocation(free_list, used_range, used_range_index, reallocate_byte_size, alignment, mem,
                                &reallocated_mem);
        }
    }
    else
    {
        // Reallocation byte-size is the same as current byte-size; no operations required.
    }

    return reallocated_mem;
}

/// Interface
////////////////////////////////////////////////////////////
uint8* FreeList_AllocateNZ(void* context, uint32 size, uint32 alignment)
{
    CTK_ASSERT(size > 0);

    // Allocate memory.
    return InternalAllocate((FreeList*)context, size, alignment);
}

uint8* FreeList_Allocate(void* context, uint32 size, uint32 alignment)
{
    CTK_ASSERT(size > 0);

    // Allocate and zero memory.
    uint8* allocated_mem = FreeList_AllocateNZ(context, size, alignment);
    memset(allocated_mem, 0, size);
    return allocated_mem;
}

uint8* FreeList_Reallocate(void* context, void* mem, uint32 new_size, uint32 alignment)
{
    CTK_ASSERT(new_size > 0);

    auto free_list = (FreeList*)context;

    // Find used-range belonging to mem.
    uint32 used_range_index = FindUsedRangeIndex(free_list, mem);
    if (used_range_index == UINT32_MAX)
    {
        CTK_FATAL("can't reallocate memory @ 0x%p; no used-range found for that memory", mem);
    }
    uint32 used_range_byte_size = free_list->ranges[used_range_index].byte_size;

    // Reallocate memory.
    uint8* reallocated_mem = InternalReallocate(free_list, used_range_index, new_size, alignment, (uint8*)mem);

    // Zero newly allocated memory in reallocated memory if it was expanded.
    if (new_size > used_range_byte_size)
    {
        memset(&reallocated_mem[used_range_byte_size], 0, new_size - used_range_byte_size);
    }

    return reallocated_mem;
}

uint8* FreeList_ReallocateNZ(void* context, void* mem, uint32 new_size, uint32 alignment)
{
    CTK_ASSERT(new_size > 0);

    auto free_list = (FreeList*)context;

    // Find used-range belonging to mem.
    uint32 used_range_index = FindUsedRangeIndex(free_list, mem);
    if (used_range_index == UINT32_MAX)
    {
        CTK_FATAL("can't reallocate memory @ 0x%p; no used-range found for that memory", mem);
    }

    // Reallocate memory.
    return InternalReallocate(free_list, used_range_index, new_size, alignment, (uint8*)mem);
}

void FreeList_Deallocate(void* context, void* mem)
{
    auto free_list = (FreeList*)context;

    // Find used-range belonging to mem and deallocate it.
    uint32 used_range_index = FindUsedRangeIndex(free_list, mem);
    if (used_range_index == UINT32_MAX)
    {
        CTK_FATAL("can't deallocate memory @ 0x%p; no used-range found for that memory", mem);
    }

    // Deallocate memory.
    InternalDeallocate(free_list, &free_list->ranges[used_range_index], used_range_index);
}

Allocator CreateFreeList(Allocator* parent, uint32 min_byte_size, FreeListInfo info)
{
    CTK_ASSERT(min_byte_size > 0);
    CTK_ASSERT(info.max_range_count > 0);

    // Max free-range count is half max-total-range-count if max range count is even: FUFU or UFUF
    // Or it is half + 1 if max-total-range-count is odd: FUFUF
    uint32 max_free_range_count = (info.max_range_count / 2) + ((info.max_range_count % 2) == 0 ? 0 : 1);

    // Range-data needs 1 range, and every available range in max-total-range-count can be allocated, so
    // max-used-range-count is the combintation of those totals.
    uint32 max_used_range_count = 1 + info.max_range_count;

    uint32 max_range_count = max_used_range_count + max_free_range_count;

    // Calculate byte sizes for range-data and free-space.
    uint32 ranges_byte_size     = SizeOf32<Range>()    * max_range_count;
    uint32 range_keys_byte_size = SizeOf32<RangeKey>() * max_range_count;
    uint32 range_data_byte_size = ranges_byte_size + range_keys_byte_size;
    uint32 free_space_byte_size = min_byte_size;

    // Init free list.
    auto free_list = Allocate<FreeList>(parent, 1);
    free_list->mem              = Allocate<uint8>(parent, range_data_byte_size + free_space_byte_size);
    free_list->byte_size        = range_data_byte_size + free_space_byte_size;
    free_list->used_range_count = 0;
    free_list->free_range_count = 0;
    free_list->max_range_count  = max_range_count;

    // Init range data.
    uint32 ranges_byte_index     = 0;
    uint32 range_keys_byte_index = ranges_byte_index + ranges_byte_size;

    free_list->ranges     = (Range*)   (&free_list->mem[ranges_byte_index]);
    free_list->range_keys = (RangeKey*)(&free_list->mem[range_keys_byte_index]);

    // Add range-data used-range and free-space free-range, then link them.
    uint32 range_data_byte_index = 0;
    uint32 free_space_byte_index = range_data_byte_index + range_data_byte_size;
    uint32 range_data_range_index =
        AddUsedRange(free_list,
                     {
                         .byte_index       = range_data_byte_index,
                         .byte_size        = range_data_byte_size,
                         .prev_range_index = UINT32_MAX,
                         .next_range_index = UINT32_MAX,
                     },
                     {
                         .mem_byte_index = 0,
                         .alignment      = (uint32)GetAlignment(free_list->mem),
                     });
    uint32 free_space_range_index =
        AddFreeRange(free_list,
                     {
                         .byte_index       = free_space_byte_index,
                         .byte_size        = free_space_byte_size,
                         .prev_range_index = UINT32_MAX,
                         .next_range_index = UINT32_MAX,
                     });
    free_list->ranges[range_data_range_index].next_range_index = free_space_range_index;
    free_list->ranges[free_space_range_index].prev_range_index = range_data_range_index;

    // Range-data range is first.
    free_list->first_range_index = range_data_range_index;

    Allocator free_list_alloc = {};
    free_list_alloc.parent       = parent;
    free_list_alloc.context      = free_list;
    free_list_alloc.type         = AllocatorType::FreeList;
    free_list_alloc.Allocate     = FreeList_Allocate;
    free_list_alloc.AllocateNZ   = FreeList_AllocateNZ;
    free_list_alloc.Reallocate   = FreeList_Reallocate;
    free_list_alloc.ReallocateNZ = FreeList_ReallocateNZ;
    free_list_alloc.Deallocate   = FreeList_Deallocate;
    return free_list_alloc;
}

void DestroyFreeList(Allocator* free_list_alloc)
{
    CTK_ASSERT(free_list_alloc->type == AllocatorType::FreeList);

    auto free_list = (FreeList*)free_list_alloc->context;
    Deallocate(free_list_alloc->parent, free_list->mem);
    *free_list = {};
}
