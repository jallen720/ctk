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
struct StackList
{
    Stack  base;
    Stack* top;
};

struct Frame : public Allocator
{
    StackList* stack_list;
    Stack*     stack;
    Stack*     parent;
    uint32     parent_base_count;
    ~Frame()
    {
        parent->count = parent_base_count;
        stack_list->top = parent;
    }
};

static constexpr uint32 MAX_STACK_LISTS  = 64;
static constexpr uint32 FRAME_STACK_SIZE = Kilobyte32<4>();
FMap<DWORD, StackList, MAX_STACK_LISTS> g_stack_lists;

void InitFrameAllocator(Allocator* allocator, uint32 size)
{
    DWORD thread_id = GetCurrentThreadId();
    if (!CanPush(&g_stack_lists, thread_id))
    {
        CTK_FATAL("can't create thread frame stack: frame stack already created for this thread");
    }

    StackList* stack_list = Push(&g_stack_lists, thread_id);
    stack_list->base = CreateStack(allocator, size);
    stack_list->top  = &stack_list->base;
}

void DestroyFrameAllocator()
{
    DWORD thread_id = GetCurrentThreadId();
    StackList* stack_list = FindValue(&g_stack_lists, thread_id);
    if (stack_list == NULL)
    {
        CTK_FATAL("can't destroy thread frame stack: frame stack doesn't exist for this thread");
    }

    DestroyStack(&stack_list->base);
    Remove(&g_stack_lists, thread_id);
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
    StackList* stack_list = FindValue(&g_stack_lists, GetCurrentThreadId());
    if (stack_list == NULL)
    {
        CTK_FATAL("can't create frame; frame stack for this thread has not been initialized");
    }

    // Allocate new stack for top stack.
    Stack* parent = stack_list->top;
    uint32 parent_base_count = parent->count;
    stack_list->top = Allocate<Stack>(parent, 1);
    *stack_list->top = CreateStack(parent, parent->size - parent->count);

    Frame frame = {};
    frame.Allocate          = Frame_Allocate;
    frame.AllocateNZ        = Frame_AllocateNZ;
    frame.stack_list        = stack_list;
    frame.stack             = stack_list->top;
    frame.parent            = parent;
    frame.parent_base_count = parent_base_count;
    return frame;
}
