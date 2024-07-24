/// Data
////////////////////////////////////////////////////////////
struct RangeInfo
{
    uint32 byte_index;
    uint32 byte_size;
    bool   is_free;
};

// /// Utils
// ////////////////////////////////////////////////////////////
template<typename... Args>
void PrintRangeType(FreeList* free_list, uint32 range_index, const char* msg, Args... args)
{
    if (range_index == UINT32_MAX)
    {
        Print(OutputColor::SKY, msg, args...);
    }
    else if (IsFreeRangeIndex(free_list, range_index))
    {
        Print(OutputColor::GREEN, msg, args...);
    }
    else if (IsUsedRangeIndex(free_list, range_index))
    {
        Print(OutputColor::MAGENTA, msg, args...);
    }
    else
    {
        Print(OutputColor::RED, msg, args...);
    }
}

/// Interface
////////////////////////////////////////////////////////////
void PrintRangeSimple(RangeInfo range_info)
{
    if (range_info.byte_size == 0)
    {
        PrintLine();
        CTK_FATAL("can't print range: range size == 0");
    }

    if (range_info.is_free)
    {
        Print(OutputColor::GREEN, 'F');
        CTK_REPEAT(range_info.byte_size - 1)
        {
            Print('.');
        }
    }
    else
    {
        Print(OutputColor::MAGENTA, 'U');
        CTK_REPEAT(range_info.byte_size - 1)
        {
            Print('=');
        }
    }
}

void PrintAllRangesSimple(FreeList* free_list)
{
    uint32 range_index = free_list->first_range_index;
    while (range_index != UINT32_MAX)
    {
        Range* range = &free_list->ranges[range_index];
        PrintRangeSimple({
                             .byte_index = range->byte_index,
                             .byte_size  = range->byte_size,
                             .is_free    = IsFreeRangeIndex(free_list, range_index),
                         });
        range_index = range->next_range_index;
    }
    PrintLine();
}

void PrintAllRangesSimple(RangeInfo* ranges, uint32 count)
{
    CTK_ITER_PTR(range, ranges, count)
    {
        PrintRangeSimple(*range);
    }
    PrintLine();
}

void PrintAllRangesBytes(FreeList* free_list)
{
    uint32 range_index = free_list->first_range_index;
    while (range_index != UINT32_MAX)
    {
        Range* range = &free_list->ranges[range_index];

        // Print range info.
        Print('[');
        PrintRangeType(free_list, range_index, "%6u", range->byte_index);
        Print(']');

        // Print range bytes.
        uint32 range_byte_index = range->byte_index;
        for (uint32 i = 0; i < range->byte_size; i += 1)
        {
            PrintASCIICharSingle((char)free_list->mem[range_byte_index + i], '.');
        }

        range_index = range->next_range_index;
    }
    PrintLine();
}

void PrintAllRangesByteValues(FreeList* free_list)
{
    uint32 range_index = free_list->first_range_index;
    while (range_index != UINT32_MAX)
    {
        Range* range = &free_list->ranges[range_index];

        // Print range info.
        Print('[');
        PrintRangeType(free_list, range_index, "%6u", range->byte_index);
        Print(']');

        // Print range chunk bytes.
        uint32 range_byte_index = range->byte_index;
        for (uint32 i = 0; i < range->byte_size; i += 1)
        {
            Print("\\%03u", free_list->mem[range_byte_index + i]);
        }

        range_index = range->next_range_index;
    }
    PrintLine();
}

