/// Data
////////////////////////////////////////////////////////////
template<typename Type>
struct RingBuffer
{
    Type*  data;
    uint32 size;
    uint32 index;
};

/// CTK_ITER Interface
////////////////////////////////////////////////////////////
template<typename Type>
static Type* IterStart(RingBuffer<Type>* ring_buffer)
{
    return ring_buffer->data;
}

template<typename Type>
static Type* IterEnd(RingBuffer<Type>* ring_buffer)
{
    return ring_buffer->data + ring_buffer->size;
}

/// Interface
////////////////////////////////////////////////////////////
template<typename Type>
static RingBuffer<Type> CreateRingBuffer(Allocator* allocator, uint32 size)
{
    RingBuffer<Type> ring_buffer = {};
    ring_buffer.data  = size > 0 ? Allocate<Type>(allocator, size) : NULL;
    ring_buffer.size  = size;
    ring_buffer.index = 0;
    return ring_buffer;
}

template<typename Type>
static void DestroyRingBuffer(RingBuffer<Type>* ring_buffer, Allocator* allocator)
{
    if (ring_buffer->data != NULL)
    {
        Deallocate(allocator, ring_buffer->data);
    }
    *ring_buffer = {};
}

template<typename Type>
static Type* Next(RingBuffer<Type>* ring_buffer)
{
    ring_buffer->index += 1;
    if (ring_buffer->index >= ring_buffer->size)
    {
        ring_buffer->index = 0;
    }
    return ring_buffer->data + ring_buffer->index;
}

template<typename Type>
static Type* GetCurrentPtr(RingBuffer<Type>* ring_buffer)
{
    return ring_buffer->data + ring_buffer->index;
}

template<typename Type>
static Type GetCurrent(RingBuffer<Type>* ring_buffer)
{
    return ring_buffer->data[ring_buffer->index];
}

