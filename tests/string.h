#pragma once

namespace StringTest
{

/// Utils
////////////////////////////////////////////////////////////
static bool FreeListAllocationTest()
{
    FreeList free_list = CreateFreeList(&std_allocator, 64, { .max_range_count = 4 });

    String string = CreateString(&free_list.allocator, 4);
    DestroyString(&string, &free_list.allocator);

    DestroyFreeList(&free_list, &std_allocator);

    return true;
}

static bool TestStringFields(String* string, uint32 expected_size, uint32 expected_count, bool null_data)
{
    bool pass = true;

    if (!ExpectEqual("string->size", expected_size, string->size))
    {
        pass = false;
    }

    if (!ExpectEqual("string->count", expected_count, string->count))
    {
        pass = false;
    }

    if (!ExpectEqual("string->data != NULL", !null_data, string->data != NULL))
    {
        pass = false;
    }

    return pass;
}

/// Tests
////////////////////////////////////////////////////////////
static bool ResizeInitializedStringTest()
{
    bool pass = true;

    String string = CreateString(&std_allocator, 6);
    Push(&string, '1');
    Push(&string, '2');
    Push(&string, '3');
    RunTest("Initialized String", &pass, ExpectEqual, "123", 3u, &string);
    RunTest("Initialized String", &pass, TestStringFields, &string, 6u, 3u, false);

    ResizeNZ(&string, &std_allocator, 4u);
    RunTest("ResizeNZ(string, &std_allocator, 4u)", &pass, TestStringFields, &string, 4u, 3u, false);

    ResizeNZ(&string, &std_allocator, 2u);
    RunTest("ResizeNZ(string, &std_allocator, 2u)", &pass, TestStringFields, &string, 2u, 2u, false);

    ResizeNZ(&string, &std_allocator, 0u);
    RunTest("ResizeNZ(string, &std_allocator, 0u)", &pass, TestStringFields, &string, 0u, 0u, true);

    DestroyString(&string, &std_allocator);

    return pass;
}

static bool ResizeUninitializedStringTest()
{
    bool pass = true;

    String string = {};
    RunTest("Uninitialized String", &pass, TestStringFields, &string, 0u, 0u, true);

    ResizeNZ(&string, &std_allocator, 1);
    RunTest("ResizeNZ(&string, &std_allocator, 1) (initializes the string)", &pass,
            TestStringFields, &string, 1u, 0u, false);

    ResizeNZ(&string, &std_allocator, 2);
    RunTest("ResizeNZ(&string, &std_allocator, 2)", &pass,
            TestStringFields, &string, 2u, 0u, false);

    ResizeNZ(&string, &std_allocator, 0);
    RunTest("ResizeNZ(&string, &std_allocator, 0) (de-initializes the string)", &pass,
            TestStringFields, &string, 0u, 0u, true);

    return pass;
}

static bool CanPushTest()
{
    bool pass = true;

    String string = CreateString(&std_allocator, 2);

    if (!ExpectEqual("string(size=2,count=0); CanPush(string, 2)", true, CanPush(&string, 2)))
    {
        pass = false;
    }

    if (!ExpectEqual("string(size=2,count=0); CanPush(&string, 3)", false, CanPush(&string, 3)))
    {
        pass = false;
    }

    Push(&string, (char)'1&');
    if (!ExpectEqual("string(size=2,count=1); CanPush(&string, 2)", false, CanPush(&string, 2)))
    {
        pass = false;
    }

    DestroyString(&string, &std_allocator);

    return pass;
}

static bool RemoveTest()
{
    bool pass = true;

    String string = CreateString(&std_allocator, "1234");
    RunTest("Init Layout \"1234\"", &pass, ExpectEqual, "1234", &string);

    Remove(&string, 0);
    RunTest("Remove(string, 0)", &pass, ExpectEqual, "234", &string);

    Remove(&string, 2);
    RunTest("Remove(string, 2)", &pass, ExpectEqual, "23", &string);

    Remove(&string, 0);
    RunTest("Remove(string, 0)", &pass, ExpectEqual, "3", &string);

    Remove(&string, 0);
    RunTest("Remove(string, 0)", &pass, ExpectEqual, "", &string);

    DestroyString(&string, &std_allocator);
    return pass;
}

static bool RemoveRangeTest()
{
    bool pass = true;

    String string = CreateString(&std_allocator, "12345678");
    RunTest("Init Layout \"12345678\"", &pass, ExpectEqual, "12345678", &string);

    RemoveRange(&string, 0, 2);
    RunTest("RemoveRange(string, 0, 2)", &pass, ExpectEqual, "345678", &string);

    RemoveRange(&string, 4, 2);
    RunTest("RemoveRange(string, 4, 2)", &pass, ExpectEqual, "3456", &string);

    RemoveRange(&string, 0, 2);
    RunTest("RemoveRange(string, 0, 2)", &pass, ExpectEqual, "56", &string);

    RemoveRange(&string, 0, 2);
    RunTest("RemoveRange(string, 0, 2)", &pass, ExpectEqual, "", &string);

    DestroyString(&string, &std_allocator);
    return pass;
}

static bool WriteTest()
{
    bool pass = true;

    {
        static constexpr uint32 STRING_SIZE = 5;
        String string = CreateString(&std_allocator, STRING_SIZE);

        Write(&string, "test");
        RunTest("Write(string, \"test\") (equal up to string.count)", &pass,
                ExpectEqual, "test", &string);
        RunTest("Write(string, \"test\") (equal up to string.count)", &pass,
                TestStringFields, &string, STRING_SIZE, 4u, false);

        DestroyString(&string, &std_allocator);
    }
    {
        static constexpr uint32 STRING_SIZE = 8;
        String string = CreateString(&std_allocator, STRING_SIZE);

        char expected[STRING_SIZE] = {};
        Write(&string, "test");
        Write(expected, STRING_SIZE, "test");
        RunTest("Write(string, \"test\") (equal up to string.size)", &pass,
                ExpectEqualFull, (const char*)expected, STRING_SIZE, &string);
        RunTest("Write(string, \"test\") (equal up to string.size)", &pass,
                TestStringFields, &string, STRING_SIZE, 4u, false);

        DestroyString(&string, &std_allocator);
    }

    return pass;
}

static bool AppendTest()
{
    bool pass = true;

    {
        static constexpr uint32 STRING_SIZE = 18;
        char expected[4][STRING_SIZE] = {};
        Write(expected[0], STRING_SIZE, "test1");
        Write(expected[1], STRING_SIZE, "test1test2");
        Write(expected[2], STRING_SIZE, "test1test2test3");
        Write(expected[3], STRING_SIZE, "test1test2test3test4"); // Will truncate "st4"

        String string = CreateString(&std_allocator, STRING_SIZE);

        Append(&string, "test1");
        RunTest("Append(string, \"test1\")", &pass, ExpectEqualFull, (const char*)expected[0], STRING_SIZE, &string);
        RunTest("Append(string, \"test1\")", &pass, TestStringFields, &string, STRING_SIZE, 5u, false);

        Append(&string, "test2");
        RunTest("Append(string, \"test2\")", &pass, ExpectEqualFull, (const char*)expected[1], STRING_SIZE, &string);
        RunTest("Append(string, \"test2\")", &pass, TestStringFields, &string, STRING_SIZE, 10u, false);

        Append(&string, "test3");
        RunTest("Append(string, \"test3\")", &pass, ExpectEqualFull, (const char*)expected[2], STRING_SIZE, &string);
        RunTest("Append(string, \"test3\")", &pass, TestStringFields, &string, STRING_SIZE, 15u, false);

        Append(&string, "test4");
        RunTest("Append(string, \"test4\")", &pass, ExpectEqualFull, (const char*)expected[3], STRING_SIZE, &string);
        RunTest("Append(string, \"test4\")", &pass, TestStringFields, &string, STRING_SIZE, STRING_SIZE, false);

        DestroyString(&string, &std_allocator);
    }

    return pass;
}

static bool ContainsTest()
{
    bool pass = true;

    String string = CreateString(&std_allocator, "1234");
    RunTest("Init string: \"1234\"", &pass, TestStringFields, &string, 4u, 4u, false);

    if (!ExpectEqual("Contains(string, '2')", true, Contains(&string, '2')))
    {
        pass = false;
    }

    if (!ExpectEqual("Contains(string, '4')", true, Contains(&string, '4')))
    {
        pass = false;
    }

    if (!ExpectEqual("Contains(string, '5')", false, Contains(&string, '5')))
    {
        pass = false;
    }

    DestroyString(&string, &std_allocator);

    return pass;
}

static bool ReverseTest()
{
    bool pass = true;

    String string = CreateString(&std_allocator, 16);

    // Even
    PushRange(&string, "this is a test");
    RunTest("Init Layout", &pass, ExpectEqual, "this is a test", &string);

    Reverse(&string);
    RunTest("Reverse()", &pass, ExpectEqual, "tset a si siht", &string);

    Reverse(&string);
    RunTest("Reverse()", &pass, ExpectEqual, "this is a test", &string);

    Clear(&string);

    // Odd
    PushRange(&string, "this is a test2");
    RunTest("Init Layout", &pass, ExpectEqual, "this is a test2", &string);

    Reverse(&string);
    RunTest("Reverse()", &pass, ExpectEqual, "2tset a si siht", &string);

    Reverse(&string);
    RunTest("Reverse()", &pass, ExpectEqual, "this is a test2", &string);

    DestroyString(&string, &std_allocator);

    return pass;
}

static bool ConversionTests()
{
    bool pass = true;

#define INT_CONVERSION_TEST(INT_TYPE, MIN, MAX, MIN_UNDERFLOW_STR, MAX_OVERFLOW_STR) \
    RunTest("ToInt<"#INT_TYPE">(\"8\")",                                &pass, ExpectEqual, (INT_TYPE)8,     ToInt<INT_TYPE>("8")); \
    RunTest("ToInt<"#INT_TYPE">(\"16\")",                               &pass, ExpectEqual, (INT_TYPE)16,    ToInt<INT_TYPE>("16")); \
    RunTest("ToInt<"#INT_TYPE">(\"128\")",                              &pass, ExpectEqual, (INT_TYPE)128,   ToInt<INT_TYPE>("128")); \
    RunTest("ToInt<"#INT_TYPE">(\"-8\")",                               &pass, ExpectEqual, (INT_TYPE)-8,    ToInt<INT_TYPE>("-8")); \
    RunTest("ToInt<"#INT_TYPE">(\"-16\")",                              &pass, ExpectEqual, (INT_TYPE)-16,   ToInt<INT_TYPE>("-16")); \
    RunTest("ToInt<"#INT_TYPE">(\"-128\")",                             &pass, ExpectEqual, (INT_TYPE)-128,  ToInt<INT_TYPE>("-128")); \
    RunTest("ToInt<"#INT_TYPE">(\""#MAX"\")",                           &pass, ExpectEqual, (INT_TYPE)MAX,   ToInt<INT_TYPE>(#MAX)); \
    RunTest("ToInt<"#INT_TYPE">(\""MAX_OVERFLOW_STR"\") (overflow)",    &pass, ExpectEqual, (INT_TYPE)MIN,   ToInt<INT_TYPE>(MAX_OVERFLOW_STR)); \
    RunTest("ToInt<"#INT_TYPE">(\""#MIN"\")",                           &pass, ExpectEqual, (INT_TYPE)MIN,   ToInt<INT_TYPE>(#MIN)); \
    RunTest("ToInt<"#INT_TYPE">(\""MIN_UNDERFLOW_STR"\") (underflow)",  &pass, ExpectEqual, (INT_TYPE)MAX,   ToInt<INT_TYPE>(MIN_UNDERFLOW_STR)); \
    RunTest("ToInt<"#INT_TYPE">(\" 8 \")",                              &pass, ExpectEqual, (INT_TYPE)8,     ToInt<INT_TYPE>(" 8 ")); \
    RunTest("ToInt<"#INT_TYPE">(\" 16 \")",                             &pass, ExpectEqual, (INT_TYPE)16,    ToInt<INT_TYPE>(" 16 ")); \
    RunTest("ToInt<"#INT_TYPE">(\" 128 \")",                            &pass, ExpectEqual, (INT_TYPE)128,   ToInt<INT_TYPE>(" 128 ")); \
    RunTest("ToInt<"#INT_TYPE">(\" -8 \")",                             &pass, ExpectEqual, (INT_TYPE)-8,    ToInt<INT_TYPE>(" -8 ")); \
    RunTest("ToInt<"#INT_TYPE">(\" -16 \")",                            &pass, ExpectEqual, (INT_TYPE)-16,   ToInt<INT_TYPE>(" -16 ")); \
    RunTest("ToInt<"#INT_TYPE">(\" -128 \")",                           &pass, ExpectEqual, (INT_TYPE)-128,  ToInt<INT_TYPE>(" -128 ")); \
    RunTest("ToInt<"#INT_TYPE">(\" "#MAX" \")",                         &pass, ExpectEqual, (INT_TYPE)MAX,   ToInt<INT_TYPE>(" "#MAX" ")); \
    RunTest("ToInt<"#INT_TYPE">(\" "MAX_OVERFLOW_STR" \") (overflow)",  &pass, ExpectEqual, (INT_TYPE)MIN,   ToInt<INT_TYPE>(" "MAX_OVERFLOW_STR" ")); \
    RunTest("ToInt<"#INT_TYPE">(\" "#MIN" \")",                         &pass, ExpectEqual, (INT_TYPE)MIN,   ToInt<INT_TYPE>(" "#MIN" ")); \
    RunTest("ToInt<"#INT_TYPE">(\" "MIN_UNDERFLOW_STR" \") (overflow)", &pass, ExpectEqual, (INT_TYPE)MAX,   ToInt<INT_TYPE>(" "MIN_UNDERFLOW_STR" ")); \
    RunTest("ToInt<"#INT_TYPE">(\"100partially invalid\")",             &pass, ExpectEqual, (INT_TYPE)100,   ToInt<INT_TYPE>("100partially invalid")); \
    RunTest("ToInt<"#INT_TYPE">(\"-100partially invalid\")",            &pass, ExpectEqual, (INT_TYPE)-100,  ToInt<INT_TYPE>("-100partially invalid")); \
    RunTest("ToInt<"#INT_TYPE">(\" 100partially invalid\")",            &pass, ExpectEqual, (INT_TYPE)100,   ToInt<INT_TYPE>(" 100partially invalid")); \
    RunTest("ToInt<"#INT_TYPE">(\" -100partially invalid\")",           &pass, ExpectEqual, (INT_TYPE)-100,  ToInt<INT_TYPE>(" -100partially invalid")); \
    RunTest("ToInt<"#INT_TYPE">(\"invalid\")",                          &pass, ExpectEqual, (INT_TYPE)0,     ToInt<INT_TYPE>("invalid"));

#define FLOAT_CONVERSION_TEST(FLOAT_TYPE) \
    RunTest("ToFloat<"#FLOAT_TYPE">(\"1.23\")",                   &pass, ExpectEqual, (FLOAT_TYPE)1.23,               ToFloat<FLOAT_TYPE>("1.23")); \
    RunTest("ToFloat<"#FLOAT_TYPE">(\"10.23\")",                  &pass, ExpectEqual, (FLOAT_TYPE)10.23,              ToFloat<FLOAT_TYPE>("10.23")); \
    RunTest("ToFloat<"#FLOAT_TYPE">(\"1024.2345678\")",           &pass, ExpectEqual, (FLOAT_TYPE)1024.2345678,       ToFloat<FLOAT_TYPE>("1024.2345678")); \
    RunTest("ToFloat<"#FLOAT_TYPE">(\"-1.23\")",                  &pass, ExpectEqual, (FLOAT_TYPE)-1.23,              ToFloat<FLOAT_TYPE>("-1.23")); \
    RunTest("ToFloat<"#FLOAT_TYPE">(\"-10.23\")",                 &pass, ExpectEqual, (FLOAT_TYPE)-10.23,             ToFloat<FLOAT_TYPE>("-10.23")); \
    RunTest("ToFloat<"#FLOAT_TYPE">(\"-1024.2345678\")",          &pass, ExpectEqual, (FLOAT_TYPE)-1024.2345678,      ToFloat<FLOAT_TYPE>("-1024.2345678")); \
    RunTest("ToFloat<"#FLOAT_TYPE">(\"1024123123.2345678\")",     &pass, ExpectEqual, (FLOAT_TYPE)1024123123.2345678, ToFloat<FLOAT_TYPE>("1024123123.2345678")); \
    RunTest("ToFloat<"#FLOAT_TYPE">(\" 1.23 \")",                 &pass, ExpectEqual, (FLOAT_TYPE)1.23,               ToFloat<FLOAT_TYPE>(" 1.23 ")); \
    RunTest("ToFloat<"#FLOAT_TYPE">(\" 10.23 \")",                &pass, ExpectEqual, (FLOAT_TYPE)10.23,              ToFloat<FLOAT_TYPE>(" 10.23 ")); \
    RunTest("ToFloat<"#FLOAT_TYPE">(\" 1024.2345678 \")",         &pass, ExpectEqual, (FLOAT_TYPE)1024.2345678,       ToFloat<FLOAT_TYPE>(" 1024.2345678 ")); \
    RunTest("ToFloat<"#FLOAT_TYPE">(\" -1.23 \")",                &pass, ExpectEqual, (FLOAT_TYPE)-1.23,              ToFloat<FLOAT_TYPE>(" -1.23 ")); \
    RunTest("ToFloat<"#FLOAT_TYPE">(\" -10.23 \")",               &pass, ExpectEqual, (FLOAT_TYPE)-10.23,             ToFloat<FLOAT_TYPE>(" -10.23 ")); \
    RunTest("ToFloat<"#FLOAT_TYPE">(\" -1024.2345678 \")",        &pass, ExpectEqual, (FLOAT_TYPE)-1024.2345678,      ToFloat<FLOAT_TYPE>(" -1024.2345678 ")); \
    RunTest("ToFloat<"#FLOAT_TYPE">(\"1.1partially invalid\")",   &pass, ExpectEqual, (FLOAT_TYPE)1.1,                ToFloat<FLOAT_TYPE>("1.1partially invalid")); \
    RunTest("ToFloat<"#FLOAT_TYPE">(\"-1.1partially invalid\")",  &pass, ExpectEqual, (FLOAT_TYPE)-1.1,               ToFloat<FLOAT_TYPE>("-1.1partially invalid")); \
    RunTest("ToFloat<"#FLOAT_TYPE">(\" 1.1partially invalid\")",  &pass, ExpectEqual, (FLOAT_TYPE)1.1,                ToFloat<FLOAT_TYPE>(" 1.1partially invalid")); \
    RunTest("ToFloat<"#FLOAT_TYPE">(\" -1.1partially invalid\")", &pass, ExpectEqual, (FLOAT_TYPE)-1.1,               ToFloat<FLOAT_TYPE>(" -1.1partially invalid")); \
    RunTest<Func<FLOAT_TYPE, const char*>>("ToFloat<"#FLOAT_TYPE">(\"1.1.1\")", &pass, ExpectFatalError, ToFloat<FLOAT_TYPE>, "1.1.1");

    // INT_CONVERSION_TEST(sint64, -9223372036854775807, 9223372036854775807, "-9223372036854775809", "9223372036854775808")
    INT_CONVERSION_TEST(sint32, -2147483648, 2147483647,           "-2147483649", "2147483648")
    INT_CONVERSION_TEST(sint16, -32768,      32767,                "-32769",      "32768")
    INT_CONVERSION_TEST(sint8,  -128,        127,                  "-129",        "128")
    INT_CONVERSION_TEST(uint64, 0,           18446744073709551615, "-1",          "18446744073709551616")
    INT_CONVERSION_TEST(uint32, 0,           4294967295,           "-1",          "4294967296")
    INT_CONVERSION_TEST(uint16, 0,           65535,                "-1",          "65536")
    INT_CONVERSION_TEST(uint8,  0,           255,                  "-1",          "256")
    FLOAT_CONVERSION_TEST(float32)
    FLOAT_CONVERSION_TEST(float64)

    return pass;
}

static bool Run()
{
    bool pass = true;

    RunTest("FreeListAllocationTest()",        &pass, FreeListAllocationTest);
    RunTest("ResizeInitializedStringTest()",   &pass, ResizeInitializedStringTest);
    RunTest("ResizeUninitializedStringTest()", &pass, ResizeUninitializedStringTest);
    RunTest("CanPushTest()",                   &pass, CanPushTest);
    RunTest("RemoveTest()",                    &pass, RemoveTest);
    RunTest("RemoveRangeTest()",               &pass, RemoveRangeTest);
    RunTest("WriteTest()",                     &pass, WriteTest);
    RunTest("AppendTest()",                    &pass, AppendTest);
    RunTest("ContainsTest()",                  &pass, ContainsTest);
    RunTest("ReverseTest()",                   &pass, ReverseTest);
    RunTest("ConversionTests()",               &pass, ConversionTests);

    return pass;
}

}
