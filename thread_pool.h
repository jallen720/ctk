/// Data
////////////////////////////////////////////////////////////
struct BatchRange
{
    uint32 start;
    uint32 size;
};

using TaskHnd = uint32;

struct Task
{
    CRITICAL_SECTION   lock;
    CONDITION_VARIABLE done_convar;
    void*              data;
    Func<void, void*>  func;
    TaskHnd            next;
    bool               done;
};

struct TaskList
{
    TaskHnd            head;
    CRITICAL_SECTION   lock;
    CONDITION_VARIABLE available;
};

struct ThreadPool
{
    Array<HANDLE> threads;
    Array<Task>   tasks;
    TaskList      idle_tasks;
    TaskList      ready_tasks;
    uint32        size;
};

static constexpr TaskHnd NO_TASK = UINT32_MAX;

/// Utils
////////////////////////////////////////////////////////////
static void InitTaskList(TaskList* task_list)
{
    task_list->head = NO_TASK;
    InitializeCriticalSection(&task_list->lock);
    InitializeConditionVariable(&task_list->available);
}

static TaskHnd PopTask(ThreadPool* thread_pool, TaskList* task_list)
{
    EnterCriticalSection(&task_list->lock);

    // Wait for task to be available.
    while (task_list->head == NO_TASK)
    {
        SleepConditionVariableCS(&task_list->available, &task_list->lock, INFINITE);
    }

    // Get task pointer and handle and update list head.
    TaskHnd task_hnd = task_list->head;
    Task* task = GetPtr(&thread_pool->tasks, task_hnd);
    task_list->head = task->next;

    LeaveCriticalSection(&task_list->lock);

    // Initialize popped task.
    task->next = NO_TASK;

    return task_hnd;
}

static void PushTask(ThreadPool* thread_pool, TaskList* task_list, TaskHnd task_hnd)
{
    Task* task = GetPtr(&thread_pool->tasks, task_hnd);
    EnterCriticalSection(&task_list->lock);
    task->next = task_list->head;
    task_list->head = task_hnd;
    LeaveCriticalSection(&task_list->lock);

    // Wake next thread waiting for task to be ready.
    WakeConditionVariable(&task_list->available);
}

/// Interface
////////////////////////////////////////////////////////////
static DWORD ThreadFunc(void* data)
{
    auto thread_pool = (ThreadPool*)data;
    for (;;)
    {
        // Pop next ready task once available.
        TaskHnd task_hnd = PopTask(thread_pool, &thread_pool->ready_tasks);
        Task* task = GetPtr(&thread_pool->tasks, task_hnd);

        // Run task.
        task->func(task->data);

        // Flag task as done and wake any wait() operations for this task.
        EnterCriticalSection(&task->lock);
        task->done = true;
        LeaveCriticalSection(&task->lock);
        WakeAllConditionVariable(&task->done_convar);

        // Push task to top of idle list.
        PushTask(thread_pool, &thread_pool->idle_tasks, task_hnd);
    }
    return 0;
}

static void InitThreadPool(ThreadPool* thread_pool, Allocator* allocator, uint32 thread_count)
{
    CTK_ASSERT(thread_count > 0);

    thread_pool->size    = thread_count;
    thread_pool->threads = CreateArray<HANDLE>(allocator, thread_count);
    thread_pool->tasks   = CreateArrayFull<Task>(allocator, thread_count);
    InitTaskList(&thread_pool->idle_tasks);
    InitTaskList(&thread_pool->ready_tasks);

    // Create threads.
    for (uint32 i = 0; i < thread_count; ++i)
    {
        HANDLE hnd = CreateThread(NULL,        // Thread Attributes
                                  0,           // Stack Size
                                  ThreadFunc,  // Thread Function
                                  thread_pool, // Thread Data
                                  0,           // Creation Flags
                                  NULL);       // Pointer to variable for thread ID.
        if (hnd == NULL)
        {
            Win32Error e = {};
            GetWin32Error(&e);
            CTK_FATAL("CreateThread() failed: %.*s", e.message_length, e.message);
        }
        Push(&thread_pool->threads, hnd);
    }

    // Initialize tasks.
    for (uint32 i = 0; i < thread_count; ++i)
    {
        Task* task = GetPtr(&thread_pool->tasks, i);
        task->done = false;
        task->next = NO_TASK;
        InitializeCriticalSection(&task->lock);
        InitializeConditionVariable(&task->done_convar);
    }

    // Link all tasks and set first task as idle_task.
    for (uint32 i = 0; i < thread_count - 1; ++i)
    {
        GetPtr(&thread_pool->tasks, i)->next = i + 1;
    }

    thread_pool->idle_tasks.head = 0;
}

