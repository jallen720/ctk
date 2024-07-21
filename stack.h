/// Data
////////////////////////////////////////////////////////////
struct Stack
{
    Allocator* allocator;
    uint8*     mem;
    uint32     size;
    uint32     count;
};

/// Interface
////////////////////////////////////////////////////////////
Stack CreateStack(Allocator* allocator, uint32 size)
{
    CTK_ASSERT(size > 0);

    Stack stack = {};
    stack.allocator = allocator;
    stack.mem       = Allocate<uint8>(allocator, size);
    stack.size      = size;
    stack.count     = 0;
    return stack;
}

void DestroyStack(Stack* stack)
{
    Deallocate(stack->allocator, stack->mem);
    *stack = {};
}

void Clear(Stack* stack)
{
    stack->count = 0;
}

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

template<typename Type>
Type* AllocateNZ(Stack* stack, uint32 count, uint32 alignment)
{
    return (Type*)AllocateNZ(stack, SizeOf32<Type>() * count, alignment);
}

template<typename Type>
Type* AllocateNZ(Stack* stack, uint32 count)
{
    return AllocateNZ<Type>(stack, count, alignof(Type));
}

uint8* Allocate(Stack* stack, uint32 size, uint32 alignment)
{
    uint8* allocated_mem = AllocateNZ(stack, size, alignment);
    memset(allocated_mem, 0, size);
    return allocated_mem;
}

template<typename Type>
Type* Allocate(Stack* stack, uint32 count, uint32 alignment)
{
    return (Type*)Allocate(stack, SizeOf32<Type>() * count, alignment);
}

template<typename Type>
Type* Allocate(Stack* stack, uint32 count)
{
    return Allocate<Type>(stack, count, alignof(Type));
}

/// Allocator
////////////////////////////////////////////////////////////
uint8* Stack_AllocateNZ(void* context, uint32 size, uint32 alignment)
{
    return AllocateNZ((Stack*)context, size, alignment);
}

uint8* Stack_Allocate(void* context, uint32 size, uint32 alignment)
{
    return Allocate((Stack*)context, size, alignment);
}

Allocator CreateStackAllocator(Allocator* allocator, uint32 size)
{
    Stack* stack = Allocate<Stack>(allocator, 1);
    *stack = CreateStack(allocator, size);

    Allocator stack_allocator =
    stack_allocator.allocator  = allocator;
    stack_allocator.context    = (void*)stack;
    stack_allocator.type       = AllocatorType::Stack;
    stack_allocator.AllocateNZ = Stack_AllocateNZ;
    stack_allocator.Allocate   = Stack_Allocate;
    return stack_allocator;
}

void DestroyStackAllocator(Allocator* allocator)
{
    CTK_ASSERT(allocator->type == AllocatorType::Stack);
    DestroyStack((Stack*)allocator->context);
}
