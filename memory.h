#pragma once

#include "ctk/ctk.h"

namespace ctk {

struct region {
    u8* Memory;
    u64 Size;
    region* Next;
};

struct heap {
    u8* Memory;
    u64 Size;
    array<region> RegionPool;
    region* Used;
    region* Free;
    region* Available;
};

static heap _MAIN_HEAP;

static void visualize_main_heap();

static region* create_region(u8* Memory, u64 Size, region* Next = NULL) {
    region* Region = NULL;
    if(_MAIN_HEAP.Available) {
        Region = _MAIN_HEAP.Available;
        _MAIN_HEAP.Available = _MAIN_HEAP.Available->Next;
    } else {
        Region = push(&_MAIN_HEAP.RegionPool);
    }
    Region->Memory = Memory;
    Region->Size = Size;
    Region->Next = Next;
    return Region;
}

static void allocate_main_heap(u64 Size, u32 MaxRegionCount) {
    _MAIN_HEAP = {};
    _MAIN_HEAP.Memory = (u8*)malloc(Size);
    CTK_ASSERT(_MAIN_HEAP.Memory != NULL);
    _MAIN_HEAP.Size = Size;
    _MAIN_HEAP.RegionPool = create_array_empty<region>(MaxRegionCount);
    _MAIN_HEAP.Free = create_region(_MAIN_HEAP.Memory, Size);
    ctk::info("RegionPool.Count %u", _MAIN_HEAP.RegionPool.Count);
}

static u8* allocate(u64 Size) {
    // Find region that is large enough to be allocated from.
    region* SelectedRegionParent = NULL;
    region* SelectedRegion = _MAIN_HEAP.Free;
    while(SelectedRegion) {
        if(SelectedRegion->Size >= Size) {
            break;
        }
        SelectedRegionParent = SelectedRegion;
        SelectedRegion = SelectedRegion->Next;
    }
    if(SelectedRegion == NULL) {
        CTK_FATAL("no regions available that can allocate %u bytes", Size);
    }

    // Split region and move allocation region to _MAIN_HEAP.Used.
    _MAIN_HEAP.Used = create_region(SelectedRegion->Memory, Size, _MAIN_HEAP.Used);
    region* NewFree = Size < SelectedRegion->Size ? create_region(SelectedRegion->Memory + Size, SelectedRegion->Size - Size, SelectedRegion->Next) : SelectedRegion->Next;
    if(SelectedRegionParent == NULL) {
        _MAIN_HEAP.Free = NewFree;
    } else {
        SelectedRegionParent->Next = NewFree;
    }
    SelectedRegion->Next = _MAIN_HEAP.Available;
    _MAIN_HEAP.Available = SelectedRegion;

    return _MAIN_HEAP.Used->Memory;
}

static void ctk_free(void* Memory) {
    CTK_ASSERT(Memory)

    // Find region associated with memory.
    region* UsedRegionParent = NULL;
    region* UsedRegion = _MAIN_HEAP.Used;
    while(UsedRegion) {
        if(UsedRegion->Memory == Memory) {
            break;
        }
        UsedRegionParent = UsedRegion;
        UsedRegion = UsedRegion->Next;
    }
    if(UsedRegion == NULL) {
        CTK_FATAL("failed to find region associated with pointer %p to be freed", Memory);
    }
    if(UsedRegionParent) {
        UsedRegionParent->Next = UsedRegion->Next;
    } else {
        _MAIN_HEAP.Used = UsedRegion->Next;
    }

    // Find where to insert (or merge) region.
    region* FreeRegionParent = NULL;
    region* FreeRegion = _MAIN_HEAP.Free;
    while(1) {
        // No region (located after freed region) was found, so freed region needs to be added to the end of the list.
        if(FreeRegion == NULL) {
            if(FreeRegionParent) {
                FreeRegionParent->Next = UsedRegion;
            } else {
                _MAIN_HEAP.Free = UsedRegion;
            }
            UsedRegion->Next = NULL;
            break;
        }

        // Region (located after freed region) found.
        if(FreeRegion->Memory > Memory) {
            if(FreeRegionParent) {
                FreeRegionParent->Next = UsedRegion;
            } else {
                _MAIN_HEAP.Free = UsedRegion;
            }
            UsedRegion->Next = FreeRegion;
            break;
        }

        FreeRegionParent = FreeRegion;
        FreeRegion = FreeRegion->Next;
    }

    // Merge adjacent regions that would form a contiguous region.
    if(UsedRegion->Next && UsedRegion->Memory + UsedRegion->Size == UsedRegion->Next->Memory) {
        UsedRegion->Size += UsedRegion->Next->Size;
        region* NewNext = UsedRegion->Next->Next;
        UsedRegion->Next->Next = _MAIN_HEAP.Available;
        _MAIN_HEAP.Available = UsedRegion->Next;
        UsedRegion->Next = NewNext;
    }
    if(FreeRegionParent && FreeRegionParent->Memory + FreeRegionParent->Size == FreeRegionParent->Next->Memory) {
        FreeRegionParent->Size += FreeRegionParent->Next->Size;
        region* NewNext = FreeRegionParent->Next->Next;
        FreeRegionParent->Next->Next = _MAIN_HEAP.Available;
        _MAIN_HEAP.Available = FreeRegionParent->Next;
        FreeRegionParent->Next = NewNext;
    }
}

//template<typename type>
//static type* allocate(u32 Count) {
//    return (type*)allocate(Count * sizeof(type));
//}

static void visualize_main_heap() {
    static char* Display = NULL;
    if(Display == NULL) {
        Display = (char*)malloc(_MAIN_HEAP.Size + 1);
        memset(Display, 0, _MAIN_HEAP.Size + 1);
    }

    ctk::print_line("\n_MAIN_HEAP ===========================");
    region* Region = _MAIN_HEAP.Used;
    ctk::print_line("used");
    while(Region) {
        u64 Offset = Region->Memory - _MAIN_HEAP.Memory;
        ctk::print_line(1, "{ .Offset = %llu, .Size = %u }", Offset, Region->Size);
        memset(Display + (Offset), 'X', Region->Size);
        Region = Region->Next;
    }

    Region = _MAIN_HEAP.Free;
    ctk::print_line("free");
    while(Region) {
        u64 Offset = Region->Memory - _MAIN_HEAP.Memory;
        ctk::print_line(1, "{ .Offset = %llu, .Size = %u }", Offset, Region->Size);
        memset(Display + (Offset), '_', Region->Size);
        Region = Region->Next;
    }

    Region = _MAIN_HEAP.Available;
    ctk::print_line("available");
    while(Region) {
        u64 Offset = Region->Memory - _MAIN_HEAP.Memory;
        ctk::print_line(1, "{ .Offset = %llu, .Size = %u }", Offset, Region->Size);
        Region = Region->Next;
    }

    ctk::print_line("region pool: %u", _MAIN_HEAP.RegionPool.Count);
    ctk::print_line("[%s]", Display);
}

}
