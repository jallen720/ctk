/// Data
////////////////////////////////////////////////////////////
struct Stack
{
    Allocator  allocator;
    Allocator* parent_allocator;

    Stack* parent;
    uint8* mem;
    uint32 size;
    uint32 count;

    ~Stack()
    {
        // "Deallocate" stack from parent stack.
        if (parent != NULL)
        {
            parent->count -= this->size;
        }
    }
};

/// Allocator
////////////////////////////////////////////////////////////
uint8* AllocateNZ(Stack* stack, uint32 size, uint32 alignment);
uint8* Allocate(Stack* stack, uint32 size, uint32 alignment);

uint8* Stack_AllocateNZ(void* data, uint32 size, uint32 alignment)
{
    return AllocateNZ((Stack*)data, size, alignment);
}

uint8* Stack_Allocate(void* data, uint32 size, uint32 alignment)
{
    return Allocate((Stack*)data, size, alignment);
}

constexpr Allocator STACK_ALLOCATOR =
{
    .AllocateNZ   = Stack_AllocateNZ,
    .Allocate     = Stack_Allocate,
    .ReallocateNZ = NULL,
    .Reallocate   = NULL,
    .Deallocate   = NULL,
};

/// Interface
////////////////////////////////////////////////////////////
Stack CreateStack(Allocator* allocator, uint32 size)
{
    CTK_ASSERT(size > 0);

    Stack stack = {};
    stack.allocator        = STACK_ALLOCATOR;
    stack.parent_allocator = allocator;
    stack.parent           = NULL;
    stack.mem              = Allocate<uint8>(allocator, size);
    stack.size             = size;
    stack.count            = 0;
    return stack;
}

void DestroyStack(Stack* stack)
{
    Deallocate(stack->parent_allocator, stack->mem);
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

Stack CreateFrame(Stack* parent)
{
    uint32 size = parent->size - parent->count;

    Stack frame = {};
    frame.allocator = STACK_ALLOCATOR;
    frame.parent    = parent;
    frame.mem       = Allocate<uint8>(parent, size);
    frame.size      = size;
    frame.count     = 0;
    return frame;
}

