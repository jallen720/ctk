/// Data
////////////////////////////////////////////////////////////
struct Stack
{
    uint8* mem;
    uint32 size;
    uint32 count;
};

/// Interface
////////////////////////////////////////////////////////////
uint8* Stack_AllocateNZ(void* context, uint32 size, uint32 alignment)
{
    auto stack = (Stack*)context;
    CTK_ASSERT(size > 0);

    uint32 aligned_index = Align(stack->mem + stack->count, alignment) - stack->mem;
    if (aligned_index + size > stack->size)
    {
        CTK_FATAL("cannot allocate %u bytes from stack at %u-byte aligned address at index %u; allocation would exceed "
                  "stack size of %u",
                  size, alignment, aligned_index, stack->size);
    }

    stack->count = aligned_index + size;
    return &stack->mem[aligned_index];
}

uint8* Stack_Allocate(void* context, uint32 size, uint32 alignment)
{
    uint8* allocated_mem = Stack_AllocateNZ(context, size, alignment);
    memset(allocated_mem, 0, size);
    return allocated_mem;
}

Allocator CreateStack(Allocator* parent, uint32 size)
{
    CTK_ASSERT(size > 0);

    Stack* stack = Allocate<Stack>(parent, 1);
    stack->mem   = Allocate<uint8>(parent, size);
    stack->size  = size;
    stack->count = 0;

    Allocator stack_alloc = {};
    stack_alloc.parent     = parent;
    stack_alloc.context    = stack;
    stack_alloc.type       = AllocatorType::Stack;
    stack_alloc.Allocate   = Stack_Allocate;
    stack_alloc.AllocateNZ = Stack_AllocateNZ;
    return stack_alloc;
}

void DestroyStack(Allocator* stack_alloc)
{
    CTK_ASSERT(stack_alloc->type == AllocatorType::Stack);

    auto stack = (Stack*)stack_alloc->context;
    Deallocate(stack_alloc->parent, stack->mem);
    *stack = {};
}

void Clear(Allocator* stack_alloc)
{
    CTK_ASSERT(stack_alloc->type == AllocatorType::Stack);

    auto stack = (Stack*)stack_alloc->context;
    stack->count = 0;
}
