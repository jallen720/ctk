/// Data
////////////////////////////////////////////////////////////
template<typename Key, typename Value>
struct Pair {
    Key   key;
    Value value;
};

/// Interface
////////////////////////////////////////////////////////////
template<typename Key, typename Value>
Value* FindValue(const Pair<Key, Value>* pairs, uint32 pair_count, Key key) {
    for (uint32 i = 0; i < pair_count; i += 1) {
        if (pairs[i].key == key) {
            return &pairs[i].value;
        }
    }

    return NULL;
}

template<typename Key, typename Value>
Key* FindKey(const Pair<Key, Value>* pairs, uint32 pair_count, Value value) {
    for (uint32 i = 0; i < pair_count; i += 1) {
        if (pairs[i].value == value) {
            return &pairs[i].key;
        }
    }

    return NULL;
}
