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

bool ExpectBytes(Stack* stack, uint8* expected_bytes)
{
    bool pass = true;

    for (uint32 i = 0; i < stack->size; i += 1)
    {
        if (stack->mem[i] != expected_bytes[i])
        {
            pass = false;
            break;
        }
    }

    PrintExpectedTag();
    PrintBytes(expected_bytes, stack->size);
    PrintLine();

    PrintActualTag(pass);
    PrintBytes(stack->mem, stack->size);
    PrintLine();

    return pass;
}
