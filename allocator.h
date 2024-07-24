/// Data
////////////////////////////////////////////////////////////
enum struct AllocatorType
{
    STD,
    FreeList,
    Stack,
    Frame,
};

struct Allocator;
struct Allocator
{
    AllocatorType                               type;
    void*                                       context;
    Allocator*                                  parent;
    Func<uint8*, Allocator*, uint32, uint32>         Allocate;
    Func<uint8*, Allocator*, uint32, uint32>         AllocateNZ;
    Func<uint8*, Allocator*, void*,  uint32, uint32> Reallocate;
    Func<uint8*, Allocator*, void*,  uint32, uint32> ReallocateNZ;
    Func<void,   Allocator*, void*>                  Deallocate;
};

/// Allocator Interface
////////////////////////////////////////////////////////////
uint8* AllocateNZ(Allocator* allocator, uint32 size, uint32 alignment)
{
    CTK_ASSERT(allocator->AllocateNZ != NULL);
    return allocator->AllocateNZ(allocator, size, alignment);
}

template<typename Type>
Type* AllocateNZ(Allocator* allocator, uint32 count, uint32 alignment)
{
    return (Type*)AllocateNZ(allocator, SizeOf32<Type>() * count, alignment);
}

template<typename Type>
Type* AllocateNZ(Allocator* allocator, uint32 count)
{
    return AllocateNZ<Type>(allocator, count, alignof(Type));
}

uint8* Allocate(Allocator* allocator, uint32 size, uint32 alignment)
{
    CTK_ASSERT(allocator->Allocate != NULL);
    return allocator->Allocate(allocator, size, alignment);
}

template<typename Type>
Type* Allocate(Allocator* allocator, uint32 count, uint32 alignment)
{
    return (Type*)Allocate(allocator, SizeOf32<Type>() * count, alignment);
}

template<typename Type>
Type* Allocate(Allocator* allocator, uint32 count)
{
    return Allocate<Type>(allocator, count, alignof(Type));
}

uint8* ReallocateNZ(Allocator* allocator, void* mem, uint32 new_size, uint32 alignment)
{
    CTK_ASSERT(allocator->ReallocateNZ != NULL);
    return allocator->ReallocateNZ(allocator, mem, new_size, alignment);
}

template<typename Type>
Type* ReallocateNZ(Allocator* allocator, Type* mem, uint32 new_count, uint32 alignment)
{
    return (Type*)ReallocateNZ(allocator, (void*)mem, SizeOf32<Type>() * new_count, alignment);
}

template<typename Type>
Type* ReallocateNZ(Allocator* allocator, Type* mem, uint32 new_count)
{
    return ReallocateNZ(allocator, mem, new_count, alignof(Type));
}

uint8* Reallocate(Allocator* allocator, void* mem, uint32 new_size, uint32 alignment)
{
    CTK_ASSERT(allocator->Reallocate != NULL);
    return allocator->Reallocate(allocator, mem, new_size, alignment);
}

template<typename Type>
Type* Reallocate(Allocator* allocator, Type* mem, uint32 new_count, uint32 alignment)
{
    return (Type*)Reallocate(allocator, (void*)mem, SizeOf32<Type>() * new_count, alignment);
}

template<typename Type>
Type* Reallocate(Allocator* allocator, Type* mem, uint32 new_count)
{
    return Reallocate(allocator, mem, new_count, alignof(Type));
}

void Deallocate(Allocator* allocator, void* mem)
{
    CTK_ASSERT(allocator->Deallocate != NULL);
    allocator->Deallocate(allocator, mem);
}
