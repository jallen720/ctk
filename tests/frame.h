#pragma once

namespace FrameTest
{

bool AllocateTest()
{
    bool pass = true;

    constexpr uint32 STACK_BYTE_SIZE = 32;
    CreateThreadFrameStack(&g_std_allocator, STACK_BYTE_SIZE);
    FString<STACK_BYTE_SIZE> expected_bytes = {};

    {
        Frame frame1 = CreateFrame();
        RunTest("CreateFrame()", &pass, TestStackFields, "frame1.stack", frame1.stack, STACK_BYTE_SIZE, 0u);

        auto buf1 = Allocate<char>(&frame1, 6);
        RunTest("Allocate<char>(&frame1, 6)", &pass, TestStackFields, "frame1.stack", frame1.stack, STACK_BYTE_SIZE, 6u);

        Write(buf1, 6, "test1");
        PushRange(&expected_bytes, "test1", 6);
        RunTest("Write(buf1, 6, \"test1\");", &pass, ExpectBytes, frame1.stack, (uint8*)expected_bytes.data);
        {
            Frame frame2 = CreateFrame();
            RunTest("CreateFrame()", &pass, TestStackFields, "frame2.stack", frame2.stack, STACK_BYTE_SIZE, 6u);

            auto buf2 = Allocate<char>(&frame2, 6);
            RunTest("Allocate<char>(&frame2, 6)", &pass, TestStackFields, "frame2.stack", frame2.stack, STACK_BYTE_SIZE, 12u);

            Write(buf2, 6, "test2");
            PushRange(&expected_bytes, "test2", 6);
            RunTest("Write(buf2, 6, \"test2\")", &pass, ExpectBytes, frame2.stack, (uint8*)expected_bytes.data);
            {
                Frame frame3 = CreateFrame();
                RunTest("CreateFrame()", &pass, TestStackFields, "frame3.stack", frame3.stack, STACK_BYTE_SIZE, 12u);

                auto buf3 = Allocate<char>(&frame3, 6);
                RunTest("Allocate<char>(&frame3, 6)", &pass, TestStackFields, "frame3.stack", frame3.stack, STACK_BYTE_SIZE, 18u);

                Write(buf3, 6, "test3");
                PushRange(&expected_bytes, "test3", 6);
                RunTest("Write(buf3, 6, \"test3\")", &pass, ExpectBytes, frame3.stack, (uint8*)expected_bytes.data);
            }

            expected_bytes.count -= 6;
            RunTest("previous frame ended", &pass, TestStackFields, "frame2.stack", frame2.stack, STACK_BYTE_SIZE, 12u);

            {
                Frame frame4 = CreateFrame();
                RunTest("CreateFrame()", &pass, TestStackFields, "frame4.stack", frame4.stack, STACK_BYTE_SIZE, 12u);

                auto buf4 = Allocate<char>(&frame4, 6);
                RunTest("Allocate<char>(&frame4, 6)", &pass, TestStackFields, "frame4.stack", frame4.stack, STACK_BYTE_SIZE, 18u);

                Write(buf4, 6, "test4");
                PushRange(&expected_bytes, "test4", 6);
                RunTest("Write(buf4, 6, \"test4\")", &pass, ExpectBytes, frame4.stack, (uint8*)expected_bytes.data);
            }

            RunTest("previous frame ended", &pass, TestStackFields, "frame2.stack", frame2.stack, STACK_BYTE_SIZE, 12u);
        }

        RunTest("previous frame ended", &pass, TestStackFields, "frame1.stack", frame1.stack, STACK_BYTE_SIZE, 6u);
    }

    DestroyThreadFrameStack();
    return pass;
}

bool Run()
{
    bool pass = true;

    RunTest("AllocateTest", &pass, AllocateTest);

    return pass;
}

}
