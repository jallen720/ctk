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
static Stack g_temp_stack;

void InitTempStack(Allocator* parent, uint32 size)
{
    CTK_ASSERT(size > 0);
    g_temp_stack = CreateStack(parent, size);
}

void DeinitTempStack()
{
    DestroyStack(&g_temp_stack);
}

uint32 PushTempStackFrame()
{
    if (g_temp_stack.size == 0)
    {
        CTK_FATAL("can't push temp stack frame; temp stack is not initialized");
    }

    if (g_temp_stack.count >= g_temp_stack.size)
    {
        CTK_FATAL("can't push temp stack frame; temp stack is full");
    }

    return g_temp_stack.count;
}

void PopTempStackFrame(uint32 frame)
{
    if (g_temp_stack.size == 0)
    {
        CTK_FATAL("can't pop temp stack frame; temp stack is not initialized");
    }

    if (frame > g_temp_stack.count)
    {
        CTK_FATAL("can't pop temp stack frame; frame is higher than current temp stack index, meaning a frame pushed "
                  "before this one has already been popped. Check the order in which PopTempStackFrame() is called "
                  "with other temp stack frames.");
    }

    g_temp_stack.count = frame;
}

Allocator* TempStackAllocator()
{
    if (g_temp_stack.size == 0)
    {
        CTK_FATAL("can't get temp stack allocator; temp stack is not initialized");
    }

    return &g_temp_stack.allocator;
}
