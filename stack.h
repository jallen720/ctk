/// Data
////////////////////////////////////////////////////////////
struct Stack {
    Allocator  allocator;
    Allocator* parent;

    uint8*     mem;
    uint32     size;
    uint32     count;
    uint32     reserve_start_index;
};

/// Utils
////////////////////////////////////////////////////////////
uint32 GetAlignedIndex(Stack* stack, uint32 alignment) {
    return Align(stack->mem + stack->count, alignment) - stack->mem;
}

/// Interface
////////////////////////////////////////////////////////////
uint8* AllocateNZ(Stack* stack, uint32 size, uint32 alignment) {
    CTK_ASSERT(size > 0);

    uint32 aligned_index = GetAlignedIndex(stack, alignment);
    if (aligned_index + size > stack->size) {
        CTK_FATAL("cannot allocate %u bytes from stack at %u-byte aligned address at index %u; allocation would exceed "
                  "stack size of %u",
                  size, alignment, aligned_index, stack->size);
    }

    stack->count = aligned_index + size;
    return &stack->mem[aligned_index];
}

uint8* Allocate(Stack* stack, uint32 size, uint32 alignment) {
    uint8* allocated_mem = AllocateNZ(stack, size, alignment);
    memset(allocated_mem, 0, size);
    return allocated_mem;
}

template<typename Type>
Type* Allocate(Stack* stack, uint32 count) {
    return (Type*)Allocate(stack, count * sizeof(Type), alignof(Type));
}

uint8* Stack_AllocateNZ(Allocator* allocator, uint32 size, uint32 alignment) {
    return AllocateNZ((Stack*)allocator, size, alignment);
}

uint8* Stack_Allocate(Allocator* allocator, uint32 size, uint32 alignment) {
    return Allocate((Stack*)allocator, size, alignment);
}

Stack CreateStack(Allocator* parent, uint32 size) {
    CTK_ASSERT(size > 0);

    Stack stack = {};
    stack.allocator.Allocate   = Stack_Allocate;
    stack.allocator.AllocateNZ = Stack_AllocateNZ;
    stack.parent               = parent;
    stack.mem                  = Allocate<uint8>(parent, size);
    stack.size                 = size;
    stack.count                = 0;
    stack.reserve_start_index  = UINT32_MAX;
    return stack;
}

void DestroyStack(Stack* stack) {
    Deallocate(stack->parent, stack->mem);
    *stack = {};
}

void Clear(Stack* stack) {
    stack->count = 0;
}

template<typename Type>
void Reserve(Stack* stack, Type** data, uint32* size) {
    if (stack->reserve_start_index != UINT32_MAX) {
        CTK_FATAL("can't reserve remaining stack memory; it has already been reserved");
    }

    uint32 aligned_index = GetAlignedIndex(stack, alignof(Type));
    uint32 aligned_alloc_size = stack->size - aligned_index;
    if (aligned_alloc_size == 0) {
        CTK_FATAL("can't reserve %u-byte aligned memory from stack; stack needs enough space for atleast 1 "
                  "element of size %u, but only has %u bytes remaining",
                  alignof(Type),
                  sizeof(Type),
                  aligned_alloc_size);
    }

    stack->reserve_start_index = aligned_index;
    stack->count = aligned_index + aligned_alloc_size;
    *size = aligned_alloc_size / sizeof(Type);
    *data = (Type*)&stack->mem[aligned_index];
}

void Commit(Stack* stack, uint32 elem_size, uint32 used_size) {
    if (stack->reserve_start_index == UINT32_MAX) {
        CTK_FATAL("Commit() failed; Reserve() not called first");
    }

    uint32 new_count = stack->reserve_start_index + (used_size * elem_size);
    CTK_ASSERT(new_count <= stack->size);

    stack->count = new_count;
    stack->reserve_start_index = UINT32_MAX;
}

