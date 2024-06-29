#pragma once

namespace FreeListTest
{

/// Data
////////////////////////////////////////////////////////////
static constexpr uint32 MAX_RANGE_COUNT       = 9; // Required for double merge test.
static constexpr uint32 MAX_FREE_RANGE_COUNT  = (MAX_RANGE_COUNT / 2) + ((MAX_RANGE_COUNT % 2) == 0 ? 0 : 1);
static constexpr uint32 MAX_USED_RANGE_COUNT  = 1 + MAX_RANGE_COUNT;
static constexpr uint32 MAX_TOTAL_RANGE_COUNT = MAX_USED_RANGE_COUNT + MAX_FREE_RANGE_COUNT;

static constexpr uint32 RANGES_BYTE_SIZE     = SizeOf32<Range>()    * MAX_TOTAL_RANGE_COUNT;
static constexpr uint32 RANGE_KEYS_BYTE_SIZE = SizeOf32<RangeKey>() * MAX_TOTAL_RANGE_COUNT;

static constexpr uint32 RANGE_DATA_BYTE_SIZE = RANGES_BYTE_SIZE + RANGE_KEYS_BYTE_SIZE;

static constexpr uint32 RANGE_DATA_BYTE_INDEX = 0;
static constexpr uint32 FREE_SPACE_BYTE_INDEX = RANGE_DATA_BYTE_INDEX + RANGE_DATA_BYTE_SIZE;

static constexpr RangeInfo RANGE_DATA_RANGE_INFO =
{
    .byte_index = RANGE_DATA_BYTE_INDEX,
    .byte_size  = RANGE_DATA_BYTE_SIZE,
    .is_free    = false,
};

/// Utils
////////////////////////////////////////////////////////////
static bool ExpectLayout(FreeList* free_list, Array<RangeInfo> layout)
{
    bool pass = true;
    uint32 range_info_index = 0;
    uint32 range_index = free_list->first_range_index;
    uint32 range_count = 0;
    while (range_index != UINT32_MAX)
    {
        ++range_count;
        Range* range = free_list->ranges + range_index;

        // Only test range if there are any left in layout.
        if (range_info_index < layout.count)
        {
            RangeInfo* range_info = GetPtr(&layout, range_info_index);

            if (range->byte_size != range_info->byte_size)
            {
                pass = false;
                PrintExpectedTag();
                Print("range at index ");
                PrintRangeType(free_list, range_index, "%u", range_index);
                PrintLine(" to have byte-size %u", range_info->byte_size);
                PrintActual(false, "was byte-size %u", range->byte_size);
            }

            if (range->byte_index != range_info->byte_index)
            {
                pass = false;
                PrintExpectedTag();
                Print("range at index ");
                PrintRangeType(free_list, range_index, "%u", range_index);
                PrintLine(" to be at byte-index %u", range_info->byte_size);
                PrintActual(false, "was at byte-index %u", range->byte_index);
            }

            if (!range_info->is_free && IsFreeRangeIndex(free_list, range_index))
            {
                pass = false;
                PrintExpected("range at index %u to be used", range_index);
                PrintActual(false, "was free");
            }
            else if (range_info->is_free && !IsFreeRangeIndex(free_list, range_index))
            {
                pass = false;
                PrintExpected("range at index %u to be free", range_index);
                PrintActual(false, "was used");
            }

            ++range_info_index;
        }

        range_index = range->next_range_index;
    }

    if (layout.count != range_count)
    {
        pass = false;
        PrintExpected("free_list to have %u ranges", layout.count);
        PrintActual(false, "found %u ranges", range_count);
    }

    PrintExpectedTag();
    PrintLine();
    PrintAllRangesSimple(layout.data, layout.count);
    PrintLine();

    PrintActualTag(pass);
    PrintLine();
    PrintAllRangesSimple(free_list);
    PrintLine();

    ValidateRanges(free_list);

    return pass;
}

static void FreeLayoutRange(Array<RangeInfo>* expected_layout, uint32 alloc_index)
{
    static constexpr uint32 FREE_SPACE_RANGE_INDEX_START = 1;
    uint32 range_index = FREE_SPACE_RANGE_INDEX_START + alloc_index;
    RangeInfo* range = GetPtr(expected_layout, range_index);
    range->is_free = true;

    uint32 next_range_index = range_index + 1;
    if (next_range_index < expected_layout->size)
    {
        RangeInfo* next_range = GetPtr(expected_layout, next_range_index);
        if (next_range->is_free)
        {
            range->byte_size += next_range->byte_size;
            Remove(expected_layout, next_range_index);
        }
    }

    uint32 prev_range_index = range_index - 1;
    if (prev_range_index >= FREE_SPACE_RANGE_INDEX_START)
    {
        RangeInfo* prev_range = GetPtr(expected_layout, prev_range_index);
        if (prev_range->is_free)
        {
            prev_range->byte_size += range->byte_size;
            Remove(expected_layout, range_index);
        }
    }
}

/// Tests
////////////////////////////////////////////////////////////
static bool AllocateMultiple()
{
    bool pass = true;

    static constexpr uint32 FREE_LIST_BYTE_SIZE = 16;
    static_assert(FREE_LIST_BYTE_SIZE > 14); // Must have enough room to hold test_allocs.
    FreeList free_list = CreateFreeList(&std_allocator, FREE_LIST_BYTE_SIZE, { MAX_RANGE_COUNT });
    uint32 test_allocs[] = { 8, 4, 2, 1, 1 };

    {
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            {
                .byte_index = FREE_SPACE_BYTE_INDEX,
                .byte_size  = FREE_LIST_BYTE_SIZE,
                .is_free    = true
            },
        };
        RunTest("Initial Layout", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }

    for (uint32 curr_alloc_index = 0; curr_alloc_index < CTK_ARRAY_SIZE(test_allocs); ++curr_alloc_index)
    {
        auto layout = CreateArray<RangeInfo>(&std_allocator, FREE_LIST_BYTE_SIZE);
        Push(&layout, RANGE_DATA_RANGE_INFO);
        uint32 byte_index = FREE_SPACE_BYTE_INDEX;
        uint32 byte_size = 0;
        uint32 remaining_byte_size = FREE_LIST_BYTE_SIZE;

        // Push previous allocations' used-ranges to layout.
        for (uint32 prev_alloc_index = 0; prev_alloc_index < curr_alloc_index; ++prev_alloc_index)
        {
            byte_size = test_allocs[prev_alloc_index];
            Push(&layout, { .byte_index = byte_index, .byte_size = byte_size, .is_free = false });
            byte_index += byte_size;
            remaining_byte_size -= byte_size;
        }

        // Push current allocation's used-range to layout.
        byte_size = test_allocs[curr_alloc_index];
        Push(&layout, { .byte_index = byte_index, .byte_size = byte_size, .is_free = false });
        byte_index += byte_size;
        remaining_byte_size -= byte_size;

        // If there is space left over, push free-range to layout to cover that space.
        if (remaining_byte_size > 0)
        {
            Push(&layout, { .byte_index = byte_index, .byte_size = remaining_byte_size, .is_free = true });
        }

        Allocate<uint8>(&free_list, test_allocs[curr_alloc_index]);
        FString<256> description = {};
        Write(&description, "Allocate<uint8>(&free_list, %u)", test_allocs[curr_alloc_index]);
        RunTest(&description, &pass, ExpectLayout, &free_list, layout);

        DestroyArray(&layout, &std_allocator);
    }

    DestroyFreeList(&free_list, &std_allocator);
    return pass;
}

static bool AllocateFullFreeList()
{
    bool pass = true;

    static constexpr uint32 FREE_LIST_BYTE_SIZE = 16;
    FreeList free_list = CreateFreeList(&std_allocator, FREE_LIST_BYTE_SIZE, { MAX_RANGE_COUNT });
    uint8* alloc = NULL;

    {
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            {
                .byte_index = FREE_SPACE_BYTE_INDEX,
                .byte_size  = FREE_LIST_BYTE_SIZE,
                .is_free    = true
            },
        };
        RunTest("Initial Layout", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }

    {
        alloc = Allocate<uint8>(&free_list, FREE_LIST_BYTE_SIZE);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            {
                .byte_index = FREE_SPACE_BYTE_INDEX,
                .byte_size  = FREE_LIST_BYTE_SIZE,
                .is_free    = false
            },
        };
        FString<256> description = {};
        Write(&description, "Allocate<uint8>(&free_list, %u)", FREE_LIST_BYTE_SIZE);
        RunTest(&description, &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        Deallocate(&free_list, alloc);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            {
                .byte_index = FREE_SPACE_BYTE_INDEX,
                .byte_size  = FREE_LIST_BYTE_SIZE,
                .is_free    = true
            },
        };
        RunTest("Deallocate(&free_list, alloc)", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }

    DestroyFreeList(&free_list, &std_allocator);
    return pass;
}

static bool DeallocateDoubleMerge()
{
    bool pass = true;

    static constexpr uint32 FREE_LIST_BYTE_SIZE = 60;
    FreeList free_list = CreateFreeList(&std_allocator, FREE_LIST_BYTE_SIZE, { MAX_RANGE_COUNT });
    uint8* allocs[] =
    {
        Allocate<uint8>(&free_list, 12),
        Allocate<uint8>(&free_list, 8),
        Allocate<uint8>(&free_list, 4),
        Allocate<uint8>(&free_list, 2),
        Allocate<uint8>(&free_list, 16),
        Allocate<uint8>(&free_list, 1),
        Allocate<uint8>(&free_list, 6),
        Allocate<uint8>(&free_list, 1),
    };

    RangeInfo layout[] =
    {
        RANGE_DATA_RANGE_INFO,
        { .byte_index = FREE_SPACE_BYTE_INDEX + 0,  .byte_size = 12, .is_free = false },
        { .byte_index = FREE_SPACE_BYTE_INDEX + 12, .byte_size = 8,  .is_free = false },
        { .byte_index = FREE_SPACE_BYTE_INDEX + 20, .byte_size = 4,  .is_free = false },
        { .byte_index = FREE_SPACE_BYTE_INDEX + 24, .byte_size = 2,  .is_free = false },
        { .byte_index = FREE_SPACE_BYTE_INDEX + 26, .byte_size = 16, .is_free = false },
        { .byte_index = FREE_SPACE_BYTE_INDEX + 42, .byte_size = 1,  .is_free = false },
        { .byte_index = FREE_SPACE_BYTE_INDEX + 43, .byte_size = 6,  .is_free = false },
        { .byte_index = FREE_SPACE_BYTE_INDEX + 49, .byte_size = 1,  .is_free = false },
        { .byte_index = FREE_SPACE_BYTE_INDEX + 50, .byte_size = 10, .is_free = true  },
    };
    Array<RangeInfo> expected_layout = CTK_WRAP_ARRAY(layout);
    RunTest("Initial Layout", &pass, ExpectLayout, &free_list, expected_layout);

    // Evens
    for (uint32 alloc_index = 0; alloc_index < CTK_ARRAY_SIZE(allocs); alloc_index += 2)
    {
        // Deallocate and update expected layout.
        Deallocate(&free_list, allocs[alloc_index]);
        FreeLayoutRange(&expected_layout, alloc_index);

        // Run test.
        FString<256> description = {};
        Write(&description, "Deallocate(&free_list, allocs[%u])", alloc_index);
        RunTest(&description, &pass, ExpectLayout, &free_list, expected_layout);
    }

    // Odds
    for (uint32 alloc_index = 1; alloc_index < CTK_ARRAY_SIZE(allocs); alloc_index += 2)
    {
        // Deallocate and update expected layout.
        Deallocate(&free_list, allocs[alloc_index]);
        FreeLayoutRange(&expected_layout, 1);

        // Run test.
        FString<256> description = {};
        Write(&description, "Deallocate(&free_list, allocs[%u])", alloc_index);
        RunTest(&description, &pass, ExpectLayout, &free_list, expected_layout);
    }

    DestroyFreeList(&free_list, &std_allocator);
    return pass;
}

static bool DeallocatePrevMerge()
{
    bool pass = true;

    static constexpr uint32 FREE_LIST_BYTE_SIZE = 60;
    FreeList free_list = CreateFreeList(&std_allocator, FREE_LIST_BYTE_SIZE, { MAX_RANGE_COUNT });
    uint8* allocs[] =
    {
        Allocate<uint8>(&free_list, 24),
        Allocate<uint8>(&free_list, 16),
        Allocate<uint8>(&free_list, 8),
        Allocate<uint8>(&free_list, 4),
    };

    {
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0,  .byte_size = 24, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 24, .byte_size = 16, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 40, .byte_size = 8,  .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 48, .byte_size = 4,  .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 52, .byte_size = 8,  .is_free = true  },
        };
        RunTest("Initial Layout", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        Deallocate(&free_list, allocs[0]);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0,  .byte_size = 24, .is_free = true  },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 24, .byte_size = 16, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 40, .byte_size = 8,  .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 48, .byte_size = 4,  .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 52, .byte_size = 8,  .is_free = true  },
        };
        RunTest("Deallocate(&free_list, allocs[0])", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        Deallocate(&free_list, allocs[1]);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0,  .byte_size = 40, .is_free = true },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 40, .byte_size = 8,  .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 48, .byte_size = 4,  .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 52, .byte_size = 8,  .is_free = true  },
        };
        RunTest("Deallocate(&free_list, allocs[1])", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }

    DestroyFreeList(&free_list, &std_allocator);
    return pass;
}

