/// STD Allocator
////////////////////////////////////////////////////////////
uint8* STD_AllocateNZ(void* context, uint32 size, uint32 alignment)
{
    CTK_ASSERT(size > 0);
    CTK_UNUSED(context);

    return (uint8*)_aligned_malloc(size, alignment);
}

uint8* STD_Allocate(void* context, uint32 size, uint32 alignment)
{
    uint8* allocated_mem = STD_AllocateNZ(context, size, alignment);
    memset(allocated_mem, 0, size);
    return allocated_mem;
}

uint8* STD_ReallocateNZ(void* context, void* mem, uint32 new_size, uint32 alignment)
{
    CTK_UNUSED(context);

    return (uint8*)_aligned_realloc(mem, new_size, alignment);
}

void STD_Deallocate(void* context, void* mem)
{
    CTK_UNUSED(context);

    _aligned_free(mem);
}

Allocator g_std_allocator =
{
    .type         = AllocatorType::STD,
    .context      = NULL,
    .parent       = NULL,
    .Allocate     = STD_Allocate,
    .AllocateNZ   = STD_AllocateNZ,
    .Reallocate   = NULL,
    .ReallocateNZ = STD_ReallocateNZ,
    .Deallocate   = STD_Deallocate,
};

/// Frame Allocator
////////////////////////////////////////////////////////////
