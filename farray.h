/// Data
////////////////////////////////////////////////////////////
template<typename Type, uint32 size>
struct FArray
{
    Type   data[size];
    uint32 count;
};

/// CTK_ITER Interface
////////////////////////////////////////////////////////////
template<typename Type, uint32 size>
Type* IterStart(FArray<Type, size>* array)
{
    return array->data;
}

template<typename Type, uint32 size>
Type* IterEnd(FArray<Type, size>* array)
{
    return array->data + array->count;
}

/// Interface
////////////////////////////////////////////////////////////
template<typename Type, uint32 size>
bool CanPush(FArray<Type, size>* array, uint32 count)
{
    return array->count + count <= size;
}

template<typename Type, uint32 size>
Type* Push(FArray<Type, size>* array, Type elem)
{
    if (array->count == size)
    {
        CTK_FATAL("can't push element to array: no space available");
    }

    Type* new_elem = array->data + array->count;
    *new_elem = elem;
    array->count += 1;
    return new_elem;
}

template<typename Type, uint32 size>
Type* Push(FArray<Type, size>* array)
{
    return Push(array, {});
}

template<typename Type, uint32 size>
void PushRange(FArray<Type, size>* array, const Type* elems, uint32 elem_count)
{
    if (elem_count == 0)
    {
        return;
    }

    uint32 available_space = size - array->count;
    if (available_space < elem_count)
    {
        CTK_FATAL("can't push %u elements to array: array has %u available slots", elem_count, available_space);
    }

    memcpy(array->data + array->count, elems, elem_count * sizeof(Type));
    array->count += elem_count;
}

template<typename Type, uint32 size>
void PushRange(FArray<Type, size>* array, FArray<Type, size>* other)
{
    PushRange(array, other->data, other->count);
}

template<typename Type, uint32 size>
void Remove(FArray<Type, size>* array, uint32 index)
{
    CTK_ASSERT(index < array->count);

    memmove(array->data + index, array->data + index + 1, (array->count - index - 1) * sizeof(Type));
    --array->count;
}

template<typename Type, uint32 size>
void RemoveRange(FArray<Type, size>* array, uint32 index, uint32 count)
{
    CTK_ASSERT(index < array->count);
    CTK_ASSERT(index + count <= array->count);

    memmove(array->data + index, array->data + index + count, (array->count - index - count) * sizeof(Type));
    array->count -= count;
}

template<typename Type, uint32 size>
void Clear(FArray<Type, size>* array)
{
    array->count = 0;
}

template<typename Type, uint32 size>
Type* GetPtr(FArray<Type, size>* array, uint32 index)
{
    CTK_ASSERT(index < array->count);

    return array->data + index;
}

template<typename Type, uint32 size>
Type Get(FArray<Type, size>* array, uint32 index)
{
    CTK_ASSERT(index < array->count);

    return array->data[index];
}

template<typename Type, uint32 size>
void Set(FArray<Type, size>* array, uint32 index, Type val)
{
    CTK_ASSERT(index < array->count);

    array->data[index] = val;
}

template<typename Type, uint32 size>
uint32 ByteSize(FArray<Type, size>* array)
{
    return size * sizeof(Type);
}

template<typename Type, uint32 size>
uint32 ByteCount(FArray<Type, size>* array)
{
    return array->count * sizeof(Type);
}

template<typename Type, uint32 size>
constexpr uint32 GetSize(FArray<Type, size>* array)
{
    CTK_UNUSED(array)
    return size;
}

template<typename Type, uint32 size>
bool Contains(FArray<Type, size>* array, Type val)
{
    CTK_ITER(array_val, array)
    {
        if (*array_val == val)
        {
            return true;
        }
    }

    return false;
}

template<typename Type, uint32 size>
void Reverse(FArray<Type, size>* array)
{
    Reverse(array->data, array->count);
}

template<typename Type, uint32 size>
void InsertionSort(FArray<Type, size>* array, Func<bool, Type*, Type*> SortFunc)
{
    InsertionSort(array->data, array->count, SortFunc);
}