static void DeinitThreadPool(ThreadPool* thread_pool, Allocator* allocator)
{
    DestroyArray(&thread_pool->threads, allocator);
    DestroyArray(&thread_pool->tasks, allocator);
}

static ThreadPool* CreateThreadPool(Allocator* allocator, uint32 thread_count)
{
    CTK_ASSERT(thread_count > 0);

    auto thread_pool = Allocate<ThreadPool>(allocator, 1);
    InitThreadPool(thread_pool, allocator, thread_count);
    return thread_pool;
}

static void DestroyThreadPool(ThreadPool* thread_pool, Allocator* allocator)
{
    DeinitThreadPool(thread_pool, allocator);
    Deallocate(allocator, thread_pool);
}

static TaskHnd SubmitTask(ThreadPool* thread_pool, void* data, Func<void, void*> func)
{
    // Pop next idle task once available.
    TaskHnd task_hnd = PopTask(thread_pool, &thread_pool->idle_tasks);
    Task* task = GetPtr(&thread_pool->tasks, task_hnd);

    // Initialize task.
    task->data = data;
    task->func = func;
    task->done = false;

    // Push to end of ready tasks.
    PushTask(thread_pool, &thread_pool->ready_tasks, task_hnd);

    return task_hnd;
}

static void Wait(ThreadPool* thread_pool, TaskHnd task_hnd)
{
    Task* task = GetPtr(&thread_pool->tasks, task_hnd);
    if (task->done)
    {
        return;
    }

    EnterCriticalSection(&task->lock);
    while (!task->done)
    {
        SleepConditionVariableCS(&task->done_convar, &task->lock, INFINITE);
    }
    LeaveCriticalSection(&task->lock);
}

static BatchRange GetBatchRange(uint32 region_index, uint32 region_count, uint32 batch_size)
{
    uint32 small_part_size = batch_size / region_count;
    uint32 large_part_size = small_part_size + 1;
    uint32 large_part_count = batch_size % region_count;

    return
    {
        .start = (region_index * small_part_size) + Min(large_part_count, region_index),
        .size  = region_index < large_part_count ? large_part_size : small_part_size,
    };
}

static void GetBatchRanges(Array<BatchRange>* batch_ranges, uint32 total_batch_size)
{
    CTK_ASSERT(batch_ranges->count > 0);

    // Calculate batch range start and base size.
    uint32 base_batch_size = total_batch_size / batch_ranges->count;
    for (uint32 i = 0; i < batch_ranges->count; ++i)
    {
        GetPtr(batch_ranges, i)->size = base_batch_size;
    }

    // Add remainders to sizes where needed.
    uint32 remainders = total_batch_size % batch_ranges->count;
    for (uint32 i = 0; i < remainders; ++i)
    {
        GetPtr(batch_ranges, i)->size += 1;
    }

    // Calculate batch range start indexes.
    uint32 start = 0;
    for (uint32 i = 0; i < batch_ranges->count; ++i)
    {
        BatchRange* batch_range = GetPtr(batch_ranges, i);
        batch_range->start = start;
        start += batch_range->size;
    }
}

