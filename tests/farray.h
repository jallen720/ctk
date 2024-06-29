#pragma once

namespace FArrayTest
{

/// Utils
////////////////////////////////////////////////////////////
template<typename Type, uint32 size>
static bool TestArrayFields(FArray<Type, size>* array, uint32 expected_size, uint32 expected_count)
{
    bool pass = true;

    if (!ExpectEqual("array size", expected_size, size))
    {
        pass = false;
    }

    if (!ExpectEqual("array->count", expected_count, array->count))
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
static bool CanPushTest()
{
    bool pass = true;

    FArray<char, 2> array = {};

    if (!ExpectEqual("array(size=2,count=0); CanPush(array, 2)", true, CanPush(&array, 2)))
    {
        pass = false;
    }

    if (!ExpectEqual("array(size=2,count=0); CanPush(array, 3)", false, CanPush(&array, 3)))
    {
        pass = false;
    }

    Push(&array, 'a');
    if (!ExpectEqual("array(size=2,count=1); CanPush(array, 2)", false, CanPush(&array, 2)))
    {
        pass = false;
    }

    return pass;
}

static bool RemoveTest()
{
    bool pass = true;

    FArray<char, 4> array = {};
    {
        PushRange(&array, "1234", 4);
        RunTest("Init Layout \"1234\"", &pass, ExpectEqual, "1234", &array);
    }
    {
        Remove(&array, 0);
        RunTest("Remove(array, 0)", &pass, ExpectEqual, "234", &array);
    }
    {
        Remove(&array, 2);
        RunTest("Remove(array, 2)", &pass, ExpectEqual, "23", &array);
    }
    {
        Remove(&array, 0);
        RunTest("Remove(array, 0)", &pass, ExpectEqual, "3", &array);
    }
    {
        Remove(&array, 0);
        RunTest("Remove(array, 0)", &pass, ExpectEqual, "", &array);
    }

    return pass;
}

static bool RemoveRangeTest()
{
    bool pass = true;

    FArray<char, 8> array = {};
    {
        PushRange(&array, "12345678", 8);
        RunTest("Init Layout \"12345678\"", &pass, ExpectEqual, "12345678", &array);
    }
    {
        RemoveRange(&array, 0, 2);
        RunTest("RemoveRange(array, 0, 2)", &pass, ExpectEqual, "345678", &array);
    }
    {
        RemoveRange(&array, 4, 2);
        RunTest("RemoveRange(array, 4, 2)", &pass, ExpectEqual, "3456", &array);
    }
    {
        RemoveRange(&array, 0, 2);
        RunTest("RemoveRange(array, 0, 2)", &pass, ExpectEqual, "56", &array);
    }
    {
        RemoveRange(&array, 0, 2);
        RunTest("RemoveRange(array, 0, 2)", &pass, ExpectEqual, "", &array);
    }

    return pass;
}

static bool ContainsTest()
{
    bool pass = true;

    uint32 ints[] = { 1, 2, 3, 4 };
    FArray<uint32, 4> array = {};
    PushRange(&array, ints, 4);
    RunTest("Init array: { 1, 2, 3, 4 }", &pass, TestArrayFields, &array, 4u, 4u);

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

    FArray<char, 16> array = {};

    // Even
    {
        const char* even = "this is a test";
        PushRange(&array, even, StringSize(even));
        RunTest("Init Layout", &pass, ExpectEqual, even, &array);
    }
    {
        Reverse(&array);
        RunTest("Reverse()", &pass, ExpectEqual, "tset a si siht", &array);
    }
    {
        Reverse(&array);
        RunTest("Reverse()", &pass, ExpectEqual, "this is a test", &array);
    }

    // Odd
    Clear(&array);
    {
        const char* odd = "this is a test2";
        PushRange(&array, odd, StringSize(odd));
        RunTest("Init Layout", &pass, ExpectEqual, odd, &array);
    }
    {
        Reverse(&array);
        RunTest("Reverse()", &pass, ExpectEqual, "2tset a si siht", &array);
    }
    {
        Reverse(&array);
        RunTest("Reverse()", &pass, ExpectEqual, "this is a test2", &array);
    }

    return pass;
}

static bool InsertionSortTest()
{
    bool pass = true;

    FArray<char, 8> array = {};

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

    return pass;
}

static bool Run()
{
    bool pass = true;

    RunTest("CanPushTest()",       &pass, CanPushTest);
    RunTest("RemoveTest()",        &pass, RemoveTest);
    RunTest("RemoveRangeTest()",   &pass, RemoveRangeTest);
    RunTest("ContainsTest()",      &pass, ContainsTest);
    RunTest("ReverseTest()",       &pass, ReverseTest);
    RunTest("InsertionSortTest()", &pass, InsertionSortTest);

    return pass;
}

}