void PrintRange(FreeList* free_list, uint32 range_index)
{
    if (IsFreeRangeIndex(free_list, range_index))
    {
        PrintLine(OutputColor::GREEN, "Free Range");
    }
    else
    {
        PrintLine(OutputColor::MAGENTA, "Used Range");
    }
    Range* range = free_list->ranges + range_index;
    Print("range_index:      ");
    PrintRangeType(free_list, range_index, "%u", range_index);
    PrintLine();
    PrintLine("byte_index:       %u", range->byte_index);
    PrintLine("byte_size:        %u", range->byte_size);
    Print("prev_range_index: ");
    PrintRangeType(free_list, range->prev_range_index, "%u", range->prev_range_index);
    PrintLine();
    Print("next_range_index: ");
    PrintRangeType(free_list, range->next_range_index, "%u", range->next_range_index);
    PrintLine();
    uint8* range_mem = GetRangeMem(free_list, range->byte_index);
    Print("mem:              ");
    // if (!IsInFreeList(free_list, range_mem))
    // {
    //     PrintLine(OutputColor::RED, "0x%p", range_mem);
    // }
    // else
    {
        PrintLine("0x%p", range_mem);
    }
}

void PrintAllRanges(FreeList* free_list)
{
    uint32 range_index = free_list->first_range_index;
    while (range_index != UINT32_MAX)
    {
        PrintRange(free_list, range_index);
        range_index = free_list->ranges[range_index].next_range_index;
    }
    PrintLine();
}

void PrintRangeKeys(FreeList* free_list)
{
    PrintLine(OutputColor::MAGENTA, "Used Ranges (count=%u):", free_list->used_range_count);
    for (uint32 i = 0; i < free_list->used_range_count; i += 1)
    {
        RangeKey* range_key = &free_list->range_keys[i];
        PrintLine("[%3u] mem_byte_index: %u", i, range_key->mem_byte_index);
    }

    PrintLine(OutputColor::GREEN, "Free Ranges (count=%u):", free_list->free_range_count);
    for (uint32 i = GetFreeRangesFirstIndex(free_list); i < free_list->max_range_count; i += 1)
    {
        RangeKey* range_key = &free_list->range_keys[i];
        PrintLine("[%3u] byte_index: %u", i, range_key->byte_index);
        PrintLine("      byte_size:  %u", range_key->byte_size);
    }
}

void PrintUsage(FreeList* free_list)
{
    uint32 used_byte_size  = 0;
    uint32 free_byte_size  = 0;
    uint32 meta_byte_size  = 0;
    uint32 total_byte_size = free_list->byte_size;

    // Count range data byte size.
    Range* range_data_range = free_list->ranges + free_list->first_range_index;
    meta_byte_size = range_data_range->byte_size;

    uint32 range_index = range_data_range->next_range_index;
    while (range_index != UINT32_MAX)
    {
        Range* range = &free_list->ranges[range_index];
        if (IsFreeRangeIndex(free_list, range_index))
        {
            free_byte_size += range->byte_size;
        }
        else
        {
            used_byte_size += range->byte_size;
        }

        range_index = range->next_range_index;
    }

    uint32 unaccounted_byte_size = total_byte_size - meta_byte_size - used_byte_size - free_byte_size;
    PrintLine("total: %u", total_byte_size);
    PrintLine("meta:  %u (%.1f%%)", meta_byte_size, 100 * ((float32)meta_byte_size / (float32)total_byte_size));
    PrintLine("used:  %u (%.1f%%)", used_byte_size, 100 * ((float32)used_byte_size / (float32)total_byte_size));
    PrintLine("free:  %u (%.1f%%)", free_byte_size, 100 * ((float32)free_byte_size / (float32)total_byte_size));
    PrintLine("????:  %u (%.1f%%)", unaccounted_byte_size, 100 * ((float32)unaccounted_byte_size / (float32)total_byte_size));
}