static bool DeallocateNextMerge()
{
    bool pass = true;

    static constexpr uint32 FREE_LIST_BYTE_SIZE = 60;
    FreeList free_list = CreateFreeList(&std_allocator, FREE_LIST_BYTE_SIZE, { MAX_RANGE_COUNT });
    uint8* allocs[] =
    {
        Allocate<uint8>(&free_list, 24),
        Allocate<uint8>(&free_list, 16),
        Allocate<uint8>(&free_list, 8),
        Allocate<uint8>(&free_list, 4),
    };

    {
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0,  .byte_size = 24, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 24, .byte_size = 16, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 40, .byte_size = 8,  .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 48, .byte_size = 4,  .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 52, .byte_size = 8,  .is_free = true  },
        };
        RunTest("Initial Layout", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        Deallocate(&free_list, allocs[1]);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0,  .byte_size = 24, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 24, .byte_size = 16, .is_free = true  },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 40, .byte_size = 8,  .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 48, .byte_size = 4,  .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 52, .byte_size = 8,  .is_free = true  },
        };
        RunTest("Deallocate(&free_list, allocs[1])", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        Deallocate(&free_list, allocs[0]);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0,  .byte_size = 40, .is_free = true  },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 40, .byte_size = 8,  .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 48, .byte_size = 4,  .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 52, .byte_size = 8,  .is_free = true  },
        };
        RunTest("Deallocate(&free_list, allocs[0])", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }

    DestroyFreeList(&free_list, &std_allocator);
    return pass;
}

