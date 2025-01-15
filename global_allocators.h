/// STD Allocator
////////////////////////////////////////////////////////////
uint8* STD_AllocateNZ(Allocator* allocator, uint32 size, uint32 alignment)
{
    CTK_UNUSED(allocator);
    CTK_ASSERT(size > 0);

    return (uint8*)_aligned_malloc(size, alignment);
}

uint8* STD_Allocate(Allocator* allocator, uint32 size, uint32 alignment)
{
    uint8* allocated_mem = STD_AllocateNZ(allocator, size, alignment);
    memset(allocated_mem, 0, size);
    return allocated_mem;
}

uint8* STD_ReallocateNZ(Allocator* allocator, void* mem, uint32 new_size, uint32 alignment)
{
    CTK_UNUSED(allocator);

    return (uint8*)_aligned_realloc(mem, new_size, alignment);
}

void STD_Deallocate(Allocator* allocator, void* mem)
{
    CTK_UNUSED(allocator);

    _aligned_free(mem);
}

Allocator g_std_allocator =
{
    .Allocate     = STD_Allocate,
    .AllocateNZ   = STD_AllocateNZ,
    .Reallocate   = NULL,
    .ReallocateNZ = STD_ReallocateNZ,
    .Deallocate   = STD_Deallocate,
};

/// Temp Stack Allocator
////////////////////////////////////////////////////////////
static constexpr uint32 MAX_THREAD_TEMP_STACKS = 256;
static FMap<DWORD, Stack, MAX_THREAD_TEMP_STACKS> g_temp_stacks;

void InitTempStack(Allocator* parent, uint32 size)
{
    CTK_ASSERT(size > 0);

    DWORD thread_id = GetCurrentThreadId();
    Stack* temp_stack = FindValue(&g_temp_stacks, thread_id);
    if (temp_stack != NULL)
    {
        CTK_FATAL("can't initialize temp stack on thread %u; temp stack already initialized", thread_id);
    }

    Push(&g_temp_stacks, thread_id, CreateStack(parent, size));
}

void DeinitTempStack()
{
    DWORD thread_id = GetCurrentThreadId();
    Stack* temp_stack = FindValue(&g_temp_stacks, thread_id);
    if (temp_stack == NULL)
    {
        CTK_FATAL("can't de-initialize temp stack on thread %u; temp stack is not initialized", thread_id);
    }

    DestroyStack(temp_stack);
}

uint32 PushTempStackFrame()
{
    DWORD thread_id = GetCurrentThreadId();
    Stack* temp_stack = FindValue(&g_temp_stacks, thread_id);
    if (temp_stack == NULL)
    {
        CTK_FATAL("can't push temp stack frame on thread %u; temp stack is not initialized", thread_id);
    }

    if (temp_stack->count >= temp_stack->size)
    {
        CTK_FATAL("can't push temp stack frame; temp stack is full");
    }

    return temp_stack->count;
}

void PopTempStackFrame(uint32 frame)
{
    DWORD thread_id = GetCurrentThreadId();
    Stack* temp_stack = FindValue(&g_temp_stacks, thread_id);
    if (temp_stack == NULL)
    {
        CTK_FATAL("can't pop temp stack frame on thread %u; temp stack is not initialized", thread_id);
    }

    if (frame > temp_stack->count)
    {
        CTK_FATAL("can't pop temp stack frame; frame is higher than current temp stack index, meaning a frame pushed "
                  "before this one has already been popped. Check the order in which PopTempStackFrame() is called "
                  "with other temp stack frames.");
    }

    temp_stack->count = frame;
}

Allocator* TempStackAllocator()
{
    DWORD thread_id = GetCurrentThreadId();
    Stack* temp_stack = FindValue(&g_temp_stacks, thread_id);
    if (temp_stack == NULL)
    {
        CTK_FATAL("can't get temp stack allocator on thread %u; temp stack is not initialized", thread_id);
    }

    return &temp_stack->allocator;
}

Stack* TempStack()
{
    DWORD thread_id = GetCurrentThreadId();
    Stack* temp_stack = FindValue(&g_temp_stacks, thread_id);
    if (temp_stack == NULL)
    {
        CTK_FATAL("can't get temp stack on thread %u; temp stack is not initialized", thread_id);
    }

    return temp_stack;
}
