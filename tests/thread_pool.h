#pragma once

namespace ThreadPoolTest
{

static CRITICAL_SECTION print_lock;

static void Thread(void* data)
{
    uint32 id = (uint32)data;
    for (uint32 i = 0; i < 4; ++i)
    {
        EnterCriticalSection(&print_lock);
        PrintLine("thread %u: %u", id, i);
        LeaveCriticalSection(&print_lock);
        Sleep(250);
    }
}

static void ThreadPoolTest()
{
    static constexpr uint32 THREAD_COUNT = 4;
    ThreadPool thread_pool = {};
    InitThreadPool(&thread_pool, &std_allocator, THREAD_COUNT);
    InitializeCriticalSection(&print_lock);

    TaskHnd tasks[THREAD_COUNT] = {};
    for (uint32 i = 0; i < THREAD_COUNT; i += 1)
    {
        tasks[i] = SubmitTask(&thread_pool, (void*)i, Thread);
    }
    for (uint32 i = 0; i < THREAD_COUNT; i += 1)
    {
        Wait(&thread_pool, tasks[i]);
    }

    PrintLine("done");

    DestroyThreadPool(&thread_pool);
}

static void DebugGetBatchRange(uint32 thread_index, uint32 total_thread_count, uint32 total_batch_size)
{
    BatchRange res = GetBatchRange(thread_index, total_thread_count, total_batch_size);
    PrintLine("GetBatchRange(thread_index=%u, total_thread_count=%u, total_batch_size=%u) = "
              "{ .start = %u, .size = %u }",
              thread_index, total_thread_count, total_batch_size, res.start, res.size);
}

static void BatchRangeTest()
{
    static constexpr uint32 THREAD_COUNT = 5;
    static constexpr uint32 BATCH_SIZE   = 27;
    for (uint32 thread_index = 0; thread_index < THREAD_COUNT; thread_index += 1)
    {
        DebugGetBatchRange(thread_index, THREAD_COUNT, BATCH_SIZE);
    }
}

static void Run()
{
    ThreadPoolTest();
    BatchRangeTest();
}

}
