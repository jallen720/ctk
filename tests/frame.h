#pragma once

namespace FrameTest
{

bool AllocateTest()
{
    bool pass = true;

    constexpr uint32 STACK_BYTE_SIZE = 32;
    CreateThreadFrameStack(&g_std_allocator, STACK_BYTE_SIZE);

    {
        Frame frame1 = CreateFrame();
        RunTest("CreateFrame()", &pass, TestStackFields, "frame1.stack", frame1.stack, STACK_BYTE_SIZE, 0u);

        auto buf1 = Allocate<char>(&frame1, 6);
        RunTest("Allocate<char>(&frame1, 6)", &pass, TestStackFields, "frame1.stack", frame1.stack, STACK_BYTE_SIZE, 6u);

        Write(buf1, 6, "test1");
        RunTest("Write(buf1, 6, \"test1\");", &pass, ExpectEqual, "test1\0", frame1.stack->mem, 6u);
        {
            Frame frame2 = CreateFrame();
            RunTest("CreateFrame()", &pass, TestStackFields, "frame2.stack", frame2.stack, STACK_BYTE_SIZE, 6u);

            auto buf2 = Allocate<char>(&frame2, 6);
            RunTest("Allocate<char>(&frame2, 6)", &pass, TestStackFields, "frame2.stack", frame2.stack, STACK_BYTE_SIZE, 12u);

            Write(buf2, 6, "test2");
            RunTest("Write(buf2, 6, \"test2\")", &pass, ExpectEqual, "test1\0test2\0", frame2.stack->mem, 12u);
            {
                Frame frame3 = CreateFrame();
                RunTest("CreateFrame()", &pass, TestStackFields, "frame3.stack", frame3.stack, STACK_BYTE_SIZE, 12u);

                auto buf3 = Allocate<char>(&frame3, 6);
                RunTest("Allocate<char>(&frame3, 6)", &pass, TestStackFields, "frame3.stack", frame3.stack, STACK_BYTE_SIZE, 18u);

                Write(buf3, 6, "test3");
                RunTest("Write(buf3, 6, \"test3\")", &pass, ExpectEqual, "test1\0test2\0test3\0", frame3.stack->mem, 18u);
            }

            RunTest("previous frame ended", &pass, TestStackFields, "frame2.stack", frame2.stack, STACK_BYTE_SIZE, 12u);
            RunTest("previous frame ended", &pass, ExpectEqual, "test1\0test2\0test3\0", frame2.stack->mem, 18u);

            {
                Frame frame4 = CreateFrame();
                RunTest("CreateFrame()", &pass, TestStackFields, "frame4.stack", frame4.stack, STACK_BYTE_SIZE, 12u);

                auto buf4 = Allocate<char>(&frame4, 6);
                RunTest("Allocate<char>(&frame4, 6)", &pass, TestStackFields, "frame4.stack", frame4.stack, STACK_BYTE_SIZE, 18u);

                Write(buf4, 6, "test4");
                RunTest("Write(buf4, 6, \"test4\")", &pass, ExpectEqual, "test1\0test2\0test4\0", frame4.stack->mem, 18u);
            }

            RunTest("previous frame ended", &pass, TestStackFields, "frame2.stack", frame2.stack, STACK_BYTE_SIZE, 12u);
            RunTest("previous frame ended", &pass, ExpectEqual, "test1\0test2\0test4\0", frame2.stack->mem, 18u);
        }

        RunTest("previous frame ended", &pass, TestStackFields, "frame1.stack", frame1.stack, STACK_BYTE_SIZE, 6u);
    }

    DestroyThreadFrameStack();

    return pass;
}

bool AllocateOverwriteTest()
{
    bool pass = true;

    constexpr uint32 STACK_BYTE_SIZE = 32;
    CreateThreadFrameStack(&g_std_allocator, STACK_BYTE_SIZE);

    Frame frame1 = CreateFrame();
    auto buf1 = Allocate<char>(&frame1, 6);
    Write(buf1, 6, "test1");
    RunTest("Write(buf1, 6, \"test1\")", &pass, ExpectEqual, "test1\0", frame1.stack->mem, 6u);

    {
        Frame frame2 = CreateFrame();
        auto buf2 = Allocate<char>(&frame2, 6);
        Write(buf2, 6, "test2");
        RunTest("Write(buf2, 6, \"test2\")", &pass, ExpectEqual, "test1\0test2\0", frame1.stack->mem, 12u);

        auto buf3 = Allocate<char>(&frame1, 6);
        Write(buf3, 6, "test3");
        RunTest("Write(buf3, 6, \"test3\")", &pass, ExpectEqual, "test1\0test3\0", frame1.stack->mem, 18u);
    }

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
