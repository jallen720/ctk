#pragma once

namespace FrameTest
{

uint32 ExpectedFrameSize(Frame* frame)
{
    // Parent stack size, minus any allocated space, minus the stack allocated for this frame.
    return frame->parent->size - Align(frame->parent_base_count, 8u) - Align(SizeOf32<Stack>(), 8u);
}

bool AllocateTest()
{
    bool pass = true;

    const uint32 FRAME_ALLOCATOR_SIZE = 512u;
    InitFrameAllocator(&g_std_allocator, FRAME_ALLOCATOR_SIZE);

    {
        Frame frame1 = CreateFrame();
        uint32 frame1_expected_size = ExpectedFrameSize(&frame1);
        RunTest("CreateFrame()", &pass,
                TestStackFields, "frame1.stack", frame1.stack, frame1_expected_size, 0u);

        auto buf1 = Allocate<char>(&frame1, 6);
        RunTest("Allocate<char>(&frame1, 6)", &pass,
                TestStackFields, "frame1.stack", frame1.stack, frame1_expected_size, 6u);

        Write(buf1, 6, "test1");
        RunTest("Write(buf1, 6, \"test1\");", &pass,
                ExpectEqual, "test1\0", frame1.stack->mem, 6u);
        {
            Frame frame2 = CreateFrame();
            uint32 frame2_expected_size = ExpectedFrameSize(&frame2);
            RunTest("CreateFrame()", &pass,
                    TestStackFields, "frame2.stack", frame2.stack, frame2_expected_size, 0u);

            auto buf2 = Allocate<char>(&frame2, 6);
            RunTest("Allocate<char>(&frame2, 6)", &pass,
                    TestStackFields, "frame2.stack", frame2.stack, frame2_expected_size, 6u);

            Write(buf2, 6, "test2");
            RunTest("Write(buf2, 6, \"test2\")", &pass,
                    ExpectEqual, "test2\0", frame2.stack->mem, 6u);
        }

        RunTest("frame2 ended", &pass,
                TestStackFields, "frame1.stack", frame1.stack, frame1_expected_size, 6u);
        RunTest("frame2 ended", &pass,
                ExpectEqual, "test1\0", frame1.stack->mem, 6u);

        {
            Frame frame3 = CreateFrame();
            uint32 frame3_expected_size = ExpectedFrameSize(&frame3);
            RunTest("CreateFrame()", &pass,
                    TestStackFields, "frame3.stack", frame3.stack, frame3_expected_size, 0u);

            auto buf3 = Allocate<char>(&frame3, 6);
            RunTest("Allocate<char>(&frame3, 6)", &pass,
                    TestStackFields, "frame3.stack", frame3.stack, frame3_expected_size, 6u);

            Write(buf3, 6, "test3");
            RunTest("Write(buf3, 6, \"test3\")", &pass,
                    ExpectEqual, "test3\0", frame3.stack->mem, 6u);
        }

        RunTest("frame3 ended", &pass,
                TestStackFields, "frame1.stack", frame1.stack, frame1_expected_size, 6u);
        RunTest("frame3 ended", &pass,
                ExpectEqual, "test1\0", frame1.stack->mem, 6u);
    }

    DeinitFrameAllocator();

    return pass;
}

bool AllocateOverwriteTest()
{
    bool pass = true;

    constexpr uint32 FRAME_ALLOCATOR_SIZE = 512u;
    InitFrameAllocator(&g_std_allocator, FRAME_ALLOCATOR_SIZE);

    Frame frame1 = CreateFrame();
    auto buf1 = Allocate<char>(&frame1, 8u);
    Write(buf1, 6u, "test1");
    RunTest("Write(buf1, 6u, \"test1\")", &pass, ExpectEqual, "test1\0", frame1.stack->mem, 6u);

    {
        Frame frame2 = CreateFrame();
        auto buf2 = Allocate<char>(&frame2, 6u);
        Write(buf2, 6u, "test2");
        RunTest("Write(buf2, 6u, \"test2\")", &pass, ExpectEqual, "test2\0", frame2.stack->mem, 6u);

        RunTest<Func<char*, Allocator*, uint32>>(
            "Allocate<char>(&frame1, 6u) while frame2 is still in scope", &pass,
            ExpectFatalError, Allocate, (Allocator*)&frame1, 6u);
    }

    DeinitFrameAllocator();

    return pass;
}

bool Run()
{
    bool pass = true;

    RunTest("AllocateTest",          &pass, AllocateTest);
    RunTest("AllocateOverwriteTest", &pass, AllocateOverwriteTest);

    return pass;
}

}
