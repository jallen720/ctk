/// Interface
////////////////////////////////////////////////////////////
template<typename Type>
void Reverse(Type* array, uint32 size) {
    for (uint32 index = 0; index < size / 2; index += 1) {
        uint32 inverse_index = size - 1 - index;
        Type temp = array[index];
        array[index] = array[inverse_index];
        array[inverse_index] = temp;
    }
}

template<typename Type, typename ...Args>
void InsertionSort(Type* array, uint32 size, Func<bool, Type*, Type*, Args...> SortFunc, Args... args) {
    for (sint32 i = 1; i < size; i += 1) {
        Type temp = array[i];
        sint32 j = i - 1;
        for (; j >= 0 && !SortFunc(&array[j], &temp, args...); j -= 1) {
            array[j + 1] = array[j];
        }
        array[j + 1] = temp;
    }
}
