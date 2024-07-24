#pragma once

namespace StackTest
{

bool TestStackFields(const char* stack_name, Allocator* stack, uint32 expected_size, uint32 expected_count)
{
    bool pass = true;

    auto stack_context = (Stack*)stack->context;

    FString<256> description = {};
    Write(&description, "%s->size", stack_name);
    if (!ExpectEqual(&description, expected_size, stack_context->size))
    {
        pass = false;
    }

    Write(&description, "%s->count", stack_name);
    if (!ExpectEqual(&description, expected_count, stack_context->count))
    {
        pass = false;
    }

    return pass;
}

bool ExpectBytes(Allocator* stack, uint8* expected_bytes)
{
    bool pass = true;

    auto stack_context = (Stack*)stack->context;

    for (uint32 i = 0; i < stack_context->size; ++i)
    {
        if (stack_context->mem[i] != expected_bytes[i])
        {
            pass = false;
            break;
        }
    }

    PrintExpectedTag();
    PrintBytes(expected_bytes, stack_context->size);
    PrintLine();

    PrintActualTag(pass);
    PrintBytes(stack_context->mem, stack_context->size);
    PrintLine();

    return pass;
}

bool AllocateTest()
{
    bool pass = true;

    constexpr uint32 STACK_BYTE_SIZE = 32;
    Allocator stack = CreateStack(&g_std_allocator, STACK_BYTE_SIZE);
    char* buffer = NULL;

    {
        buffer = Allocate<char>(&stack, STACK_BYTE_SIZE);
        uint8 expected_bytes[STACK_BYTE_SIZE] = {};
        FString<256> description = {};
        Write(&description, "Allocate(&stack, %u)", STACK_BYTE_SIZE);

        RunTest(&description, &pass, TestStackFields, "&stack", &stack, STACK_BYTE_SIZE, STACK_BYTE_SIZE);
        RunTest(&description, &pass, ExpectBytes, &stack, expected_bytes);
    }
    {
        uint8 expected_bytes[STACK_BYTE_SIZE] = {};
        Write((char*)expected_bytes, STACK_BYTE_SIZE, "test");
        Write(buffer, STACK_BYTE_SIZE, "test");

        FString<256> description = {};
        Write(&description, "Write(buffer, %u, \"test\")", STACK_BYTE_SIZE);
        RunTest(&description, &pass, ExpectBytes, &stack, expected_bytes);
    }

    DestroyStack(&stack);
    return pass;
}

bool AlignmentTest()
{
    bool pass = true;

    constexpr uint32 STACK_BYTE_SIZE = 1024;
    constexpr uint32 STACK_COUNT     = 16;
    Allocator stacks[STACK_COUNT] = {};

    CTK_ITER_PTR(stack, stacks, STACK_COUNT)
    {
        *stack = CreateStack(&g_std_allocator, STACK_BYTE_SIZE);
        for (uint32 alignment = 1; alignment <= 32; alignment *= 2)
        {
            FString<256> description = {};
            Write(&description, "Allocate(stack, size: 1, alignment: %u) is aligned to %u", alignment, alignment);
            uint32 allocation_alignment = GetAlignment(Allocate(stack, 1, alignment));
            RunTest(&description, &pass, ExpectGTEqual, alignment, allocation_alignment);
        }
    }

    // Cleanup.
    CTK_ITER_PTR(stack, stacks, STACK_COUNT)
    {
        DestroyStack(stack);
    }

    return pass;
}

bool Run()
{
    bool pass = true;

    RunTest("AllocateTest",  &pass, AllocateTest);
    RunTest("AlignmentTest", &pass, AlignmentTest);

    return pass;
}

}
