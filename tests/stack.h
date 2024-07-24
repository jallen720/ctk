#pragma once

namespace StackTest
{

bool AllocateTest()
{
    bool pass = true;

    constexpr uint32 STACK_BYTE_SIZE = 32;
    Stack stack = CreateStack(&g_std_allocator, STACK_BYTE_SIZE);
    char* buffer = NULL;

    {
        buffer = Allocate<char>(&stack, STACK_BYTE_SIZE);
        FString<STACK_BYTE_SIZE> expected_bytes = {};

        FString<256> description = {};
        Write(&description, "Allocate(&stack, %u)", STACK_BYTE_SIZE);

        RunTest(&description, &pass, TestStackFields, "&stack", &stack, STACK_BYTE_SIZE, STACK_BYTE_SIZE);
        RunTest(&description, &pass, ExpectBytes, &stack, (uint8*)expected_bytes.data);
    }
    {
        Write(buffer, STACK_BYTE_SIZE, "test");

        FString<STACK_BYTE_SIZE> expected_bytes = {};
        Write(&expected_bytes, "test");

        FString<256> description = {};
        Write(&description, "Write(buffer, %u, \"test\")", STACK_BYTE_SIZE);
        RunTest(&description, &pass, ExpectBytes, &stack, (uint8*)expected_bytes.data);
    }

    DestroyStack(&stack);
    return pass;
}

bool AlignmentTest()
{
    bool pass = true;

    constexpr uint32 STACK_BYTE_SIZE = 1024;
    constexpr uint32 STACK_COUNT     = 16;
    Stack stacks[STACK_COUNT] = {};

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
