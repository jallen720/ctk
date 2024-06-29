/// Data
////////////////////////////////////////////////////////////
template<typename Type>
struct Optional
{
    Type value;
    bool exists;
};

/// Interface
////////////////////////////////////////////////////////////
template<typename Type>
static void Set(Optional<Type>* opt, Type value)
{
    opt->value  = value;
    opt->exists = true;
}

template<typename Type>
static void Unset(Optional<Type>* opt)
{
    opt->exists = false;
}