static bool ReallocateSameSize()
{
    bool pass = true;

    static constexpr uint32 FREE_LIST_BYTE_SIZE = 4;
    FreeList free_list = CreateFreeList(&std_allocator, FREE_LIST_BYTE_SIZE, { MAX_RANGE_COUNT });
    uint8* alloc = NULL;

    {
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 4, .is_free = true  },
        };
        RunTest("Initial Layout", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        alloc = Allocate<uint8>(&free_list, 1);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 1, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 1, .byte_size = 3, .is_free = true  },
        };
        RunTest("Allocate<uint8>(&free_list, 1)", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        alloc = Reallocate(&free_list, alloc, 1);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 1, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 1, .byte_size = 3, .is_free = true  },
        };
        RunTest("Reallocate(&free_list, alloc, 1)", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }

    DestroyFreeList(&free_list, &std_allocator);
    return pass;
}

static bool ReallocateSmallerNoNextHeader()
{
    bool pass = true;

    static constexpr uint32 FREE_LIST_BYTE_SIZE = 5;
    FreeList free_list = CreateFreeList(&std_allocator, FREE_LIST_BYTE_SIZE, { MAX_RANGE_COUNT });
    uint8* alloc = NULL;

    {
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 5, .is_free = true  },
        };
        RunTest("Initial Layout", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        alloc = Allocate<uint8>(&free_list, 5);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 5, .is_free = false },
        };
        RunTest("Allocate<uint8>(&free_list, 5)", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        alloc = Reallocate(&free_list, alloc, 3);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 3, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 3, .byte_size = 2, .is_free = true  },
        };
        RunTest("Reallocate(&free_list, alloc, 3)", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        Allocate<uint8>(&free_list, 2);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 3, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 3, .byte_size = 2, .is_free = false },
        };
        RunTest("Allocate<uint8>(&free_list, 2)", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }

    DestroyFreeList(&free_list, &std_allocator);
    return pass;
}

