/// Data
////////////////////////////////////////////////////////////
struct Stack
{
    Allocator  allocator;
    Allocator* parent;

    uint8*     mem;
    uint32     size;
    uint32     count;
};

/// Interface
////////////////////////////////////////////////////////////
uint8* AllocateNZ(Stack* stack, uint32 size, uint32 alignment)
{
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

uint8* Allocate(Stack* stack, uint32 size, uint32 alignment)
{
    uint8* allocated_mem = AllocateNZ(stack, size, alignment);
    memset(allocated_mem, 0, size);
    return allocated_mem;
}

uint8* Stack_AllocateNZ(Allocator* allocator, uint32 size, uint32 alignment)
{
    return AllocateNZ((Stack*)allocator, size, alignment);
}

uint8* Stack_Allocate(Allocator* allocator, uint32 size, uint32 alignment)
{
    return Allocate((Stack*)allocator, size, alignment);
}

Stack CreateStack(Allocator* parent, uint32 size)
{
    CTK_ASSERT(size > 0);

    Stack stack = {};
    stack.allocator.Allocate   = Stack_Allocate;
    stack.allocator.AllocateNZ = Stack_AllocateNZ;
    stack.parent               = parent;
    stack.mem                  = Allocate<uint8>(parent, size);
    stack.size                 = size;
    stack.count                = 0;
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
