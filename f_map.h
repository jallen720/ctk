/// Data
////////////////////////////////////////////////////////////
template<typename Key, typename Value, uint32 size>
struct FMap
{
    Key    keys  [size];
    Value  values[size];
    uint32 count;
};

/// Interface
////////////////////////////////////////////////////////////
template<typename Key, typename Value, uint32 size>
uint32 FindValueIndex(FMap<Key, Value, size>* map, Key key)
{
    for (uint32 i = 0; i < map->count; i += 1)
    {
        if (map->keys[i] == key)
        {
            return i;
        }
    }

    return UINT32_MAX;
}

template<typename Key, typename Value, uint32 size>
uint32 FindKeyIndex(FMap<Key, Value, size>* map, Value value)
{
    for (uint32 i = 0; i < map->count; i += 1)
    {
        if (map->values[i] == value)
        {
            return i;
        }
    }

    return UINT32_MAX;
}

template<typename Key, typename Value, uint32 size>
Value* FindValue(FMap<Key, Value, size>* map, Key key)
{
    uint32 value_index = FindValueIndex(map, key);
    return value_index == UINT32_MAX ? NULL : &map->values[value_index];
}

template<typename Key, typename Value, uint32 size>
Key* FindKey(FMap<Key, Value, size>* map, Value value)
{
    uint32 key_index = FindValueIndex(map, value);
    return key_index == UINT32_MAX ? NULL : &map->keys[key_index];
}

template<typename Key, typename Value, uint32 size>
bool CanPush(FMap<Key, Value, size>* map, uint32 count)
{
    return map->count + count <= size;
}

template<typename Key, typename Value, uint32 size>
bool CanPush(FMap<Key, Value, size>* map, Key key)
{
    return FindValue(map, key) == NULL;
}

template<typename Key, typename Value, uint32 size>
Value* Push(FMap<Key, Value, size>* map, Key key, Value value)
{
    if (!CanPush(map, 1))
    {
        CTK_FATAL("can't push key/value pair to map: no space available");
    }

    if (!CanPush(map, key))
    {
        CTK_FATAL("can't push key/value pair to map: key already exists in map");
    }

    map->keys[map->count] = key;
    Value* new_value = &map->values[map->count];
    *new_value = value;
    map->count += 1;
    return new_value;
}

template<typename Key, typename Value, uint32 size>
Value* Push(FMap<Key, Value, size>* map, Key key)
{
    return Push(map, key, {});
}

template<typename Key, typename Value, uint32 size>
void Clear(FMap<Key, Value, size>* map)
{
    map->count = 0;
}

template<typename Key, typename Value, uint32 size>
constexpr uint32 GetSize(FMap<Key, Value, size>* map)
{
    CTK_UNUSED(map);
    return size;
}

template<typename Key, typename Value, uint32 size>
void Remove(FMap<Key, Value, size>* map, Key key)
{
    uint32 index = FindValueIndex(map, key);
    if (index == UINT32_MAX)
    {
        CTK_FATAL("can't remove map entry with key: key does not exist in map");
    }

    memmove(&map->keys  [index], &map->keys  [index + 1], (map->count - index - 1) * sizeof(Key));
    memmove(&map->values[index], &map->values[index + 1], (map->count - index - 1) * sizeof(Value));
    map->count -= 1;
}