void PrintNeighborRanges(FreeList* free_list, uint32 range_index, uint32 neighbor_count)
{
    PrintLine("printing neighbors for:");

    PrintRange(free_list, range_index);

    PrintLine("\n=======================");
    uint32 start_range_index = range_index;
    uint32 prev_count = 0;
    for (uint32 i = 0; i < neighbor_count; i += 1)
    {
        uint32 prev_range_index = free_list->ranges[start_range_index].prev_range_index;
        if (prev_range_index == UINT32_MAX)
        {
            break;
        }

        prev_count += 1;
        start_range_index = prev_range_index;
    }

    uint32 total_count = prev_count + 1 + neighbor_count;
    uint32 curr_range_index = start_range_index;
    for (uint32 i = 0; i < total_count; i += 1)
    {
        PrintRange(free_list, curr_range_index);
        curr_range_index = free_list->ranges[curr_range_index].next_range_index;
        if (curr_range_index == UINT32_MAX)
        {
            break;
        }
    }
    PrintLine("=======================\n");
}

void ValidateRanges(FreeList* free_list)
{
    for (uint32 range_index = 0; range_index < free_list->used_range_count; range_index += 1)
    {
        Range* range = &free_list->ranges[range_index];
        uint32 next_range_byte_index = range->next_range_index == UINT32_MAX
                                        ? free_list->byte_size
                                        : free_list->ranges[range->next_range_index].byte_index;
        if (range->byte_index + range->byte_size != next_range_byte_index)
        {
            PrintNeighborRanges(free_list, range_index, 1);
            CTK_FATAL("free-list validation failed: range (index=%u) has byte_index %u and byte_size %u which "
                      "exceeds next range's (index=%u) byte_index %u",
                      range_index, range->byte_index, range->byte_size, range->next_range_index,
                      next_range_byte_index);
        }

        if (range->prev_range_index != UINT32_MAX &&
            free_list->ranges[range->prev_range_index].next_range_index != range_index)
        {
            PrintNeighborRanges(free_list, range_index, 1);
            CTK_FATAL("free-list validation failed: range's (index=%u) prev-range is index %u, but "
                      "prev-range's next-range is index %u",
                      range_index, range->prev_range_index,
                      free_list->ranges[range->prev_range_index].next_range_index);
        }

        if (range->next_range_index != UINT32_MAX &&
            free_list->ranges[range->next_range_index].prev_range_index != range_index)
        {
            PrintNeighborRanges(free_list, range_index, 1);
            CTK_FATAL("free-list validation failed: range's (index=%u) next-range is index %u, but "
                      "next-range's prev-range is index %u",
                      range_index, range->next_range_index,
                      free_list->ranges[range->next_range_index].prev_range_index);
        }
    }
    for (uint32 range_index = GetFreeRangesFirstIndex(free_list); range_index < free_list->max_range_count;
         range_index += 1)
    {
        Range* range = &free_list->ranges[range_index];
        uint32 next_range_byte_index = range->next_range_index == UINT32_MAX
                                        ? free_list->byte_size
                                        : free_list->ranges[range->next_range_index].byte_index;
        if (range->byte_index + range->byte_size != next_range_byte_index)
        {
            PrintNeighborRanges(free_list, range_index, 1);
            CTK_FATAL("free-list validation failed: range (index=%u) has byte_index %u and byte_size %u which "
                      "exceeds next range's (index=%u) byte_index %u",
                      range_index, range->byte_index, range->byte_size, range->next_range_index,
                      next_range_byte_index);
        }

        if (range->prev_range_index != UINT32_MAX &&
            free_list->ranges[range->prev_range_index].next_range_index != range_index)
        {
            PrintNeighborRanges(free_list, range_index, 1);
            CTK_FATAL("free-list validation failed: range's (index=%u) prev-range is index %u, but "
                      "prev-range's next-range is index %u",
                      range_index, range->prev_range_index,
                      free_list->ranges[range->prev_range_index].next_range_index);
        }

        if (range->next_range_index != UINT32_MAX &&
            free_list->ranges[range->next_range_index].prev_range_index != range_index)
        {
            PrintNeighborRanges(free_list, range_index, 1);
            CTK_FATAL("free-list validation failed: range's (index=%u) next-range is index %u, but "
                      "next-range's prev-range is index %u",
                      range_index, range->next_range_index,
                      free_list->ranges[range->next_range_index].prev_range_index);
        }
    }
}

