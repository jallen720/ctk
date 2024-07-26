/// Data
////////////////////////////////////////////////////////////
struct Stack : public Allocator
{
    uint8* mem;
    uint32 size;
    uint32 count;
};

/// Interface
////////////////////////////////////////////////////////////
uint8* Stack_AllocateNZ(Allocator* allocator, uint32 size, uint32 alignment)
{
    auto stack = (Stack*)allocator;
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

uint8* Stack_Allocate(Allocator* allocator, uint32 size, uint32 alignment)
{
    uint8* allocated_mem = Stack_AllocateNZ(allocator, size, alignment);
    memset(allocated_mem, 0, size);
    return allocated_mem;
}

Stack CreateStack(Allocator* parent, uint32 size)
{
    CTK_ASSERT(size > 0);

    Stack stack = {};
    stack.parent     = parent;
    stack.Allocate   = Stack_Allocate;
    stack.AllocateNZ = Stack_AllocateNZ;
    stack.mem        = Allocate<uint8>(parent, size);
    stack.size       = size;
    stack.count      = 0;
    return stack;
}

void DestroyStack(Stack* stack)
{
    Deallocate(stack->parent, stack->mem);
    *stack = {};
}

void Clear(Stack* stack)
{
    stack->count = 0;
}
