/// Macros
////////////////////////////////////////////////////////////
#define CTK_WRAP_ARRAY(ARRAY) WrapArray(ARRAY, CTK_ARRAY_SIZE(ARRAY))
#define CTK_WRAP_ARRAY_1(PTR) WrapArray(PTR, 1)

/// Data
////////////////////////////////////////////////////////////
template<typename Type>
struct Array
{
    Allocator* allocator;
    Type*      data;
    uint32     size;
    uint32     count;
};

/// C-Array Interface
////////////////////////////////////////////////////////////
template<typename Type>
void Reverse(Type* array, uint32 size)
{
    for (uint32 index = 0; index < size / 2; index += 1)
    {
        uint32 inverse_index = size - 1 - index;
        Type temp = array[index];
        array[index] = array[inverse_index];
        array[inverse_index] = temp;
    }
}

template<typename Type, typename ...Args>
void InsertionSort(Type* array, uint32 size, Func<bool, Type*, Type*, Args...> SortFunc, Args... args)
{
    for (sint32 i = 1; i < size; i += 1)
    {
        Type temp = array[i];
        sint32 j = i - 1;
        for (; j >= 0 && !SortFunc(&array[j], &temp, args...); j -= 1)
        {
            array[j + 1] = array[j];
        }
        array[j + 1] = temp;
    }
}

/// CTK_ITER Interface
////////////////////////////////////////////////////////////
template<typename Type>
Type* IterStart(Array<Type>* array)
{
    return array->data;
}

template<typename Type>
Type* IterEnd(Array<Type>* array)
{
    return array->data + array->count;
}

/// Array Interface
////////////////////////////////////////////////////////////
template<typename Type>
Array<Type> CreateArray(Allocator* allocator, uint32 size = 0)
{
    Array<Type> array = {};
    array.allocator = allocator;
    array.data      = size > 0 ? Allocate<Type>(allocator, size) : NULL;
    array.size      = size;
    array.count     = 0;
    return array;
}

template<typename Type>
Array<Type> CreateArray(Allocator* allocator, const Type* src_array, uint32 size)
{
    Array<Type> array = {};
    array.allocator = allocator;
    array.data      = size > 0 ? Allocate<Type>(allocator, size) : NULL;
    array.size      = size;
    array.count     = size;
    memcpy(array.data, src_array, size * sizeof(Type));
    return array;
}

template<typename Type>
Array<Type> CreateArray(Allocator* allocator, Array<Type>* src_array)
{
    return CreateArray(allocator, src_array->data, src_array->count);
}

template<typename Type>
Array<Type> CreateArrayFull(Allocator* allocator, uint32 size)
{
    Array<Type> array = {};
    array.allocator = allocator;
    array.data      = size > 0 ? Allocate<Type>(allocator, size) : NULL;
    array.size      = size;
    array.count     = size;
    return array;
}

template<typename Type>
void DestroyArray(Array<Type>* array)
{
    CTK_ASSERT(array->allocator != NULL);

    if (array->data != NULL)
    {
        Deallocate(array->allocator, array->data);
    }
    *array = {};
}

#define CTK_ARRAY_RESIZE_FUNC(NAME, REALLOCATE_FUNC) \
template<typename Type> \
void NAME(Array<Type>* array, uint32 new_size) \
{ \
    if (array->size > 0 && new_size == 0) \
    { \
        Deallocate(array->allocator, array->data); \
        array->data  = NULL; \
        array->size  = 0; \
        array->count = 0; \
    } \
    else if (array->size == 0 && new_size > 0) \
    { \
        array->data  = Allocate<Type>(array->allocator, new_size); \
        array->size  = new_size; \
        array->count = 0; \
    } \
    else \
    { \
        array->size = new_size; \
        if (array->count >= new_size) \
        { \
            array->count = new_size; \
        } \
 \
        array->data = REALLOCATE_FUNC(array->allocator, array->data, new_size); \
    } \
}

CTK_ARRAY_RESIZE_FUNC(ResizeNZ, ReallocateNZ)
CTK_ARRAY_RESIZE_FUNC(Resize,   Reallocate)

template<typename Type>
bool CanPush(Array<Type>* array, uint32 count)
{
    return array->count + count <= array->size;
}