static bool ReallocateSmallerNextHeaderIsUsed()
{
    bool pass = true;

    static constexpr uint32 FREE_LIST_BYTE_SIZE = 5;
    FreeList free_list = CreateFreeList(&std_allocator, FREE_LIST_BYTE_SIZE, { MAX_RANGE_COUNT });
    uint8* alloc = NULL;

    {
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 5, .is_free = true  },
        };
        RunTest("Initial Layout", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        alloc = Allocate<uint8>(&free_list, 3);
        Allocate<uint8>(&free_list, 2);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 3, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 3, .byte_size = 2, .is_free = false },
        };
        RunTest("Allocate<uint8>(&free_list, 3); Allocate<uint8>(&free_list, 2)", &pass,
                ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        alloc = Reallocate(&free_list, alloc, 1);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 1, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 1, .byte_size = 2, .is_free = true  },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 3, .byte_size = 2, .is_free = false },
        };
        RunTest("Reallocate(&free_list, alloc, 1)", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }

    DestroyFreeList(&free_list, &std_allocator);
    return pass;
}

static bool ReallocateSmallerNextHeaderIsFree()
{
    bool pass = true;

    static constexpr uint32 FREE_LIST_BYTE_SIZE = 5;
    FreeList free_list = CreateFreeList(&std_allocator, FREE_LIST_BYTE_SIZE, { MAX_RANGE_COUNT });
    uint8* alloc = NULL;

    {
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 5, .is_free = true  },
        };
        RunTest("Initial Layout", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        alloc = Allocate<uint8>(&free_list, 5);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 5, .is_free = false },
        };
        RunTest("Allocate<uint8>(&free_list, 5)", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        alloc = Reallocate(&free_list, alloc, 4);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 4, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 4, .byte_size = 1, .is_free = true  },
        };
        RunTest("Reallocate(&free_list, alloc, 4)", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        alloc = Reallocate(&free_list, alloc, 3);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 3, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 3, .byte_size = 2, .is_free = true  },
        };
        RunTest("Reallocate(&free_list, alloc, 3)", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        alloc = Reallocate(&free_list, alloc, 2);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 2, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 2, .byte_size = 3, .is_free = true  },
        };
        RunTest("Reallocate(&free_list, alloc, 2)", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        alloc = Reallocate(&free_list, alloc, 1);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 1, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 1, .byte_size = 4, .is_free = true  },
        };
        RunTest("Reallocate(&free_list, alloc, 1)", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        Allocate<uint8>(&free_list, 2);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 1, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 1, .byte_size = 2, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 3, .byte_size = 2, .is_free = true  },
        };
        RunTest("Allocate<uint8>(&free_list, 2)", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }

    DestroyFreeList(&free_list, &std_allocator);
    return pass;
}

static bool ReallocateLargerMergeEntireNextFreeRange()
{
    bool pass = true;

    static constexpr uint32 FREE_LIST_BYTE_SIZE = 6;
    FreeList free_list = CreateFreeList(&std_allocator, FREE_LIST_BYTE_SIZE, { MAX_RANGE_COUNT });
    uint8* allocs[] =
    {
        Allocate<uint8>(&free_list, 3),
        Allocate<uint8>(&free_list, 2),
        Allocate<uint8>(&free_list, 1),
    };
    Deallocate(&free_list, allocs[1]);

    {
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 3, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 3, .byte_size = 2, .is_free = true  },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 5, .byte_size = 1, .is_free = false },
        };
        RunTest("Initial Layout", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        allocs[0] = Reallocate(&free_list, allocs[0], 5);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 5, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 5, .byte_size = 1, .is_free = false },
        };
        RunTest("Reallocate(&free_list, alloc, 6)", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }

    DestroyFreeList(&free_list, &std_allocator);
    return pass;
}

