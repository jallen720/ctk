bool TestStackFields(const char* stack_name, Stack* stack, uint32 expected_size, uint32 expected_count)
{
    bool pass = true;

    FString<256> description = {};
    Write(&description, "%s->size", stack_name);
    if (!ExpectEqual(&description, expected_size, stack->size))
    {
        pass = false;
    }

    Write(&description, "%s->count", stack_name);
    if (!ExpectEqual(&description, expected_count, stack->count))
    {
        pass = false;
    }

    return pass;
}
