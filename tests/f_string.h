#pragma once

namespace FStringTest
{

/// Utils
////////////////////////////////////////////////////////////
template<uint32 size>
bool TestStringFields(FString<size>* string, uint32 expected_size, uint32 expected_count)
{
    bool pass = true;

    if (!ExpectEqual("FString size", expected_size, size))
    {
        pass = false;
    }

    if (!ExpectEqual("string->count", expected_count, string->count))
    {
        pass = false;
    }

    return pass;
}

/// Tests
////////////////////////////////////////////////////////////
bool CanPushTest()
{
    bool pass = true;

    FString<2> string = {};

    if (!ExpectEqual("string(size=2,count=0); CanPush(string, 2)", true, CanPush(&string, 2)))
    {
        pass = false;
    }

    if (!ExpectEqual("string(size=2,count=0); CanPush(string, 3)", false, CanPush(&string, 3)))
    {
        pass = false;
    }

    Push(&string, 'a');
    if (!ExpectEqual("string(size=2,count=1); CanPush(string, 2)", false, CanPush(&string, 2)))
    {
        pass = false;
    }

    return pass;
}

bool RemoveTest()
{
    bool pass = true;

    FString<4> string = {};

    PushRange(&string, "1234");
    RunTest("Init Layout \"1234\"", &pass, ExpectEqual, "1234", &string);

    Remove(&string, 0);
    RunTest("Remove(string, 0)", &pass, ExpectEqual, "234", &string);

    Remove(&string, 2);
    RunTest("Remove(string, 2)", &pass, ExpectEqual, "23", &string);

    Remove(&string, 0);
    RunTest("Remove(string, 0)", &pass, ExpectEqual, "3", &string);

    Remove(&string, 0);
    RunTest("Remove(string, 0)", &pass, ExpectEqual, "", &string);

    return pass;
}

bool RemoveRangeTest()
{
    bool pass = true;

    FString<8> string = {};

    PushRange(&string, "12345678");
    RunTest("Init Layout \"12345678\"", &pass, ExpectEqual, "12345678", &string);

    RemoveRange(&string, 0, 2);
    RunTest("RemoveRange(string, 0, 2)", &pass, ExpectEqual, "345678", &string);

    RemoveRange(&string, 4, 2);
    RunTest("RemoveRange(string, 4, 2)", &pass, ExpectEqual, "3456", &string);

    RemoveRange(&string, 0, 2);
    RunTest("RemoveRange(string, 0, 2)", &pass, ExpectEqual, "56", &string);

    RemoveRange(&string, 0, 2);
    RunTest("RemoveRange(string, 0, 2)", &pass, ExpectEqual, "", &string);

    return pass;
}

bool WriteTest()
{
    bool pass = true;

    {
        constexpr uint32 STRING_SIZE = 5;
        FString<STRING_SIZE> string = {}; // Must have room for null-terminator, unlike String.
        Write(&string, "test");
        const char* expected = "test";
        RunTest("Write(&string, \"test\") (equal up to string.count)", &pass,
                ExpectEqual, expected, &string);
        RunTest("Write(&string, \"test\") (equal up to string.count)", &pass,
                TestStringFields, &string, STRING_SIZE, 4u);
    }
    {
        constexpr uint32 STRING_SIZE = 8;
        FString<STRING_SIZE> string = {};
        char expected[STRING_SIZE] = {};
        Write(&string, "test");
        Write(expected, STRING_SIZE, "test");
        RunTest("Write(&string, \"test\") (equal up to string size)", &pass,
                ExpectEqualFull, (const char*)expected, STRING_SIZE, &string);
        RunTest("Write(&string, \"test\") (equal up to string size)", &pass,
                TestStringFields, &string, STRING_SIZE, 4u);
    }

    return pass;
}

bool AppendTest()
{
    bool pass = true;

    {
        constexpr uint32 STRING_SIZE = 18;
        char expected[4][STRING_SIZE] = {};
        Write(expected[0], STRING_SIZE, "test1");
        Write(expected[1], STRING_SIZE, "test1test2");
        Write(expected[2], STRING_SIZE, "test1test2test3");
        Write(expected[3], STRING_SIZE, "test1test2test3test4"); // Will truncate "st4"

        FString<STRING_SIZE> string = {};

        Append(&string, "test1");
        RunTest("Append(string, \"test1\")", &pass, ExpectEqualFull, (const char*)expected[0], STRING_SIZE, &string);
        RunTest("Append(string, \"test1\")", &pass, TestStringFields, &string, STRING_SIZE, 5u);

        Append(&string, "test2");
        RunTest("Append(string, \"test2\")", &pass, ExpectEqualFull, (const char*)expected[1], STRING_SIZE, &string);
        RunTest("Append(string, \"test2\")", &pass, TestStringFields, &string, STRING_SIZE, 10u);

        Append(&string, "test3");
        RunTest("Append(string, \"test3\")", &pass, ExpectEqualFull, (const char*)expected[2], STRING_SIZE, &string);
        RunTest("Append(string, \"test3\")", &pass, TestStringFields, &string, STRING_SIZE, 15u);

        Append(&string, "test4");
        RunTest("Append(string, \"test4\")", &pass, ExpectEqualFull, (const char*)expected[3], STRING_SIZE, &string);
        RunTest("Append(string, \"test4\")", &pass, TestStringFields, &string, STRING_SIZE, STRING_SIZE);
    }

    return pass;
}

bool ContainsTest()
{
    bool pass = true;

    FString<4> string = {};
    PushRange(&string, "1234");
    RunTest("Init string: \"1234\"", &pass, TestStringFields, &string, 4u, 4u);

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

    return pass;
}

bool ReverseTest()
{
    bool pass = true;

    FString<16> string = {};

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

    return pass;
}

bool Run()
{
    bool pass = true;

    RunTest("CanPushTest()",     &pass, CanPushTest);
    RunTest("RemoveTest()",      &pass, RemoveTest);
    RunTest("RemoveRangeTest()", &pass, RemoveRangeTest);
    RunTest("WriteTest()",       &pass, WriteTest);
    RunTest("AppendTest()",      &pass, AppendTest);
    RunTest("ContainsTest()",    &pass, ContainsTest);
    RunTest("ReverseTest()",     &pass, ReverseTest);

    return pass;
}

}