static bool ReallocateLargerMergePartialNextFreeRange()
{
    bool pass = true;

    static constexpr uint32 FREE_LIST_BYTE_SIZE = 6;
    FreeList free_list = CreateFreeList(&std_allocator, FREE_LIST_BYTE_SIZE, { MAX_RANGE_COUNT });
    uint8* alloc = NULL;

    {
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 6, .is_free = true  },
        };
        RunTest("Initial Layout", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        alloc = Allocate<uint8>(&free_list, 1);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 1, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 1, .byte_size = 5, .is_free = true  },
        };
        RunTest("Allocate<uint8>(&free_list, 1)", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        alloc = Reallocate(&free_list, alloc, 2);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 2, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 2, .byte_size = 4, .is_free = true  },
        };
        RunTest("Reallocate(&free_list, alloc, 2)", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        alloc = Reallocate(&free_list, alloc, 3);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 3, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 3, .byte_size = 3, .is_free = true  },
        };
        RunTest("Reallocate(&free_list, alloc, 3)", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        alloc = Reallocate(&free_list, alloc, 4);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 4, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 4, .byte_size = 2, .is_free = true  },
        };
        RunTest("Reallocate(&free_list, alloc, 4)", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        alloc = Reallocate(&free_list, alloc, 5);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 5, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 5, .byte_size = 1, .is_free = true  },
        };
        RunTest("Reallocate(&free_list, alloc, 5)", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        alloc = Reallocate(&free_list, alloc, 6);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 6, .is_free = false },
        };
        RunTest("Reallocate(&free_list, alloc, 6)", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }

    DestroyFreeList(&free_list, &std_allocator);
    return pass;
}

static bool ReallocateLargerNewAllocationPostRange()
{
    bool pass = true;

    static constexpr uint32 FREE_LIST_BYTE_SIZE = 4;
    FreeList free_list = CreateFreeList(&std_allocator, FREE_LIST_BYTE_SIZE, { MAX_RANGE_COUNT });
    uint8* allocs[] =
    {
        Allocate<uint8>(&free_list, 1),
        Allocate<uint8>(&free_list, 1),
    };

    {
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 1, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 1, .byte_size = 1, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 2, .byte_size = 2, .is_free = true  },
        };
        RunTest("Initial Layout", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        allocs[0] = Reallocate(&free_list, allocs[0], 2);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 1, .is_free = true  },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 1, .byte_size = 1, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 2, .byte_size = 2, .is_free = false },
        };
        RunTest("Reallocate(&free_list, allocs[0], 2)", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }

    DestroyFreeList(&free_list, &std_allocator);
    return pass;
}

static bool ReallocateLargerNewAllocationPreRange()
{
    bool pass = true;

    static constexpr uint32 FREE_LIST_BYTE_SIZE = 4;
    FreeList free_list = CreateFreeList(&std_allocator, FREE_LIST_BYTE_SIZE, { MAX_RANGE_COUNT });
    uint8* allocs[] =
    {
        Allocate<uint8>(&free_list, 2),
        Allocate<uint8>(&free_list, 1),
        Allocate<uint8>(&free_list, 1),
    };
    Deallocate(&free_list, allocs[0]);

    {
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 2, .is_free = true  },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 2, .byte_size = 1, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 3, .byte_size = 1, .is_free = false },
        };
        RunTest("Initial Layout", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        allocs[2] = Reallocate(&free_list, allocs[2], 2);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 2, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 2, .byte_size = 1, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 3, .byte_size = 1, .is_free = true  },
        };
        RunTest("Reallocate(&free_list, allocs[2], 2)", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }

    DestroyFreeList(&free_list, &std_allocator);
    return pass;
}

static bool ZeroAllocatedMemory()
{
    bool pass = true;

    static constexpr uint32 FREE_LIST_BYTE_SIZE = 4;
    FreeList free_list = CreateFreeList(&std_allocator, FREE_LIST_BYTE_SIZE, { MAX_RANGE_COUNT });

    // Set all free memory in free-range to non-zero values.
    uint8* free_space_mem = GetRangeMem(&free_list, free_list.ranges[GetFreeRangesFirstIndex(&free_list)].byte_index);
    memset(free_space_mem, UINT8_MAX, FREE_LIST_BYTE_SIZE);

    {
        static constexpr uint32 ALLOC_SIZE = 1;
        char test_buf[ALLOC_SIZE] = {};
        FString<256> description = {};

        char* alloc = Allocate<char>(&free_list, ALLOC_SIZE);
        Write(&description, "Allocate<char>(&free_list, %u)", ALLOC_SIZE);
        RunTest(&description, &pass,
                ExpectEqual,
                (const char*)test_buf, ALLOC_SIZE,
                (const char*)alloc,    ALLOC_SIZE);

        Deallocate(&free_list, alloc);
        RunTest("Deallocate(&free_list, alloc) (Deallocate() doesn't zero memory)", &pass,
                ExpectEqual,
                (const char*)test_buf, ALLOC_SIZE,
                (const char*)alloc,    ALLOC_SIZE);
    }
    {
        static constexpr uint32 ALLOC_SIZE = FREE_LIST_BYTE_SIZE;
        char test_buf[FREE_LIST_BYTE_SIZE] = {};
        FString<256> description = {};

        char* alloc = Allocate<char>(&free_list, ALLOC_SIZE);
        Write(&description, "Allocate<char>(&free_list, %u)", ALLOC_SIZE);
        RunTest(&description, &pass, ExpectEqual,
                (const char*)test_buf, ALLOC_SIZE,
                (const char*)alloc,    ALLOC_SIZE);

        Deallocate(&free_list, alloc);
        RunTest("Deallocate(&free_list, alloc) doesn't zero memory", &pass,
                ExpectEqual,
                (const char*)test_buf, ALLOC_SIZE,
                (const char*)alloc,    ALLOC_SIZE);
    }

    DestroyFreeList(&free_list, &std_allocator);
    return pass;
}

