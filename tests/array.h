#pragma once

namespace ArrayTest
{

/// Utils
////////////////////////////////////////////////////////////
static bool CompareArrayElements(Array<uint32> a, Array<uint32> b)
{
    bool pass = true;

    if (a.count != b.count)
    {
        pass = false;
        PrintExpected("array counts for arrays a and b to be equal");
        PrintActual(false, "a.count was %u, b.count was %u", a.count, b.count);
    }
    else
    {
        bool array_elems_equal = true;
        for (uint32 i = 0; i < a.count; ++i)
        {
            if (Get(&a, i) != Get(&b, i))
            {
                pass = false;
                array_elems_equal = false;
                break;
            }
        }

        PrintExpectedTag();
        Print("a: { ");
        for (uint32 i = 0; i < a.count; ++i)
        {
            Print("%u, ", Get(&a, i));
        }
        Print('}');
        PrintLine();

        PrintActualTag(array_elems_equal);
        Print("b: { ");
        for (uint32 i = 0; i < b.count; ++i)
        {
            Print("%u, ", Get(&b, i));
        }
        Print('}');
        PrintLine();
    }

    return pass;
}

template<typename Type>
static bool TestArrayFields(Array<Type>* array, uint32 expected_size, uint32 expected_count, bool null_data = false)
{
    bool pass = true;

    if (!ExpectEqual("array->size", expected_size, array->size))
    {
        pass = false;
    }

    if (!ExpectEqual("array->count", expected_count, array->count))
    {
        pass = false;
    }

    if (!ExpectEqual("array->data != NULL", !null_data, array->data != NULL))
    {
        pass = false;
    }

    return pass;
}

static bool SortDesc(char* a, char* b)
{
    return *a >= *b;
}

static bool SortAsc(char* a, char* b)
{
    return *a <= *b;
}

/// Tests
////////////////////////////////////////////////////////////
static bool FreeListAllocationTest()
{
    FreeList free_list = CreateFreeList(&std_allocator, 64, { .max_range_count = 4 });

    Array<uint32> array = CreateArray<uint32>(&free_list.allocator, 4);
    DestroyArray(&array, &free_list.allocator);

    DestroyFreeList(&free_list, &std_allocator);

    return true;
}

static bool ResizeInitializedArrayTest()
{
    bool pass = true;

    Array<uint32> array = CreateArray<uint32>(&std_allocator, 6);
    Push(&array, 1u);
    Push(&array, 2u);
    Push(&array, 3u);

    ResizeNZ(&array, &std_allocator, array.size - 2);
    RunTest("ResizeNZ(array, &std_allocator, &array->size - 2)", &pass, TestArrayFields, &array, 4u, 3u, false);

    ResizeNZ(&array, &std_allocator, array.size - 2);
    RunTest("ResizeNZ(array, &std_allocator, &array->size - 2)", &pass, TestArrayFields, &array, 2u, 2u, false);

    DestroyArray(&array, &std_allocator);

    return pass;
}

static bool ResizeUninitializedArrayTest()
{
    bool pass = true;

    Array<uint32> array = {};
    RunTest("Uninitialized Array", &pass, TestArrayFields, &array, 0u, 0u, true);

    ResizeNZ(&array, &std_allocator, 1);
    RunTest("ResizeNZ(&array, &std_allocator, 1) (initializes the array)", &pass,
            TestArrayFields, &array, 1u, 0u, false);

    ResizeNZ(&array, &std_allocator, 2);
    RunTest("ResizeNZ(&array, &std_allocator, 2)", &pass,
            TestArrayFields, &array, 2u, 0u, false);

    ResizeNZ(&array, &std_allocator, 0);
    RunTest("ResizeNZ(&array, &std_allocator, 0) (de-initializes the array)", &pass,
            TestArrayFields, &array, 0u, 0u, true);

    return pass;
}

static bool CanPushTest()
{
    bool pass = true;

    Array<uint32> array = CreateArray<uint32>(&std_allocator, 2);
    RunTest("CreateArray<uint32>(&std_allocator, 2)", &pass, TestArrayFields, &array, 2u, 0u, false);

    if (!ExpectEqual("array(size=2,count=0); CanPush(&array, 2)", true, CanPush(&array, 2)))
    {
        pass = false;
    }

    if (!ExpectEqual("array(size=2,count=0); CanPush(&array, 3)", false, CanPush(&array, 3)))
    {
        pass = false;
    }

    Push(&array, 1u);
    if (!ExpectEqual("array(size=2,count=1); CanPush(&array, 2)", false, CanPush(&array, 2)))
    {
        pass = false;
    }

    DestroyArray(&array, &std_allocator);

    return pass;
}

static bool RemoveTest()
{
    bool pass = true;

    uint32 ints[] = { 1, 2, 3, 4 };
    Array<uint32> ints_array = CTK_WRAP_ARRAY(ints);
    {
        uint32 expected_ints[] = { 1, 2, 3, 4 };
        RunTest("Init Layout { 1, 2, 3, 4 }", &pass, CompareArrayElements, CTK_WRAP_ARRAY(expected_ints), ints_array);
    }
    {
        Remove(&ints_array, 0);
        uint32 expected_ints[] = { 2, 3, 4 };
        RunTest("Remove(&ints_array, 0)", &pass, CompareArrayElements, CTK_WRAP_ARRAY(expected_ints), ints_array);
    }
    {
        Remove(&ints_array, 2);
        uint32 expected_ints[] = { 2, 3 };
        RunTest("Remove(&ints_array, 2)", &pass, CompareArrayElements, CTK_WRAP_ARRAY(expected_ints), ints_array);
    }
    {
        Remove(&ints_array, 0);
        uint32 expected_ints[] = { 3 };
        RunTest("Remove(&ints_array, 0)", &pass, CompareArrayElements, CTK_WRAP_ARRAY(expected_ints), ints_array);
    }

    Remove(&ints_array, 0);
    RunTest("Remove(&ints_array, 0)", &pass, CompareArrayElements, {}, ints_array);

    return pass;
}

static bool RemoveRangeTest()
{
    bool pass = true;

    uint32 ints[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    Array<uint32> ints_array = CTK_WRAP_ARRAY(ints);
    {
        uint32 expected_ints[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
        RunTest("Init Layout { 1, 2, 3, 4, 5, 6, 7, 8 }", &pass,
                CompareArrayElements, CTK_WRAP_ARRAY(expected_ints), ints_array);
    }
    {
        RemoveRange(&ints_array, 0, 2);
        uint32 expected_ints[] = { 3, 4, 5, 6, 7, 8 };
        RunTest("RemoveRange(&ints_array, 0, 2)", &pass,
                CompareArrayElements, CTK_WRAP_ARRAY(expected_ints), ints_array);
    }
    {
        RemoveRange(&ints_array, 4, 2);
        uint32 expected_ints[] = { 3, 4, 5, 6 };
        RunTest("RemoveRange(&ints_array, 4, 2)", &pass,
                CompareArrayElements, CTK_WRAP_ARRAY(expected_ints), ints_array);
    }
    {
        RemoveRange(&ints_array, 0, 2);
        uint32 expected_ints[] = { 5, 6 };
        RunTest("RemoveRange(&ints_array, 0, 2)", &pass,
                CompareArrayElements, CTK_WRAP_ARRAY(expected_ints), ints_array);
    }

    RemoveRange(&ints_array, 0, 2);
    RunTest("RemoveRange(&ints_array, 0, 2)", &pass, CompareArrayElements, {}, ints_array);

    return pass;
}

static bool ContainsTest()
{
    bool pass = true;

    uint32 ints[] = { 1, 2, 3, 4 };
    Array<uint32> array = CTK_WRAP_ARRAY(ints);
    RunTest("Init array: { 1, 2, 3, 4 }", &pass, TestArrayFields, &array, 4u, 4u, false);

    if (!ExpectEqual("Contains(array, 2u)", true, Contains(&array, 2u)))
    {
        pass = false;
    }

    if (!ExpectEqual("Contains(array, 4u)", true, Contains(&array, 4u)))
    {
        pass = false;
    }

    if (!ExpectEqual("Contains(array, 5u)", false, Contains(&array, 5u)))
    {
        pass = false;
    }

    return pass;
}

static bool ReverseTest()
{
    bool pass = true;

    auto array = CreateArray<char>(&std_allocator, 16);

    // Even
    const char* even = "this is a test";
    PushRange(&array, even, StringSize(even));
    RunTest("Init Layout", &pass, ExpectEqual, "this is a test", &array);

    Reverse(&array);
    RunTest("Reverse()", &pass, ExpectEqual, "tset a si siht", &array);

    Reverse(&array);
    RunTest("Reverse()", &pass, ExpectEqual, "this is a test", &array);

    Clear(&array);

    // Odd
    const char* odd = "this is a test2";
    PushRange(&array, odd, StringSize(odd));
    RunTest("Init Layout", &pass, ExpectEqual, "this is a test2", &array);

    Reverse(&array);
    RunTest("Reverse()", &pass, ExpectEqual, "2tset a si siht", &array);

    Reverse(&array);
    RunTest("Reverse()", &pass, ExpectEqual, "this is a test2", &array);

    DestroyArray(&array, &std_allocator);

    return pass;
}

static bool InsertionSortTest()
{
    bool pass = true;

    auto array = CreateArray<char>(&std_allocator, 8);

    PushRange(&array, "1234", 4);
    RunTest("Init Layout", &pass, ExpectEqual, "1234", &array);

    InsertionSort(&array, SortDesc);
    RunTest("InsertionSort() descending", &pass, ExpectEqual, "4321", &array);

    InsertionSort(&array, SortAsc);
    RunTest("InsertionSort() ascending", &pass, ExpectEqual, "1234", &array);

    Clear(&array);

    PushRange(&array, "111123", 6);
    RunTest("Init Layout", &pass, ExpectEqual, "111123", &array);

    InsertionSort(&array, SortDesc);
    RunTest("InsertionSort() descending", &pass, ExpectEqual, "321111", &array);

    InsertionSort(&array, SortAsc);
    RunTest("InsertionSort() ascending", &pass, ExpectEqual, "111123", &array);

    DestroyArray(&array, &std_allocator);

    return pass;
}

static bool Run()
{
    bool pass = true;

    RunTest("FreeListAllocationTest()",       &pass, FreeListAllocationTest);
    RunTest("ResizeInitializedArrayTest()",   &pass, ResizeInitializedArrayTest);
    RunTest("ResizeUninitializedArrayTest()", &pass, ResizeUninitializedArrayTest);
    RunTest("CanPushTest()",                  &pass, CanPushTest);
    RunTest("RemoveTest()",                   &pass, RemoveTest);
    RunTest("RemoveRangeTest()",              &pass, RemoveRangeTest);
    RunTest("ContainsTest()",                 &pass, ContainsTest);
    RunTest("ReverseTest()",                  &pass, ReverseTest);
    RunTest("InsertionSortTest()",            &pass, InsertionSortTest);

    return pass;
}

}
