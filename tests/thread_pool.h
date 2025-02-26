#pragma once

namespace ThreadPoolTest {

CRITICAL_SECTION print_lock;

void Thread(void* data) {
    uint32 id = (uint32)data;
    Allocator* temp_stack_allocator = TempStack_Allocator();
    for (uint32 i = 0; i < 4; i += 1) {
        uint32 frame = TempStack_PushFrame();

        uint32* x = Allocate<uint32>(temp_stack_allocator, 1);
        *x = id * i;
        EnterCriticalSection(&print_lock);
        PrintLine("thread %u x (%u * %u): (%p) %u", id, i, id, x, *x);
        LeaveCriticalSection(&print_lock);
        Sleep(250);

        TempStack_PopFrame(frame);
    }
}

void ThreadPoolTest() {
    constexpr uint32 THREAD_COUNT = 4;
    ThreadPool thread_pool = {};
    InitThreadPool(&thread_pool, &g_std_allocator, THREAD_COUNT, 512u);
    InitializeCriticalSection(&print_lock);

    TaskHnd tasks[THREAD_COUNT] = {};
    for (uint32 i = 0; i < THREAD_COUNT; i += 1) {
        tasks[i] = SubmitTask(&thread_pool, (void*)i, Thread);
    }
    for (uint32 i = 0; i < THREAD_COUNT; i += 1) {
        Wait(&thread_pool, tasks[i]);
    }

    PrintLine("done");

    DestroyThreadPool(&thread_pool);
}

void DebugGetBatchRange(uint32 thread_index, uint32 total_thread_count, uint32 total_batch_size) {
    BatchRange res = GetBatchRange(thread_index, total_thread_count, total_batch_size);
    PrintLine("GetBatchRange(thread_index=%u, total_thread_count=%u, total_batch_size=%u) = "
              "{ .start = %u, .size = %u }",
              thread_index, total_thread_count, total_batch_size, res.start, res.size);
}

void BatchRangeTest() {
    constexpr uint32 THREAD_COUNT = 5;
    constexpr uint32 BATCH_SIZE   = 27;
    for (uint32 thread_index = 0; thread_index < THREAD_COUNT; thread_index += 1) {
        DebugGetBatchRange(thread_index, THREAD_COUNT, BATCH_SIZE);
    }
}

void Run() {
    ThreadPoolTest();
    BatchRangeTest();
}

}
