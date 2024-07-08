/// Data
////////////////////////////////////////////////////////////
template<typename Key, typename Value>
struct Pair
{
    Key   key;
    Value value;
};

/// Interface
////////////////////////////////////////////////////////////
template<typename Key, typename Value>
bool FindValuePtr(Pair<Key, Value>* pairs, uint32 pair_count, Key key, Value** value = NULL)
{
    for (uint32 i = 0; i < pair_count; ++i)
    {
        if (pairs[i].key == key)
        {
            if (value != NULL)
            {
                *value = &pairs[i].value;
            }
            return true;
        }
    }

    return false;
}

template<typename Key, typename Value>
bool FindValue(const Pair<Key, Value>* pairs, uint32 pair_count, Key key, Value* value = NULL)
{
    for (uint32 i = 0; i < pair_count; ++i)
    {
        if (pairs[i].key == key)
        {
            if (value != NULL)
            {
                *value = pairs[i].value;
            }
            return true;
        }
    }

    return false;
}

template<typename Key, typename Value>
bool FindKeyPtr(const Pair<Key, Value>* pairs, uint32 pair_count, Value value, Key** key = NULL)
{
    for (uint32 i = 0; i < pair_count; ++i)
    {
        if (pairs[i].value == value)
        {
            if (key != NULL)
            {
                *key = &pairs[i].key;
            }
            return true;
        }
    }

    return false;
}

template<typename Key, typename Value>
bool FindKey(const Pair<Key, Value>* pairs, uint32 pair_count, Value value, Key* key = NULL)
{
    for (uint32 i = 0; i < pair_count; ++i)
    {
        if (pairs[i].value == value)
        {
            if (key != NULL)
            {
                *key = pairs[i].key;
            }
            return true;
        }
    }

    return false;
}
