/// Data
////////////////////////////////////////////////////////////
template<typename Type>
struct PoolHnd
{
    uint32 id;
};

template<typename Type>
union PoolNode
{
    Type          data;
    PoolHnd<Type> next_free;
};

template<typename Type>
struct Pool
{
    PoolNode<Type>* nodes;
    uint32          size;
    PoolHnd<Type>   next_free;
};

constexpr uint32 NULL_HND = 0;

/// Utils
////////////////////////////////////////////////////////////
uint32 GetIndex(uint32 hnd_id)
{
    CTK_ASSERT(hnd_id != 0);

    return hnd_id - 1;
}

template<typename Type>
uint32 GetHndIndex(PoolHnd<Type> hnd)
{
    return GetIndex(hnd.id);
}

/// Interface
////////////////////////////////////////////////////////////
template<typename Type>
void InitPool(Pool<Type>* pool, Allocator* allocator, uint32 size)
{
}

template<typename Type>
void DeinitPool(Pool<Type>* pool, Allocator* allocator)
{
    Deallocate(allocator, pool->nodes);
    pool->size         = 0;
    pool->next_free.id = NULL_HND;
}

template<typename Type>
Pool<Type> CreatePool(Allocator* allocator, uint32 size)
{
    CTK_ASSERT(size > 0);

    Pool<Type> pool = {};
    pool.nodes = Allocate<PoolNode<Type>>(allocator, size);
    pool.size  = size;

    // Point each node (except last) in pool to next node. Pool handle IDs are index + 1.
    pool.next_free = {1};
    for (uint32 id = 1; id < pool.size; ++id)
    {
        pool.nodes[GetIndex(id)].next_free = {id + 1};
    }

    return pool;
}

template<typename Type>
void DestroyPool(Pool<Type>* pool, Allocator* allocator)
{
    Deallocate(allocator, pool->nodes);
    Deallocate(allocator, pool);
}

template<typename Type>
bool IsNull(PoolHnd<Type> hnd)
{
    return hnd.id == NULL_HND;
}

template<typename Type>
bool IsEqual(PoolHnd<Type> a, PoolHnd<Type> b)
{
    return a.id == b.id;
}

template<typename Type>
bool IsFree(Pool<Type>* pool, PoolHnd<Type> hnd)
{
    PoolHnd<Type> next_free = pool->next_free;
    while (!IsNull(next_free))
    {
        if (IsEqual(next_free, hnd))
        {
            return true;
        }
        next_free = pool->nodes[GetHndIndex(next_free)].next_free;
    }

    return false;
}

template<typename Type>
Type* GetData(Pool<Type>* pool, PoolHnd<Type> hnd)
{
    uint32 index = GetHndIndex(hnd);

    if (index >= pool->size)
    {
        CTK_FATAL("can't get data from handle: handle index (%u) exceeds pool size (%u)", index, pool->size);
    }

    if (IsFree(pool, hnd))
    {
        CTK_FATAL("can't get data from handle: handle index (%u) is not allocated", index);
    }

    return &pool->nodes[index].data;
}

template<typename Type>
PoolHnd<Type> Allocate(Pool<Type>* pool)
{
    if (IsNull(pool->next_free))
    {
        CTK_FATAL("can't allocate from pool: pool has no free nodes");
    }

    PoolHnd<Type> hnd = pool->next_free;
    PoolNode<Type>* node = pool->nodes + GetHndIndex(hnd);
    pool->next_free = node->next_free;
    memset(&node->data, 0, sizeof(Type));
    return hnd;
}

template<typename Type>
void Deallocate(Pool<Type>* pool, PoolHnd<Type> hnd)
{
    uint32 index = GetHndIndex(hnd);

    if (index >= pool->size)
    {
        CTK_FATAL("can't deallocate handle from pool: handle index (%u) exceeds pool size (%u)", index, pool->size);
    }

    if (IsFree(pool, hnd))
    {
        CTK_FATAL("can't deallocate handle from pool: handle index (%u) is already deallocated", index);
    }

    // Push data's node to top of free list.
    PoolNode<Type>* node = pool->nodes + index;
    node->next_free = pool->next_free;
    pool->next_free = hnd;
}

