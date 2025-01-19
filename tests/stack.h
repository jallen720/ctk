#pragma once

namespace StackTest
{

/// Tests
////////////////////////////////////////////////////////////
bool AllocateTest()
{
    bool pass = true;

    constexpr uint32 STACK_BYTE_SIZE = 32;

    Stack stack = CreateStack(&g_std_allocator, STACK_BYTE_SIZE);

    FString<256> description = {};
    Write(&description, "CreateStack(&g_std_allocator, %u)", STACK_BYTE_SIZE);
    RunTest(&description, &pass, TestStackFields, "stack", &stack, STACK_BYTE_SIZE, 0u);

    char* buffer = NULL;

    {
        buffer = Allocate<char>(&stack.allocator, STACK_BYTE_SIZE);

        FString<256> description = {};
        Write(&description, "Allocate(&stack.allocator, %u)", STACK_BYTE_SIZE);
        RunTest(&description, &pass, TestStackFields, "stack", &stack, STACK_BYTE_SIZE, STACK_BYTE_SIZE);
    }
    {
        Write(buffer, STACK_BYTE_SIZE, "test");

        Write(&description, "Write(buffer, %u, \"test\")", STACK_BYTE_SIZE);
        RunTest(&description, &pass, ExpectEqual, "test\0", stack.mem, 5u);
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
            Write(&description, "Allocate(&stack->allocator, size: 1, alignment: %u) is aligned to %u",
                  alignment, alignment);
            uint32 allocation_alignment = GetAlignment(Allocate(&stack->allocator, 1, alignment));
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

bool TempStackAllocateTest()
{
    bool pass = true;

    static constexpr uint32 TEMP_STACK_SIZE = 512u;
    TempStack_Init(&g_std_allocator, TEMP_STACK_SIZE);
    Stack* temp_stack = TempStack_Stack();
    Allocator* temp_stack_allocator = TempStack_Allocator();

    uint32 frame1 = TempStack_PushFrame();

    auto buf1 = Allocate<char>(temp_stack_allocator, 6);
    RunTest("Allocate<char>(temp_stack_allocator, 6)", &pass,
            TestStackFields, "g_temp_stack", temp_stack, TEMP_STACK_SIZE, 6u);

    Write(buf1, 6, "test1");
    RunTest("Write(buf1, 6, \"test1\");", &pass,
            ExpectEqual, "test1\0", temp_stack->mem, 6u);
    {
        uint32 frame2 = TempStack_PushFrame();

        auto buf2 = Allocate<char>(temp_stack_allocator, 6);
        RunTest("Allocate<char>(temp_stack_allocator, 6)", &pass,
                TestStackFields, "g_temp_stack", temp_stack, TEMP_STACK_SIZE, 12u);

        Write(buf2, 6, "test2");
        RunTest("Write(buf2, 6, \"test2\")", &pass,
                ExpectEqual, "test1\0test2\0", temp_stack->mem, 12u);

        TempStack_PopFrame(frame2);
    }

    RunTest("frame2 ended", &pass,
            TestStackFields, "g_temp_stack", temp_stack, TEMP_STACK_SIZE, 6u);
    RunTest("frame2 ended", &pass,
            ExpectEqual, "test1\0", temp_stack->mem, 6u);

    {
        uint32 frame3 = TempStack_PushFrame();

        auto buf3 = Allocate<char>(temp_stack_allocator, 6);
        RunTest("Allocate<char>(temp_stack_allocator, 6)", &pass,
                TestStackFields, "g_temp_stack", temp_stack, TEMP_STACK_SIZE, 12u);

        Write(buf3, 6, "test3");
        RunTest("Write(buf3, 6, \"test3\")", &pass,
                ExpectEqual, "test1\0test3\0", temp_stack->mem, 12u);

        TempStack_PopFrame(frame3);
    }

    RunTest("frame3 ended", &pass, TestStackFields, "g_temp_stack", temp_stack, TEMP_STACK_SIZE, 6u);
    RunTest("frame3 ended", &pass, ExpectEqual, "test1\0", temp_stack->mem, 6u);

    TempStack_PopFrame(frame1);

    RunTest("frame1 ended", &pass, TestStackFields, "g_temp_stack", temp_stack, TEMP_STACK_SIZE, 0u);

    TempStack_Deinit();

    return pass;
}

bool TempStackAllocateOverwriteTest()
{
    bool pass = true;

    static constexpr uint32 TEMP_STACK_SIZE = 512u;
    TempStack_Init(&g_std_allocator, TEMP_STACK_SIZE);
    Stack* temp_stack = TempStack_Stack();
    Allocator* temp_stack_allocator = TempStack_Allocator();

    uint32 frame1 = TempStack_PushFrame();

    auto buf1 = Allocate<char>(temp_stack_allocator, 6u);
    Write(buf1, 6u, "test1");
    RunTest("Write(buf1, 6u, \"test1\")", &pass, ExpectEqual, "test1\0", temp_stack->mem, 6u);

    {
        uint32 frame2 = TempStack_PushFrame();

        auto buf2 = Allocate<char>(temp_stack_allocator, 6u);
        Write(buf2, 6u, "test2");
        RunTest("Write(buf2, 6u, \"test2\")", &pass, ExpectEqual, "test1\0test2\0", temp_stack->mem, 12u);

        TempStack_PopFrame(frame2);
    }

    TempStack_PopFrame(frame1);

    TempStack_Deinit();

    return pass;
}

#define EXPECT_FATAL_ERROR(EXPR)\
    PrintExpected("fatal error");\
    try\
    {\
        EXPR;\
        pass = false;\
        PrintActual(pass, "no fatal error");\
    }\
    catch (sint32 _)\
    {\
        PrintActual(pass, "fatal error");\
    }

bool TempStackVerifyNoFramesOrFatalFailure()
{
    bool pass = true;

    static constexpr uint32 TEMP_STACK_SIZE = 512u;
    TempStack_Init(&g_std_allocator, TEMP_STACK_SIZE);

    uint32 frame1 = TempStack_PushFrame();
        EXPECT_FATAL_ERROR(TempStack_VerifyNoFramesOrFatal());
        EXPECT_FATAL_ERROR(TempStack_Deinit());
    TempStack_PopFrame(frame1);

    TempStack_VerifyNoFramesOrFatal();
    TempStack_Deinit();

    return pass;
}

bool TempStackMissingNestedPopTest()
{
    bool pass = true;

    static constexpr uint32 TEMP_STACK_SIZE = 512u;
    TempStack_Init(&g_std_allocator, TEMP_STACK_SIZE);

    uint32 frame1 = TempStack_PushFrame();
        Allocate<uint32>(TempStack_Allocator(), 64);
        uint32 frame2 = TempStack_PushFrame();
        EXPECT_FATAL_ERROR(TempStack_PopFrame(frame1));
        TempStack_PopFrame(frame2);
    TempStack_PopFrame(frame1);

    TempStack_Deinit();

    return pass;
}

bool TempStackDoublePopTest()
{
    bool pass = true;

    static constexpr uint32 TEMP_STACK_SIZE = 512u;
    TempStack_Init(&g_std_allocator, TEMP_STACK_SIZE);

    uint32 frame1 = TempStack_PushFrame();
    TempStack_PopFrame(frame1);
    EXPECT_FATAL_ERROR(TempStack_PopFrame(frame1));

    TempStack_Deinit();

    return pass;
}

bool Run()
{
    bool pass = true;

    RunTest("AllocateTest",                          &pass, AllocateTest);
    RunTest("AlignmentTest",                         &pass, AlignmentTest);
    RunTest("TempStackAllocateTest",                 &pass, TempStackAllocateTest);
    RunTest("TempStackAllocateOverwriteTest",        &pass, TempStackAllocateOverwriteTest);
    RunTest("TempStackVerifyNoFramesOrFatalFailure", &pass, TempStackVerifyNoFramesOrFatalFailure);
    RunTest("TempStackMissingNestedPopTest",         &pass, TempStackMissingNestedPopTest);
    RunTest("TempStackDoublePopTest",                &pass, TempStackDoublePopTest);

    return pass;
}

}
