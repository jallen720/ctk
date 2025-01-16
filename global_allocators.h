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
struct Frame
{
    const char* file;
    uint32      line_num;
    uint32      stack_index;
};
struct TempStack
{
    Stack             stack;
    FArray<Frame, 64> frames;
};
static FMap<DWORD, TempStack, MAX_THREAD_TEMP_STACKS> g_temp_stacks;

/// Utils
////////////////////////////////////////////////////////////
TempStack* TempStackOrFatal(const char* func)
{
    DWORD thread_id = GetCurrentThreadId();
    TempStack* temp_stack = FindValue(&g_temp_stacks, thread_id);
    if (temp_stack == NULL)
    {
        CTK_FATAL("%s() failed on thread %u; temp stack is not initialized", func, thread_id);
    }
    return temp_stack;
}

void PrintUsedFrames(TempStack* temp_stack, uint32 start_frame_index)
{
    for (uint32 i = start_frame_index; i < temp_stack->frames.count; i += 1)
    {
        Frame* nested_frame = GetPtr(&temp_stack->frames, i);
        PrintError("    used_frame (index = %u):", i);
        PrintError("        file:        %s", nested_frame->file);
        PrintError("        line_num:    %u", nested_frame->line_num);
        PrintError("        stack_index: %u", nested_frame->stack_index);
    }
}

void TempStack_VerifyNoFramesOrFatal_Internal(TempStack* temp_stack)
{
    if (temp_stack->frames.count != 0)
    {
        PrintError("frames are still in use:");
        PrintUsedFrames(temp_stack, 0);
        CTK_FATAL("TempStack_VerifyNoFramesOrFatal failed on thread %u", GetCurrentThreadId());
    }
}

/// Interface
////////////////////////////////////////////////////////////
void TempStack_Init(Allocator* parent, uint32 size)
{
    CTK_ASSERT(size > 0);

    DWORD thread_id = GetCurrentThreadId();
    TempStack* temp_stack = FindValue(&g_temp_stacks, thread_id);
    if (temp_stack != NULL)
    {
        CTK_FATAL("TempStack_Init() failed on thread %u; temp stack already initialized", thread_id);
    }

    temp_stack = Push(&g_temp_stacks, thread_id);
    temp_stack->stack = CreateStack(parent, size);
}

void TempStack_Deinit()
{
    TempStack* temp_stack = TempStackOrFatal(__FUNCTION__);
    TempStack_VerifyNoFramesOrFatal_Internal(temp_stack);
    DestroyStack(&temp_stack->stack);
    Remove(&g_temp_stacks, GetCurrentThreadId());
}

void TempStack_VerifyNoFramesOrFatal()
{
    TempStack* temp_stack = TempStackOrFatal(__FUNCTION__);
    TempStack_VerifyNoFramesOrFatal_Internal(temp_stack);
    if (temp_stack->frames.count != 0)
    {
        PrintError("frames are still in use:");
        PrintUsedFrames(temp_stack, 0);
        CTK_FATAL("TempStack_VerifyNoFramesOrFatal() failed on thread %u", GetCurrentThreadId());
    }
}

uint32 TempStack_PushFrame(const char* file, uint32 line_num)
{
    TempStack* temp_stack = TempStackOrFatal(__FUNCTION__);
    if (temp_stack->stack.count >= temp_stack->stack.size)
    {
        CTK_FATAL("can't push temp stack frame; temp stack is full");
    }

    uint32 frame_index = temp_stack->frames.count;
    Frame* frame = Push(&temp_stack->frames);
    frame->file        = file;
    frame->line_num    = line_num;
    frame->stack_index = temp_stack->stack.count;
    return frame_index;
}
#define TempStack_PushFrame() TempStack_PushFrame(__FILE__, __LINE__)

void TempStack_PopFrame(uint32 frame_index, const char* file, uint32 line_num)
{
    TempStack* temp_stack = TempStackOrFatal(__FUNCTION__);

    if (frame_index < temp_stack->frames.count &&
        frame_index == GetLastIndex(&temp_stack->frames))
    {
        temp_stack->stack.count = PopPtr(&temp_stack->frames)->stack_index;
    }
    else
    {
        if (frame_index >= temp_stack->frames.count)
        {
            PrintError("frame index %u has already been popped from the temp stack or is invalid", frame_index);
        }
        else
        {
            Frame* frame = GetPtr(&temp_stack->frames, frame_index);
            PrintError("frame for frame_index passed to TempStack_PopFrame():");
            PrintError("    frame (index = %u):", frame_index);
            PrintError("        file:        %s", frame->file);
            PrintError("        line_num:    %u", frame->line_num);
            PrintError("        stack_index: %u", frame->stack_index);
            PrintError("frame_index passed to TempStack_PopFrame() doesn't refer to top frame in temp stack's frame "
                       "list; frames must be popped in reverse order to the order they were pushed (FILO); these are "
                       "the frames that haven't been popped:");
            PrintUsedFrames(temp_stack, frame_index + 1);
        }

        CTK_FATAL("TempStack_PopFrame() failed on thread %u", GetCurrentThreadId());
    }
}
#define TempStack_PopFrame(frame_index) TempStack_PopFrame(frame_index, __FILE__, __LINE__)

Stack* TempStack_Stack()
{
    DWORD thread_id = GetCurrentThreadId();
    TempStack* temp_stack = FindValue(&g_temp_stacks, thread_id);
    if (temp_stack == NULL)
    {
        CTK_FATAL("can't get temp stack on thread %u; temp stack is not initialized", thread_id);
    }

    return &temp_stack->stack;
}

Allocator* TempStack_Allocator()
{
    return &TempStack_Stack()->allocator;
}
