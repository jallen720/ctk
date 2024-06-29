#pragma once

namespace StackTest
{

static bool TestStackFields(const char* stack_name, Stack* stack, uint32 expected_size, uint32 expected_count)
{
    bool pass = true;

    FString<256> description = {};
    Write(&description, "%s->size", stack_name);
    if (!ExpectEqual(&description, expected_size, stack->size))
    {
        pass = false;
    }

    Write(&description, "%s->count", stack_name);
    if (!ExpectEqual(&description, expected_count, stack->count))
    {
        pass = false;
    }

    return pass;
}

static bool ExpectBytes(Stack* stack, uint8* expected_bytes)
{
    bool pass = true;

    for (uint32 i = 0; i < stack->size; ++i)
    {
        if (stack->mem[i] != expected_bytes[i])
        {
            pass = false;
            break;
        }
    }

    PrintExpectedTag();
    PrintBytes(expected_bytes, stack->size);
    PrintLine();

    PrintActualTag(pass);
    PrintBytes(stack->mem, stack->size);
    PrintLine();

    return pass;
}

static bool AllocateTest()
{
    bool pass = true;

    static constexpr uint32 STACK_BYTE_SIZE = 32;
    Stack stack = CreateStack(&std_allocator, STACK_BYTE_SIZE);
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

    DestroyStack(&stack, &std_allocator);
    return pass;
}

static bool CreateFrameTest()
{
    bool pass = true;

    static constexpr uint32 STACK_BYTE_SIZE = 32;
    Stack stack = CreateStack(&std_allocator, STACK_BYTE_SIZE);

    {
        // Allocate then clear &stack to zero memory.
        Allocate<uint8>(&stack, STACK_BYTE_SIZE);
        Clear(&stack);

        uint8 expected_bytes[STACK_BYTE_SIZE] = {};
        RunTest("Init Stack Mem", &pass, ExpectBytes, &stack, expected_bytes);
        RunTest("Init Stack Mem", &pass, TestStackFields, "&stack", &stack, STACK_BYTE_SIZE, 0u);
    }
    {
        FString<STACK_BYTE_SIZE> expected_bytes = {};

        char* buffer = Allocate<char>(&stack, 8);
        RunTest("Allocate<char>(&stack, 8)", &pass, TestStackFields, "&stack", &stack, STACK_BYTE_SIZE, 8u);

        Write(buffer, 8, "test");
        PushRange(&expected_bytes, "test\0\0\0\0", 8);
        RunTest("Write(buffer, 8, \"test\") Stack Mem", &pass, ExpectBytes, &stack, (uint8*)expected_bytes.data);

        uint32 count_before_frame = stack.count;
        {
            Stack frame = CreateFrame(&stack);
            RunTest("frame = CreateFrame(&stack)", &pass,
                    TestStackFields, "&stack", &stack, STACK_BYTE_SIZE, STACK_BYTE_SIZE);
            RunTest("frame = CreateFrame(&stack)", &pass,
                    TestStackFields, "frame", &frame, STACK_BYTE_SIZE - count_before_frame, 0u);

            char* temp_buffer = Allocate<char>(&frame, 12);
            RunTest("Allocate<char>(&frame, 12)", &pass,
                    TestStackFields, "frame", &frame, STACK_BYTE_SIZE - count_before_frame, 12u);

            // Write to temp_buffer then write frame mem to expected_bytes to check against &stack bytes.
            Write(temp_buffer, 12, "temp test 1");
            PushRange(&expected_bytes, (char*)frame.mem, frame.size);
            RunTest("Write(temp_buffer, 12, \"temp test 1\") Stack Mem", &pass,
                    ExpectBytes, &stack, (uint8*)expected_bytes.data);
        }
        RunTest("Frame leaves scope", &pass, TestStackFields, "&stack", &stack, STACK_BYTE_SIZE, count_before_frame);

        expected_bytes.count = count_before_frame;
        {
            Stack frame = CreateFrame(&stack);
            RunTest("frame = CreateFrame(&stack)", &pass,
                    TestStackFields, "&stack", &stack, STACK_BYTE_SIZE, STACK_BYTE_SIZE);
            RunTest("frame = CreateFrame(&stack)", &pass,
                    TestStackFields, "frame", &frame, STACK_BYTE_SIZE - count_before_frame, 0u);

            char* temp_buffer = Allocate<char>(&frame, 12);
            RunTest("Allocate<char>(&frame, 12)", &pass,
                    TestStackFields, "frame", &frame, STACK_BYTE_SIZE - count_before_frame, 12u);

            // Write to temp_buffer then write frame mem to expected_bytes to check against &stack bytes.
            Write(temp_buffer, 12, "temp test 2");
            PushRange(&expected_bytes, (char*)frame.mem, frame.size);
            RunTest("Write(temp_buffer, 12, \"temp test 2\") Stack Mem", &pass,
                    ExpectBytes, &stack, (uint8*)expected_bytes.data);
        }
        RunTest("Frame leaves scope", &pass, TestStackFields, "&stack", &stack, STACK_BYTE_SIZE, count_before_frame);
    }

    DestroyStack(&stack, &std_allocator);
    return pass;
}

static bool AlignmentTest()
{
    bool pass = true;

    static constexpr uint32 STACK_BYTE_SIZE = 1024;
    static constexpr uint32 STACK_COUNT     = 16;
    Stack stacks[STACK_COUNT] = {};

    CTK_ITER_PTR(stack, stacks, STACK_COUNT)
    {
        *stack = CreateStack(&std_allocator, STACK_BYTE_SIZE);
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
        DestroyStack(stack, &std_allocator);
    }

    return pass;
}

static bool Run()
{
    bool pass = true;

    RunTest("AllocateTest",    &pass, AllocateTest);
    RunTest("CreateFrameTest", &pass, CreateFrameTest);
    RunTest("AlignmentTest",   &pass, AlignmentTest);

    return pass;
}

}
