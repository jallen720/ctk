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
    .parent       = NULL,
    .Allocate     = STD_Allocate,
    .AllocateNZ   = STD_AllocateNZ,
    .Reallocate   = NULL,
    .ReallocateNZ = STD_ReallocateNZ,
    .Deallocate   = STD_Deallocate,
};

/// Frame Allocator
////////////////////////////////////////////////////////////
static constexpr uint32 MAX_FRAME_THREADS = 64;
static constexpr uint32 FRAME_STACK_SIZE  = Kilobyte32<4>();
FMap<DWORD, Stack, MAX_FRAME_THREADS> g_frame_stacks;

struct Frame : public Allocator
{
    Stack* stack;
    uint32 start_count;
    ~Frame()
    {
        stack->count = start_count;
    }
};

void CreateThreadFrameStack(Allocator* allocator, uint32 size)
{
    DWORD thread_id = GetCurrentThreadId();
    if (!CanPush(&g_frame_stacks, thread_id))
    {
        CTK_FATAL("can't create thread frame stack: frame stack already created for this thread");
    }

    Push(&g_frame_stacks, thread_id, CreateStack(allocator, size));
}

void DestroyThreadFrameStack()
{
    DWORD thread_id = GetCurrentThreadId();
    Stack* frame_stack = FindValue(&g_frame_stacks, thread_id);
    if (frame_stack == NULL)
    {
        CTK_FATAL("can't destroy thread frame stack: frame stack doesn't exist for this thread");
    }

    DestroyStack(frame_stack);
    Remove(&g_frame_stacks, thread_id);
}

uint8* Frame_Allocate(Allocator* allocator, uint32 size, uint32 alignment)
{
    auto frame = (Frame*)allocator;
    return Stack_Allocate(frame->stack, size, alignment);
}

uint8* Frame_AllocateNZ(Allocator* allocator, uint32 size, uint32 alignment)
{
    auto frame = (Frame*)allocator;
    return Stack_AllocateNZ(frame->stack, size, alignment);
}

Frame CreateFrame()
{
    Stack* frame_stack = FindValue(&g_frame_stacks, GetCurrentThreadId());
    if (frame_stack == NULL)
    {
        CTK_FATAL("can't create frame; frame stack for this thread has not been initialized");
    }

    Frame frame = {};
    frame.Allocate    = Frame_Allocate;
    frame.AllocateNZ  = Frame_AllocateNZ;
    frame.stack       = frame_stack;
    frame.start_count = frame_stack->count;
    return frame;
}