static bool ZeroReallocatedMemory()
{
    bool pass = true;

    static constexpr uint32 FREE_LIST_BYTE_SIZE = 24;
    FreeList free_list = CreateFreeList(&std_allocator, FREE_LIST_BYTE_SIZE, { MAX_RANGE_COUNT });

    // Set all free memory in free-range to non-zero values.
    uint8* free_space_mem = GetRangeMem(&free_list, free_list.ranges[GetFreeRangesFirstIndex(&free_list)].byte_index);
    memset(free_space_mem, UINT8_MAX, FREE_LIST_BYTE_SIZE);

    static constexpr uint32 INIT_ALLOC_BYTE_SIZE = 8;
    char* allocs[3] =
    {
        Allocate<char>(&free_list, INIT_ALLOC_BYTE_SIZE),
        Allocate<char>(&free_list, INIT_ALLOC_BYTE_SIZE),
        Allocate<char>(&free_list, INIT_ALLOC_BYTE_SIZE),
    };
    Write(allocs[1], INIT_ALLOC_BYTE_SIZE, "1234567"); // Write to second allocation that will be reallocated into.
    Deallocate(&free_list, allocs[1]); // Deallocating second allocation should leave data untouched.

    {
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0,  .byte_size = 8, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 8,  .byte_size = 8, .is_free = true  },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 16, .byte_size = 8, .is_free = false },
        };
        RunTest("Initial Layout", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        FString<256> description = {};
        Write(&description, "Write(allocs[0], %u, \"test\")", INIT_ALLOC_BYTE_SIZE);

        char test_buf[INIT_ALLOC_BYTE_SIZE] = {};
        Write(test_buf, INIT_ALLOC_BYTE_SIZE, "test");
        Write(allocs[0], INIT_ALLOC_BYTE_SIZE, "test");
        RunTest(&description, &pass,
                ExpectEqual,
                (const char*)test_buf,  INIT_ALLOC_BYTE_SIZE,
                (const char*)allocs[0], INIT_ALLOC_BYTE_SIZE);
    }
    {
        static constexpr uint32 REALLOC_BYTE_SIZE = 12;
        FString<256> description = {};
        Write(&description, "Reallocate(&free_list, allocs[0], %u) partial merge next", REALLOC_BYTE_SIZE);

        allocs[0] = Reallocate(&free_list, allocs[0], REALLOC_BYTE_SIZE);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0,  .byte_size = 12, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 12, .byte_size = 4,  .is_free = true  },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 16, .byte_size = 8,  .is_free = false },
        };
        RunTest(&description, &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));

        char test_buf[REALLOC_BYTE_SIZE] = {};
        Write(test_buf, REALLOC_BYTE_SIZE, "test");
        RunTest(&description, &pass,
                ExpectEqual,
                (const char*)test_buf,  REALLOC_BYTE_SIZE,
                (const char*)allocs[0], REALLOC_BYTE_SIZE);
    }

    {
        static constexpr uint32 REALLOC_BYTE_SIZE = 16;
        FString<256> description = {};
        Write(&description, "Reallocate(&free_list, allocs[0], %u) full merge next", REALLOC_BYTE_SIZE);

        allocs[0] = Reallocate(&free_list, allocs[0], REALLOC_BYTE_SIZE);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0,  .byte_size = 16, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 16, .byte_size = 8,  .is_free = false },
        };
        RunTest(&description, &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));

        char test_buf[REALLOC_BYTE_SIZE] = {};
        Write(test_buf, REALLOC_BYTE_SIZE, "test");
        RunTest(&description, &pass,
                ExpectEqual,
                (const char*)test_buf,  REALLOC_BYTE_SIZE,
                (const char*)allocs[0], REALLOC_BYTE_SIZE);
    }

    DestroyFreeList(&free_list, &std_allocator);
    return pass;
}

static bool NonZeroAllocatedMemory()
{
    bool pass = true;

    static constexpr uint32 FREE_LIST_BYTE_SIZE = 4;
    FreeList free_list = CreateFreeList(&std_allocator, FREE_LIST_BYTE_SIZE, { MAX_RANGE_COUNT });

    // Set all free memory in free-range to non-zero values.
    uint8* free_space_mem = GetRangeMem(&free_list, free_list.ranges[GetFreeRangesFirstIndex(&free_list)].byte_index);
    memset(free_space_mem, '#', FREE_LIST_BYTE_SIZE);

    {
        static constexpr uint32 ALLOC_SIZE = 1;
        FString<256> description = {};

        // Allocated range should still contain default values.
        char test_buf[ALLOC_SIZE] = {};
        memset(test_buf, '#', ALLOC_SIZE);

        char* alloc = AllocateNZ<char>(&free_list, ALLOC_SIZE);
        Write(&description, "AllocateNZ<char>(&free_list, %u) (shouldn't zero memory)", ALLOC_SIZE);
        RunTest(&description, &pass,
                ExpectEqual,
                (const char*)test_buf, ALLOC_SIZE,
                (const char*)alloc,    ALLOC_SIZE);

        Deallocate(&free_list, alloc);
        RunTest("Deallocate(&free_list, alloc) (doesn't zero memory)", &pass,
                ExpectEqual,
                (const char*)test_buf, ALLOC_SIZE,
                (const char*)alloc,    ALLOC_SIZE);
    }
    {
        static constexpr uint32 ALLOC_SIZE = FREE_LIST_BYTE_SIZE;
        FString<256> description = {};

        // Allocated range should still contain default values.
        char test_buf[FREE_LIST_BYTE_SIZE] = {};
        memset(test_buf, '#', FREE_LIST_BYTE_SIZE);

        char* alloc = AllocateNZ<char>(&free_list, ALLOC_SIZE);
        Write(&description, "AllocateNZ<char>(&free_list, %u) (shouldn't zero memory)", ALLOC_SIZE);
        RunTest(&description, &pass, ExpectEqual,
                (const char*)test_buf, ALLOC_SIZE,
                (const char*)alloc,    ALLOC_SIZE);

        Deallocate(&free_list, alloc);
        RunTest("Deallocate(&free_list, alloc) (doesn't zero memory)", &pass,
                ExpectEqual,
                (const char*)test_buf, ALLOC_SIZE,
                (const char*)alloc,    ALLOC_SIZE);
    }

    DestroyFreeList(&free_list, &std_allocator);
    return pass;
}

