/// Data
////////////////////////////////////////////////////////////
struct Allocator
{
    Func<uint8*, void*, uint32, uint32>        AllocateNZ;
    Func<uint8*, void*, uint32, uint32>        Allocate;
    Func<uint8*, void*, void*, uint32, uint32> ReallocateNZ;
    Func<uint8*, void*, void*, uint32, uint32> Reallocate;
    Func<void, void*, void*>                   Deallocate;
};

/// Allocator Interface
////////////////////////////////////////////////////////////
static uint8* AllocateNZ(Allocator* allocator, uint32 size, uint32 alignment)
{
    CTK_ASSERT(allocator->AllocateNZ != NULL);
    return allocator->AllocateNZ((void*)allocator, size, alignment);
}

template<typename Type>
static Type* AllocateNZ(Allocator* allocator, uint32 count, uint32 alignment)
{
    return (Type*)AllocateNZ(allocator, SizeOf32<Type>() * count, alignment);
}

template<typename Type>
static Type* AllocateNZ(Allocator* allocator, uint32 count)
{
    return AllocateNZ<Type>(allocator, count, alignof(Type));
}

static uint8* Allocate(Allocator* allocator, uint32 size, uint32 alignment)
{
    CTK_ASSERT(allocator->Allocate != NULL);
    return allocator->Allocate((void*)allocator, size, alignment);
}

template<typename Type>
static Type* Allocate(Allocator* allocator, uint32 count, uint32 alignment)
{
    return (Type*)Allocate(allocator, SizeOf32<Type>() * count, alignment);
}

template<typename Type>
static Type* Allocate(Allocator* allocator, uint32 count)
{
    return Allocate<Type>(allocator, count, alignof(Type));
}

static uint8* ReallocateNZ(Allocator* allocator, void* mem, uint32 new_size, uint32 alignment)
{
    CTK_ASSERT(allocator->ReallocateNZ != NULL);
    return allocator->ReallocateNZ((void*)allocator, mem, new_size, alignment);
}

template<typename Type>
static Type* ReallocateNZ(Allocator* allocator, Type* mem, uint32 new_count, uint32 alignment)
{
    return (Type*)ReallocateNZ(allocator, (void*)mem, SizeOf32<Type>() * new_count, alignment);
}

template<typename Type>
static Type* ReallocateNZ(Allocator* allocator, Type* mem, uint32 new_count)
{
    return ReallocateNZ(allocator, mem, new_count, alignof(Type));
}

static uint8* Reallocate(Allocator* allocator, void* mem, uint32 new_size, uint32 alignment)
{
    CTK_ASSERT(allocator->Reallocate != NULL);
    return allocator->Reallocate((void*)allocator, mem, new_size, alignment);
}

template<typename Type>
static Type* Reallocate(Allocator* allocator, Type* mem, uint32 new_count, uint32 alignment)
{
    return (Type*)Reallocate(allocator, (void*)mem, SizeOf32<Type>() * new_count, alignment);
}

template<typename Type>
static Type* Reallocate(Allocator* allocator, Type* mem, uint32 new_count)
{
    return Reallocate(allocator, mem, new_count, alignof(Type));
}

static void Deallocate(Allocator* allocator, void* mem)
{
    CTK_ASSERT(allocator->Deallocate != NULL);
    allocator->Deallocate((void*)allocator, mem);
}

/// STD Allocation Interface
////////////////////////////////////////////////////////////
static uint8* AllocateNZ(uint32 size, uint32 alignment)
{
    CTK_ASSERT(size > 0);
    return (uint8*)_aligned_malloc(size, alignment);;
}

template<typename Type>
static Type* AllocateNZ(uint32 count, uint32 alignment)
{
    return (Type*)AllocateNZ(SizeOf32<Type>() * count, alignment);
}

template<typename Type>
static Type* AllocateNZ(uint32 count)
{
    return AllocateNZ<Type>(count, alignof(Type));
}

static uint8* Allocate(uint32 size, uint32 alignment)
{
    uint8* allocated_mem = AllocateNZ(size, alignment);
    memset(allocated_mem, 0, size);
    return allocated_mem;
}

template<typename Type>
static Type* Allocate(uint32 count, uint32 alignment)
{
    return (Type*)Allocate(SizeOf32<Type>() * count, alignment);
}

template<typename Type>
static Type* Allocate(uint32 count)
{
    return Allocate<Type>(count, alignof(Type));
}

static uint8* ReallocateNZ(void* mem, uint32 new_size, uint32 alignment)
{
    return (uint8*)_aligned_realloc(mem, new_size, alignment);
}

template<typename Type>
static Type* ReallocateNZ(Type* mem, uint32 new_count, uint32 alignment)
{
    return (Type*)ReallocateNZ((void*)mem, SizeOf32<Type>() * new_count, alignment);
}

template<typename Type>
static Type* ReallocateNZ(Type* mem, uint32 new_count)
{
    return ReallocateNZ(mem, new_count, alignof(Type));
}

static void Deallocate(void* mem)
{
    _aligned_free(mem);
}

/// STD Allocator
////////////////////////////////////////////////////////////
static uint8* STD_AllocateNZ(void* data, uint32 size, uint32 alignment)
{
    CTK_UNUSED(data);
    return AllocateNZ(size, alignment);
}

static uint8* STD_Allocate(void* data, uint32 size, uint32 alignment)
{
    CTK_UNUSED(data);
    return Allocate(size, alignment);
}

static uint8* STD_ReallocateNZ(void* data, void* mem, uint32 new_size, uint32 alignment)
{
    CTK_UNUSED(data);
    return ReallocateNZ(mem, new_size, alignment);
}

static void STD_Deallocate(void* data, void* mem)
{
    CTK_UNUSED(data);
    Deallocate(mem);
}

static Allocator std_allocator =
{
    .AllocateNZ   = STD_AllocateNZ,
    .Allocate     = STD_Allocate,
    .ReallocateNZ = STD_ReallocateNZ,
    .Reallocate   = NULL,
    .Deallocate   = STD_Deallocate,
};

