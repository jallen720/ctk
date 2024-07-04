/// Data
////////////////////////////////////////////////////////////
struct Stack
{
    Allocator allocator;

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
static uint8* AllocateNZ(Stack* stack, uint32 size, uint32 alignment);
static uint8* Allocate(Stack* stack, uint32 size, uint32 alignment);

static uint8* Stack_AllocateNZ(void* data, uint32 size, uint32 alignment)
{
    return AllocateNZ((Stack*)data, size, alignment);
}

static uint8* Stack_Allocate(void* data, uint32 size, uint32 alignment)
{
    return Allocate((Stack*)data, size, alignment);
}

static constexpr Allocator STACK_ALLOCATOR =
{
    .AllocateNZ   = Stack_AllocateNZ,
    .Allocate     = Stack_Allocate,
    .ReallocateNZ = NULL,
    .Reallocate   = NULL,
    .Deallocate   = NULL,
};

/// Interface
////////////////////////////////////////////////////////////
static Stack CreateStack(Allocator* allocator, uint32 size)
{
    CTK_ASSERT(size > 0);

    Stack stack = {};
    stack.allocator = STACK_ALLOCATOR;
    stack.parent    = NULL;
    stack.mem       = Allocate<uint8>(allocator, size);
    stack.size      = size;
    stack.count     = 0;
    return stack;
}

static void DestroyStack(Stack* stack, Allocator* allocator)
{
    Deallocate(allocator, stack->mem);
    *stack = {};
}

static void Clear(Stack* stack)
{
    stack->count = 0;
}

static uint8* AllocateNZ(Stack* stack, uint32 size, uint32 alignment)
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
static Type* AllocateNZ(Stack* stack, uint32 count, uint32 alignment)
{
    return (Type*)AllocateNZ(stack, SizeOf32<Type>() * count, alignment);
}

template<typename Type>
static Type* AllocateNZ(Stack* stack, uint32 count)
{
    return AllocateNZ<Type>(stack, count, alignof(Type));
}

static uint8* Allocate(Stack* stack, uint32 size, uint32 alignment)
{
    uint8* allocated_mem = AllocateNZ(stack, size, alignment);
    memset(allocated_mem, 0, size);
    return allocated_mem;
}

template<typename Type>
static Type* Allocate(Stack* stack, uint32 count, uint32 alignment)
{
    return (Type*)Allocate(stack, SizeOf32<Type>() * count, alignment);
}

template<typename Type>
static Type* Allocate(Stack* stack, uint32 count)
{
    return Allocate<Type>(stack, count, alignof(Type));
}

static Stack CreateFrame(Stack* parent)
{
    uint32 size = parent->size - parent->count;
    return
    {
        .allocator = STACK_ALLOCATOR,
        .parent    = parent,
        .mem       = Allocate<uint8>(parent, size),
        .size      = size,
        .count     = 0,
    };
}

