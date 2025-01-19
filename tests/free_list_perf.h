#pragma once

namespace FreeListPerfTest
{

/// Data
////////////////////////////////////////////////////////////
enum struct OperationType
{
    ALLOCATE,
    DEALLOCATE,
    REALLOCATE,
};

struct AllocateOperation
{
    uint32 size;
};

struct DeallocateOperation
{
    uint32 index;
};

struct ReallocateOperation
{
    uint32 index;
    uint32 new_size;
};

struct AllocationChunk
{
    uint8 bytes[16];
};

struct Allocation
{
    AllocationChunk* ptr;
    uint32           size;
};

struct Operation
{
    OperationType type;
    union
    {
        AllocateOperation   allocate_operation;
        DeallocateOperation deallocate_operation;
        ReallocateOperation reallocate_operation;
    };
};

/// Debugging
////////////////////////////////////////////////////////////
void PrintOperation(Operation* op)
{
    if (op->type == OperationType::ALLOCATE)
    {
        PrintLine("type:     OperationType::ALLOCATE");
        PrintLine("size:     %u", op->allocate_operation.size);
    }
    else if (op->type == OperationType::REALLOCATE)
    {
        PrintLine("type:     OperationType::REALLOCATE");
        PrintLine("index:    %u", op->reallocate_operation.index);
        PrintLine("new_size: %u", op->reallocate_operation.new_size);
    }
    else if (op->type == OperationType::DEALLOCATE)
    {
        PrintLine("type:     OperationType::DEALLOCATE");
        PrintLine("index:    %u", op->deallocate_operation.index);
    }
    else
    {
        CTK_FATAL("unknown operation type: %u", op->type);
    }
}

/// Tests
////////////////////////////////////////////////////////////
constexpr uint32 WARMUP_PASS = UINT32_MAX;

float64 Test(Array<Operation>* ops, Array<Allocation>* allocations, Allocator* allocator, uint32 pass)
{
    Profile test_profile = {};

    if (pass != WARMUP_PASS)
    {
        FString<128> profile_name = {};
        Write(&profile_name, "test (pass %2u)", pass);
        test_profile = BeginProfile(profile_name.data);
    }
    else
    {
        PrintLine("running warmup pass...");
    }

    for (uint32 i = 0; i < ops->count; i += 1)
    {
        Operation* op = GetPtr(ops, i);
        if (op->type == OperationType::ALLOCATE)
        {
            AllocateOperation* allocate_op = &op->allocate_operation;

            Allocation* allocation = Push(allocations);
            allocation->ptr  = AllocateNZ<AllocationChunk>(allocator, allocate_op->size);
            allocation->size = allocate_op->size;
        }
        else if (op->type == OperationType::REALLOCATE)
        {
            ReallocateOperation* reallocate_op = &op->reallocate_operation;
            Allocation* allocation = GetPtr(allocations, reallocate_op->index);
            allocation->ptr  = ReallocateNZ(allocator, allocation->ptr, reallocate_op->new_size);
            allocation->size = reallocate_op->new_size;
        }
        else if (op->type == OperationType::DEALLOCATE)
        {
            DeallocateOperation* deallocate_op = &op->deallocate_operation;
            Allocation* allocation = GetPtr(allocations, deallocate_op->index);
            Deallocate(allocator, allocation->ptr);
            Remove(allocations, deallocate_op->index);
        }
        else
        {
            CTK_FATAL("unknown operation type: %u", op->type);
        }
    }

    if (pass != WARMUP_PASS)
    {
        EndProfile(&test_profile);
        PrintProfileLine(&test_profile);
    }

    return test_profile.ms;
}

void Run()
{
    PrintLine("\nFreeList Performance Test");

    constexpr uint32 FREE_LIST_BYTE_SIZE = Megabyte32<256>();
    constexpr uint32 MAX_ALLOCATION_SIZE = 4000;

    // Generate randomized operations for test.
    // RandomSeed();
    constexpr uint32 OP_COUNT      = 1000000;
    constexpr uint32 ALLOC_RATIO   = 1;
    constexpr uint32 REALLOC_RATIO = 1;
    constexpr uint32 DEALLOC_RATIO = 1;
    constexpr uint32 ALLOC_RANGE   = 0             + ALLOC_RATIO;
    constexpr uint32 REALLOC_RANGE = ALLOC_RANGE   + REALLOC_RATIO;
    constexpr uint32 DEALLOC_RANGE = REALLOC_RANGE + DEALLOC_RATIO;
    constexpr uint32 TOTAL_RANGE   = DEALLOC_RANGE;

    auto ops = CreateArray<Operation>(&g_std_allocator, OP_COUNT);
    uint32 allocate_op_count    = 0;
    uint32 reallocate_op_count  = 0;
    uint32 deallocate_op_count  = 0;
    uint32 allocation_count     = 0;
    uint32 max_allocation_count = 0;
    for (uint32 i = 0; i < OP_COUNT; i += 1)
    {
        Operation* op = Push(&ops);

        // Don't generate Deallocate/Reallocate operations if there are no allocations.
        if (allocation_count > 1)
        {
            uint32 type_value = RandomRange(0u, TOTAL_RANGE);
            if (type_value < ALLOC_RANGE)
            {
                op->type = OperationType::ALLOCATE;
            }
            else if (type_value < REALLOC_RANGE)
            {
                op->type = OperationType::REALLOCATE;
            }
            else if (type_value < DEALLOC_RANGE)
            {
                op->type = OperationType::DEALLOCATE;
            }
            else
            {
                CTK_FATAL("unhandled operation type value: %u; max is %u", type_value, TOTAL_RANGE - 1);
            }
        }
        else
        {
            op->type = OperationType::ALLOCATE;
        }

        if (op->type == OperationType::ALLOCATE)
        {
            allocate_op_count += 1;
            allocation_count += 1;
            if (allocation_count > max_allocation_count)
            {
                max_allocation_count = allocation_count;
            }
            op->allocate_operation.size = RandomRange(1u, MAX_ALLOCATION_SIZE + 1);
        }
        else if (op->type == OperationType::REALLOCATE)
        {
            reallocate_op_count += 1;
            op->reallocate_operation.index    = RandomRange(0u, allocation_count);
            op->reallocate_operation.new_size = RandomRange(1u, MAX_ALLOCATION_SIZE + 1);
        }
        else if (op->type == OperationType::DEALLOCATE)
        {
            deallocate_op_count += 1;
            op->deallocate_operation.index = RandomRange(0u, allocation_count);
            allocation_count -= 1;
        }
    }
    PrintLine("allocations:     %u", allocate_op_count);
    PrintLine("reallocations:   %u", reallocate_op_count);
    PrintLine("deallocations:   %u", deallocate_op_count);
    PrintLine("max allocations: %u", max_allocation_count);

    // Run tests.
    constexpr uint32 MAX_ALLOCATIONS = 2048;
    constexpr uint32 TEST_PASSES     = 8;
    auto allocations = CreateArray<Allocation>(&g_std_allocator, max_allocation_count);

#if 1
    // Stdlib Tests
    {
        PrintLine();
        PrintLine("Stdlib Test");

        // Run warmup test then free/clear allocations.
        Test(&ops, &allocations, &g_std_allocator, WARMUP_PASS);
        for (uint32 allocation_index = 0; allocation_index < allocations.count; allocation_index += 1)
        {
            Deallocate(&g_std_allocator, GetPtr(&allocations, allocation_index)->ptr);
        }
        Clear(&allocations);

        float64 total_ms = 0.0;
        for (uint32 pass = 0; pass < TEST_PASSES; pass += 1)
        {
            // Run test then free/clear allocations.
            total_ms += Test(&ops, &allocations, &g_std_allocator, pass);
            for (uint32 allocation_index = 0; allocation_index < allocations.count; allocation_index += 1)
            {
                Deallocate(&g_std_allocator, GetPtr(&allocations, allocation_index)->ptr);
            }
            Clear(&allocations);
        }
        PrintLine("average:        %.f ms", total_ms / TEST_PASSES);
    }
#endif

#if 1
    // FreeList Tests
    {
        PrintLine();
        PrintLine("FreeList Test");
        FreeList free_list = {};

        // Run warmup test then cleanup free_list and allocations for next test.
        free_list = CreateFreeList(&g_std_allocator, FREE_LIST_BYTE_SIZE, { max_allocation_count * 2 });
        Test(&ops, &allocations, &free_list.allocator, WARMUP_PASS);
// PrintUsage(&free_list);
        DestroyFreeList(&free_list);
        Clear(&allocations);

        // Run all test passes.
        float64 total_ms = 0.0;
        for (uint32 pass = 0; pass < TEST_PASSES; pass += 1)
        {
            // Run test then cleanup free_list and allocations for next test.
            free_list = CreateFreeList(&g_std_allocator, FREE_LIST_BYTE_SIZE, { max_allocation_count * 2 });
            total_ms += Test(&ops, &allocations, &free_list.allocator, pass);
            DestroyFreeList(&free_list);
            Clear(&allocations);
        }
        PrintLine("average:        %.f ms", total_ms / TEST_PASSES);
    }
#endif

    DestroyArray(&ops);
    DestroyArray(&allocations);
}

}