template<typename Type>
Type* Push(Array<Type>* array, Type elem)
{
    if (array->count == array->size)
    {
        CTK_FATAL("can't push element to array: no space available");
    }

    Type* new_elem = &array->data[array->count];
    *new_elem = elem;
    array->count += 1;
    return new_elem;
}

template<typename Type>
Type* Push(Array<Type>* array)
{
    return Push(array, {});
}

template<typename Type>
Type* PushResize(Array<Type>* array, Type elem, uint32 additional_space)
{
    if (!CanPush(array, 1))
    {
        Resize(array, array->size + additional_space);
    }
    return Push(array, elem);
}

template<typename Type>
Type* PushResize(Array<Type>* array, uint32 additional_space)
{
    return PushResize(array, {}, additional_space);
}

template<typename Type>
void PushRange(Array<Type>* array, const Type* data, uint32 data_size)
{
    if (data_size == 0)
    {
        return;
    }

    uint32 available_space = array->size - array->count;
    if (available_space < data_size)
    {
        CTK_FATAL("can't push %u elements to array: array has %u available slots", data_size, available_space);
    }

    memcpy(&array->data[array->count], data, data_size * sizeof(Type));
    array->count += data_size;
}

template<typename Type>
void PushRange(Array<Type>* array, Array<Type>* other)
{
    PushRange(array, other->data, other->count);
}

template<typename Type>
void PushRange(Array<Type>* array, Array<Type> other)
{
    PushRange(array, other.data, other.count);
}

template<typename Type>
void PushRangeResize(Array<Type>* array, const Type* data, uint32 data_size, uint32 additional_space)
{
    if (!CanPush(array, data_size))
    {
        Resize(array, array->size + Max(data_size, additional_space));
    }
    PushRange(array, data, data_size);
}

template<typename Type>
void PushRangeResize(Array<Type>* array, Array<Type>* other, uint32 additional_space)
{
    PushRangeResize(array, other->data, other->count, additional_space);
}

template<typename Type>
void PushRangeResize(Array<Type>* array, Array<Type> other, Allocator* allocator, uint32 additional_space)
{
    PushRangeResize(array, other.data, other.count, allocator, additional_space);
}

template<typename Type>
void Remove(Array<Type>* array, uint32 index)
{
    CTK_ASSERT(index < array->count);

    memmove(&array->data[index], &array->data[index + 1], (array->count - index - 1) * sizeof(Type));
    array->count -= 1;
}

template<typename Type>
void RemoveRange(Array<Type>* array, uint32 index, uint32 count)
{
    CTK_ASSERT(index < array->count);
    CTK_ASSERT(index + count <= array->count);

    memmove(&array->data[index], &array->data[index + count], (array->count - index - count) * sizeof(Type));
    array->count -= count;
}

template<typename Type>
void Clear(Array<Type>* array)
{
    array->count = 0;
}

template<typename Type>
Type* GetPtr(Array<Type>* array, uint32 index)
{
    CTK_ASSERT(index < array->count);

    return &array->data[index];
}

template<typename Type>
Type Get(Array<Type>* array, uint32 index)
{
    CTK_ASSERT(index < array->count);

    return array->data[index];
}

template<typename Type>
void Set(Array<Type>* array, uint32 index, Type val)
{
    CTK_ASSERT(index < array->count);

    array->data[index] = val;
}

template<typename Type>
uint32 ByteSize(Array<Type>* array)
{
    return array->size * sizeof(Type);
}

template<typename Type>
uint32 ByteCount(Array<Type>* array)
{
    return array->count * sizeof(Type);
}

template<typename Type>
bool Contains(Array<Type>* array, Type val)
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

template<typename Type>
Array<Type> WrapArray(Type* data, uint32 size)
{
    CTK_ASSERT(size > 0);

    Array<Type> array = {};
    array.data  = data;
    array.size  = size;
    array.count = size;
    return array;
}

template<typename Type, typename ...Args>
void InsertionSort(Array<Type>* array, Func<bool, Type*, Type*, Args...> SortFunc, Args... args)
{
    InsertionSort(array->data, array->count, SortFunc, args...);
}

template<typename Type>
void Reverse(Array<Type>* array)
{
    Reverse(array->data, array->count);
}

template<typename Type>
bool IsInitialized(Array<Type>* array)
{
    return array->data != NULL && array->size > 0;
}