static bool NonZeroReallocatedMemory()
{
    bool pass = true;

    static constexpr uint32 FREE_LIST_BYTE_SIZE = 6;
    FreeList free_list = CreateFreeList(&std_allocator, FREE_LIST_BYTE_SIZE, { MAX_RANGE_COUNT });

    // Set all free memory in free-range to non-zero values.
    uint8* free_space_mem = GetRangeMem(&free_list, free_list.ranges[GetFreeRangesFirstIndex(&free_list)].byte_index);
    memset(free_space_mem, '#', FREE_LIST_BYTE_SIZE);

    static constexpr uint32 INIT_ALLOC_BYTE_SIZE = 2;
    char* allocs[3] =
    {
        AllocateNZ<char>(&free_list, INIT_ALLOC_BYTE_SIZE),
        AllocateNZ<char>(&free_list, INIT_ALLOC_BYTE_SIZE),
        AllocateNZ<char>(&free_list, INIT_ALLOC_BYTE_SIZE),
    };
    Deallocate(&free_list, allocs[1]); // Deallocating second allocation should leave data untouched.

    {
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 2, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 2, .byte_size = 2, .is_free = true  },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 4, .byte_size = 2, .is_free = false },
        };
        RunTest("Initial Layout", &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));
    }
    {
        FString<256> description = {};
        Write(&description, "Initial allocation should be non-zero values", INIT_ALLOC_BYTE_SIZE);

        char test_buf[INIT_ALLOC_BYTE_SIZE] = {};
        memset(test_buf, '#', INIT_ALLOC_BYTE_SIZE);

        RunTest(&description, &pass,
                ExpectEqual,
                (const char*)test_buf,  INIT_ALLOC_BYTE_SIZE,
                (const char*)allocs[0], INIT_ALLOC_BYTE_SIZE);
    }
    {
        static constexpr uint32 REALLOC_BYTE_SIZE = 3;
        FString<256> description = {};
        Write(&description, "ReallocateNZ(&free_list, allocs[0], %u) partial merge next", REALLOC_BYTE_SIZE);

        allocs[0] = ReallocateNZ(&free_list, allocs[0], REALLOC_BYTE_SIZE);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 3, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 3, .byte_size = 1, .is_free = true  },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 4, .byte_size = 2, .is_free = false },
        };
        RunTest(&description, &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));

        // Entire reallocated buffer should still contain non-zero values.
        char test_buf[REALLOC_BYTE_SIZE] = {};
        memset(test_buf, '#', REALLOC_BYTE_SIZE);

        RunTest(&description, &pass,
                ExpectEqual,
                (const char*)test_buf,  REALLOC_BYTE_SIZE,
                (const char*)allocs[0], REALLOC_BYTE_SIZE);
    }

    {
        static constexpr uint32 REALLOC_BYTE_SIZE = 4;
        FString<256> description = {};
        Write(&description, "ReallocateNZ(&free_list, allocs[0], %u) full merge next", REALLOC_BYTE_SIZE);

        allocs[0] = ReallocateNZ(&free_list, allocs[0], REALLOC_BYTE_SIZE);
        RangeInfo layout[] =
        {
            RANGE_DATA_RANGE_INFO,
            { .byte_index = FREE_SPACE_BYTE_INDEX + 0, .byte_size = 4, .is_free = false },
            { .byte_index = FREE_SPACE_BYTE_INDEX + 4, .byte_size = 2, .is_free = false },
        };
        RunTest(&description, &pass, ExpectLayout, &free_list, CTK_WRAP_ARRAY(layout));

        // Entire reallocated buffer should still contain non-zero values.
        char test_buf[REALLOC_BYTE_SIZE] = {};
        memset(test_buf, '#', REALLOC_BYTE_SIZE);

        RunTest(&description, &pass,
                ExpectEqual,
                (const char*)test_buf,  REALLOC_BYTE_SIZE,
                (const char*)allocs[0], REALLOC_BYTE_SIZE);
    }

    DestroyFreeList(&free_list, &std_allocator);
    return pass;
}

