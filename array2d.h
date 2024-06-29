/// Data
////////////////////////////////////////////////////////////
template<typename Type>
struct Array2D
{
    Type*   data;
    uint32* counts;
    uint32  size_x;
    uint32  size_y;
};

/// Interface
////////////////////////////////////////////////////////////
template<typename Type>
static void InitArray2D(Array2D<Type>* array, Allocator* allocator, uint32 size_x, uint32 size_y)
{
    if (array->data != NULL)
    {
        CTK_FATAL("can't initialize 2d array; array may have already been initialized");
    }

    if (size_x > 0)
    {
        array->data   = size_y > 0 ? Allocate<Type>(allocator, size_x * size_y) : NULL;
        array->counts = Allocate<uint32>(allocator, size_x);
    }
    else
    {
        array->data   = NULL;
        array->counts = NULL;
    }

    array->size_x = size_x;
    array->size_y = size_y;
}

template<typename Type>
static Array2D<Type>* CreateArray2D(Allocator* allocator, uint32 size_x, uint32 size_y)
{
    auto array = Allocate<Array2D<Type>>(allocator, 1);
    InitArray2D(array, allocator, size_x, size_y);
    return array;
}

template<typename Type>
static Type* Push(Array2D<Type>* array, uint32 x, Type val)
{
    CTK_ASSERT(x < array->size_x);
    CTK_ASSERT(array->counts[x] < array->size_y);

    Type* val_ptr = &array->data[(x * array->size_y) + array->counts[x]];
    ++array->counts[x];
    *val_ptr = val;

    return val_ptr;
}

template<typename Type>
static Type* Push(Array2D<Type>* array, uint32 x)
{
    return Push(array, x, {});
}

template<typename Type>
static uint32 GetCount(Array2D<Type>* array, uint32 x)
{
    CTK_ASSERT(x < array->size_x);
    return array->counts[x];
}

template<typename Type>
static Type* GetColumn(Array2D<Type>* array, uint32 x)
{
    CTK_ASSERT(x < array->size_x);
    return &array->data[x * array->size_y];
}

template<typename Type, typename ...Args>
static void
InsertionSortColumn(Array2D<Type>* array, uint32 x, Func<bool, Type*, Type*, Args...> SortFunc, Args... args)
{
    InsertionSort(GetColumn(array, x), GetCount(array, x), SortFunc, args...);
}

