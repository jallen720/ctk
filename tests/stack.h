#pragma once

namespace StackTest {

/// Utils
////////////////////////////////////////////////////////////
bool ExpectStackFields(const char* stack_name, Stack* stack,
                       uint32 expected_size,
                       uint32 expected_count,
                       uint32 expected_reserve_start_index) {
    bool pass = true;

    FString<256> description = {};
    Write(&description, "%s->size", stack_name);
    if (!ExpectEqual(&description, expected_size, stack->size)) {
        pass = false;
    }

    Write(&description, "%s->count", stack_name);
    if (!ExpectEqual(&description, expected_count, stack->count)) {
        pass = false;
    }

    Write(&description, "%s->reserve_start_index", stack_name);
    if (!ExpectEqual(&description, expected_reserve_start_index, stack->reserve_start_index)) {
        pass = false;
    }

    return pass;
}

/// Tests
////////////////////////////////////////////////////////////
bool AllocateTest() {
    bool pass = true;

    constexpr uint32 STACK_BYTE_SIZE = 32;

    Stack stack = CreateStack(&g_std_allocator, STACK_BYTE_SIZE);

    FString<256> description = {};
    Write(&description, "CreateStack(&g_std_allocator, %u)", STACK_BYTE_SIZE);
    RunTest(&description, &pass, ExpectStackFields, "stack", &stack, STACK_BYTE_SIZE, 0u, UINT32_MAX);

    char* buffer = NULL; {
        buffer = Allocate<char>(&stack.allocator, STACK_BYTE_SIZE);

        FString<256> description = {};
        Write(&description, "Allocate(&stack.allocator, %u)", STACK_BYTE_SIZE);
        RunTest(&description, &pass, ExpectStackFields, "stack", &stack, STACK_BYTE_SIZE, STACK_BYTE_SIZE, UINT32_MAX);
    } {
        Write(buffer, STACK_BYTE_SIZE, "test");

        Write(&description, "Write(buffer, %u, \"test\")", STACK_BYTE_SIZE);
        RunTest(&description, &pass, ExpectEqual, "test\0", stack.mem, 5u);
    }

    DestroyStack(&stack);
    return pass;
}

bool AlignmentTest() {
    bool pass = true;

    constexpr uint32 STACK_BYTE_SIZE = 1024;
    constexpr uint32 STACK_COUNT     = 16;
    Stack stacks[STACK_COUNT] = {};

    CTK_ITER_PTR(stack, stacks, STACK_COUNT) {
        *stack = CreateStack(&g_std_allocator, STACK_BYTE_SIZE);
        for (uint32 alignment = 1; alignment <= 32; alignment *= 2) {
            FString<256> description = {};
            Write(&description, "Allocate(&stack->allocator, size: 1, alignment: %u) is aligned to %u",
                  alignment, alignment);
            uint32 allocation_alignment = GetAlignment(Allocate(&stack->allocator, 1, alignment));
            RunTest(&description, &pass, ExpectGTEqual, alignment, allocation_alignment);
        }
    }

    // Cleanup.
    CTK_ITER_PTR(stack, stacks, STACK_COUNT) {
        DestroyStack(stack);
    }

    return pass;
}

bool TempStackAllocateTest() {
    bool pass = true;

    static constexpr uint32 TEMP_STACK_SIZE = 512u;
    TempStack_Init(&g_std_allocator, TEMP_STACK_SIZE);
    Stack* temp_stack = TempStack_Stack();
    Allocator* temp_stack_allocator = TempStack_Allocator();

    uint32 frame1 = TempStack_PushFrame();

    auto buf1 = Allocate<char>(temp_stack_allocator, 6);
    RunTest("Allocate<char>(temp_stack_allocator, 6)", &pass,
            ExpectStackFields, "g_temp_stack", temp_stack, TEMP_STACK_SIZE, 6u, UINT32_MAX);

    Write(buf1, 6, "test1");
    RunTest("Write(buf1, 6, \"test1\");", &pass,
            ExpectEqual, "test1\0", temp_stack->mem, 6u); {
        uint32 frame2 = TempStack_PushFrame();

        auto buf2 = Allocate<char>(temp_stack_allocator, 6);
        RunTest("Allocate<char>(temp_stack_allocator, 6)", &pass,
                ExpectStackFields, "g_temp_stack", temp_stack, TEMP_STACK_SIZE, 12u, UINT32_MAX);

        Write(buf2, 6, "test2");
        RunTest("Write(buf2, 6, \"test2\")", &pass,
                ExpectEqual, "test1\0test2\0", temp_stack->mem, 12u);

        TempStack_PopFrame(frame2);
    }

    RunTest("frame2 ended", &pass,
            ExpectStackFields, "g_temp_stack", temp_stack, TEMP_STACK_SIZE, 6u, UINT32_MAX);
    RunTest("frame2 ended", &pass,
            ExpectEqual, "test1\0", temp_stack->mem, 6u); {
        uint32 frame3 = TempStack_PushFrame();

        auto buf3 = Allocate<char>(temp_stack_allocator, 6);
        RunTest("Allocate<char>(temp_stack_allocator, 6)", &pass,
                ExpectStackFields, "g_temp_stack", temp_stack, TEMP_STACK_SIZE, 12u, UINT32_MAX);

        Write(buf3, 6, "test3");
        RunTest("Write(buf3, 6, \"test3\")", &pass,
                ExpectEqual, "test1\0test3\0", temp_stack->mem, 12u);

        TempStack_PopFrame(frame3);
    }

    RunTest("frame3 ended", &pass, ExpectStackFields, "g_temp_stack", temp_stack, TEMP_STACK_SIZE, 6u, UINT32_MAX);
    RunTest("frame3 ended", &pass, ExpectEqual, "test1\0", temp_stack->mem, 6u);

    TempStack_PopFrame(frame1);

    RunTest("frame1 ended", &pass, ExpectStackFields, "g_temp_stack", temp_stack, TEMP_STACK_SIZE, 0u, UINT32_MAX);

    TempStack_Deinit();

    return pass;
}

bool TempStackAllocateOverwriteTest() {
    bool pass = true;

    static constexpr uint32 TEMP_STACK_SIZE = 512u;
    TempStack_Init(&g_std_allocator, TEMP_STACK_SIZE);
    Stack* temp_stack = TempStack_Stack();
    Allocator* temp_stack_allocator = TempStack_Allocator();

    uint32 frame1 = TempStack_PushFrame();

    auto buf1 = Allocate<char>(temp_stack_allocator, 6u);
    Write(buf1, 6u, "test1");
    RunTest("Write(buf1, 6u, \"test1\")", &pass, ExpectEqual, "test1\0", temp_stack->mem, 6u); {
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

#define EXPECT_FATAL_ERROR(EXPR) \
    PrintExpected("fatal error"); \
    try { \
        EXPR; \
        pass = false; \
        PrintActual(pass, "no fatal error"); \
    } \
    catch (sint32 _) { \
        PrintActual(pass, "fatal error"); \
    }

bool TempStackVerifyNoFramesOrFatalFailure() {
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

bool TempStackMissingNestedPopTest() {
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

bool TempStackDoublePopTest() {
    bool pass = true;

    static constexpr uint32 TEMP_STACK_SIZE = 512u;
    TempStack_Init(&g_std_allocator, TEMP_STACK_SIZE);

    uint32 frame1 = TempStack_PushFrame();
    TempStack_PopFrame(frame1);
    EXPECT_FATAL_ERROR(TempStack_PopFrame(frame1));

    TempStack_Deinit();

    return pass;
}

bool ReserveTest() {
    bool pass = true;

    static constexpr uint32 STACK_SIZE = 512u;
    Stack stack = CreateStack(&g_std_allocator, STACK_SIZE);

    uint32* data;
    uint32  data_size;
    Reserve(&stack, &data, &data_size);

    RunTest("Reserve(&stack, &data, &data_size);", &pass,
            ExpectStackFields, "stack", &stack, STACK_SIZE, STACK_SIZE, 0u);
    RunTest("data == stack->mem + stack->reserve_start_index", &pass,
            ExpectEqual, (uint64)data, (uint64)(stack.mem + stack.reserve_start_index));
    RunTest("data_size == STACK_SIZE / sizeof(uint32)", &pass, ExpectEqual, data_size, STACK_SIZE / SizeOf32<uint32>());

    data_size /= 2;
    Commit(&stack, sizeof(uint32), data_size);

    RunTest("data_size /= 2; Commit(&stack, sizeof(uint32), data_size);", &pass,
            ExpectStackFields, "stack", &stack, STACK_SIZE, data_size * SizeOf32<uint32>(), UINT32_MAX);

    DestroyStack(&stack);

    return pass;
}

bool DoubleReserveTest() {
    bool pass = true;

    static constexpr uint32 STACK_SIZE = 512u;
    Stack stack = CreateStack(&g_std_allocator, STACK_SIZE);

    uint32* data      = NULL;
    uint32  data_size = 0;
    Reserve(&stack, &data, &data_size);
    RunTest("Reserve(&stack, &data, &data_size) called twice", &pass,
            ExpectFatalError, Reserve<uint32>, &stack, &data, &data_size);

    DestroyStack(&stack);

    return pass;
}

bool ReserveAlignmentTest() {
    bool pass = true;

    static constexpr uint32 STACK_SIZE = 8u;
    Stack stack = CreateStack(&g_std_allocator, STACK_SIZE);

    Allocate(&stack, 2, alignof(uint8));
    RunTest("Allocate(&stack, 2, alignof(uint8));", &pass,
            ExpectStackFields, "stack", &stack, STACK_SIZE, 2u, UINT32_MAX);

    uint64* uint64_data = NULL;
    uint32* uint32_data = NULL;
    uint32  data_size   = 0;

    RunTest("Reserve(&stack, &uint64_data, &data_size); 8-byte aligned data will overflow stack", &pass,
            ExpectFatalError, Reserve<uint64>, &stack, &uint64_data, &data_size);

    Reserve(&stack, &uint32_data, &data_size);
    RunTest("Reserve(&stack, &uint32_data, &data_size); 4-byte aligned data works fine", &pass,
            ExpectStackFields, "stack", &stack, STACK_SIZE, STACK_SIZE, 4u);
    RunTest("uint32_data == stack.mem + 4", &pass,
            ExpectEqual, (uint64)uint32_data, (uint64)(stack.mem + 4u));

    Commit(&stack, sizeof(uint32), 1);
    RunTest("data_size += 1; Commit(&stack, data_size);", &pass,
            ExpectStackFields, "stack", &stack, STACK_SIZE, STACK_SIZE, UINT32_MAX);

    DestroyStack(&stack);

    return pass;
}

bool Run() {
    bool pass = true;

    RunTest("AllocateTest",                          &pass, AllocateTest);
    RunTest("AlignmentTest",                         &pass, AlignmentTest);
    RunTest("TempStackAllocateTest",                 &pass, TempStackAllocateTest);
    RunTest("TempStackAllocateOverwriteTest",        &pass, TempStackAllocateOverwriteTest);
    RunTest("TempStackVerifyNoFramesOrFatalFailure", &pass, TempStackVerifyNoFramesOrFatalFailure);
    RunTest("TempStackMissingNestedPopTest",         &pass, TempStackMissingNestedPopTest);
    RunTest("TempStackDoublePopTest",                &pass, TempStackDoublePopTest);
    RunTest("ReserveTest",                           &pass, ReserveTest);
    RunTest("DoubleReserveTest",                     &pass, DoubleReserveTest);
    RunTest("ReserveAlignmentTest",                  &pass, ReserveAlignmentTest);

    return pass;
}

}