static bool AllocateAlignmentTest()
{
    bool pass = true;

    static constexpr uint32 FREE_LIST_BYTE_SIZE = 1024;
    static constexpr uint32 FREE_LIST_COUNT     = 16;
    FreeList free_lists[FREE_LIST_COUNT] = {};

    CTK_ITER_PTR(free_list, free_lists, FREE_LIST_COUNT)
    {
        *free_list = CreateFreeList(&std_allocator, FREE_LIST_BYTE_SIZE, { MAX_RANGE_COUNT });
        for (uint32 alignment = 1; alignment <= 32; alignment *= 2)
        {
            FString<256> description = {};
            Write(&description, "Allocate(free_list, size: 1, alignment: %u) is aligned to %u", alignment, alignment);
            uint32 allocation_alignment = GetAlignment(Allocate(free_list, 1, alignment));
            RunTest(&description, &pass, ExpectGTEqual, alignment, allocation_alignment);
        }
    }

    // Cleanup.
    CTK_ITER_PTR(free_list, free_lists, FREE_LIST_COUNT)
    {
        DestroyFreeList(free_list, &std_allocator);
    }

    return pass;
}

static bool ReallocateAlignmentTest()
{
    bool pass = true;

    static constexpr uint32 FREE_LIST_BYTE_SIZE = 1024;
    static constexpr uint32 FREE_LIST_COUNT     = 16;
    FreeList free_lists[FREE_LIST_COUNT] = {};

    CTK_ITER_PTR(free_list, free_lists, FREE_LIST_COUNT)
    {
        *free_list = CreateFreeList(&std_allocator, FREE_LIST_BYTE_SIZE, { MAX_RANGE_COUNT });
        uint8* allocation = Allocate(free_list, 1, 1);
        for (uint32 alignment = 1; alignment <= 32; alignment *= 2)
        {
            FString<256> description = {};
            Write(&description, "allocation = Reallocate(free_list, allocation, 1, alignment: %u) is aligned to %u",
                  alignment, alignment);
            uint32 allocation_alignment = GetAlignment(allocation = Reallocate(free_list, allocation, 1, alignment));
            RunTest(&description, &pass, ExpectGTEqual, alignment, allocation_alignment);
        }
    }

    // Cleanup.
    CTK_ITER_PTR(free_list, free_lists, FREE_LIST_COUNT)
    {
        DestroyFreeList(free_list, &std_allocator);
    }

    return pass;
}

static void AlignmentExample()
{
    CTK_REPEAT(1)
    {
        uint8* mem           = Allocate(256, 16);
        uint32 mem_alignment = GetAlignment(mem);
        uint32 range_index   = 8;
        uint8* range         = mem + range_index;
        Print    ("mem alignment:   %4u [ ", mem_alignment);
        PrintBits(mem);
        Print("]\n");
        Print    ("range alignment: %4u [ ", GetAlignment(range));
        PrintBits(range);
        Print("]\n");
        PrintLine("range_index:     %4u", range_index);
        PrintLine();

        for (uint32 alignment = 1; alignment <= 512; alignment *= 2)
        {
            uint8* aligned_range       = Align(range, alignment);
            uint32 alignment_offset    = (uint32)(aligned_range - range);
            uint32 aligned_range_index = range_index + alignment_offset;
            PrintLine("    alignment:               %4u", alignment);
            PrintLine("    alignment_offset:        %4u", alignment_offset);
            PrintLine("    aligned_range_index:     %4u", aligned_range_index);
            Print    ("    aligned_range alignment: %4u [ ", GetAlignment(aligned_range));
            PrintBits(aligned_range);
            Print("]\n");
            PrintLine();
        }
    }
}

static bool Run()
{
    bool pass = true;

    RunTest("AllocateMultiple()",                          &pass, AllocateMultiple);
    RunTest("AllocateFullFreeList()",                      &pass, AllocateFullFreeList);
    RunTest("DeallocateDoubleMerge()",                     &pass, DeallocateDoubleMerge);
    RunTest("DeallocatePrevMerge()",                       &pass, DeallocatePrevMerge);
    RunTest("DeallocateNextMerge()",                       &pass, DeallocateNextMerge);

    RunTest("ReallocateSameSize()",                        &pass, ReallocateSameSize);
    RunTest("ReallocateSmallerNoNextHeader()",             &pass, ReallocateSmallerNoNextHeader);
    RunTest("ReallocateSmallerNextHeaderIsUsed()",         &pass, ReallocateSmallerNextHeaderIsUsed);
    RunTest("ReallocateSmallerNextHeaderIsFree()",         &pass, ReallocateSmallerNextHeaderIsFree);
    RunTest("ReallocateLargerMergeEntireNextFreeRange()",  &pass, ReallocateLargerMergeEntireNextFreeRange);
    RunTest("ReallocateLargerMergePartialNextFreeRange()", &pass, ReallocateLargerMergePartialNextFreeRange);
    RunTest("ReallocateLargerNewAllocationPostRange()",    &pass, ReallocateLargerNewAllocationPostRange);
    RunTest("ReallocateLargerNewAllocationPreRange()",     &pass, ReallocateLargerNewAllocationPreRange);

    RunTest("ZeroAllocatedMemory()",                       &pass, ZeroAllocatedMemory);
    RunTest("ZeroReallocatedMemory()",                     &pass, ZeroReallocatedMemory);
    RunTest("NonZeroAllocatedMemory()",                    &pass, NonZeroAllocatedMemory);
    RunTest("NonZeroReallocatedMemory()",                  &pass, NonZeroReallocatedMemory);

    RunTest("AllocateAlignmentTest()",                     &pass, AllocateAlignmentTest);
    RunTest("ReallocateAlignmentTest()",                   &pass, ReallocateAlignmentTest);

    // AlignmentExample();
    return pass;
}

}
