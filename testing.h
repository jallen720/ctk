/// Data
////////////////////////////////////////////////////////////
struct TestingState
{
    uint32 tabs;
    uint32 total_tests;
    uint32 passed_tests;
    uint32 failed_tests;
    bool   show_passed_tests;
};

TestingState g_testing_state;

/// Interface
////////////////////////////////////////////////////////////

/// Logging
////////////////////////////////////////////////////////////
template<typename... Args>
void TestPrintLine(const char* msg, Args... args)
{
    PrintTabs(g_testing_state.tabs);
    PrintLine(msg, args...);
}

void PrintExpectedTag()
{
    PrintTabs(g_testing_state.tabs);
    Print(OutputColor::GREEN, "Expected");
    Print(": ");
}

template<typename... Args>
void PrintExpected(const char* msg, Args... args)
{
    PrintExpectedTag();
    PrintLine(msg, args...);
}

void PrintActualTag(bool pass)
{
    PrintTabs(g_testing_state.tabs);
    if (pass)
    {
        Print(OutputColor::GREEN, "Actual");
        Print(":   ");
    }
    else
    {
        Print(OutputColor::RED, "Actual");
        Print(":   ");
    }
}

template<typename... Args>
void PrintActual(bool pass, const char* msg, Args... args)
{
    PrintActualTag(pass);
    PrintLine(msg, args...);
}

/// Test Runners
////////////////////////////////////////////////////////////
template<typename... Args>
void RunTest(const char* description, uint32 description_size, bool* parent_pass, Func<bool, Args...> TestFunc,
                    Args... args)
{
    TestPrintLine(CTK_ANSI_HIGHLIGHT(SKY, "Test") ": %.*s", description_size, description);

    // Run test function at 1 extra tab level.
    g_testing_state.tabs += 1;
    bool test_passed = TestFunc(args...);
    g_testing_state.tabs -= 1;

    // Update test stats.
    g_testing_state.total_tests += 1;
    if (test_passed)
    {
        g_testing_state.passed_tests += 1;
    }
    else
    {
        g_testing_state.failed_tests += 1;
    }

    // Only update parent's pass flag if the test func failed.
    if (parent_pass != NULL && !test_passed)
    {
        *parent_pass = false;
    }

    // Output pass/fail based on test function result (not parent_pass).
    if (test_passed)
    {
        TestPrintLine(CTK_ANSI_HIGHLIGHT(GREEN, "Pass"));
    }
    else
    {
        TestPrintLine(CTK_ANSI_HIGHLIGHT(RED, "Fail"));
    }
}

template<typename... Args>
void RunTest(const char* description, bool* parent_pass, Func<bool, Args...> TestFunc, Args... args)
{
    RunTest(description, StringSize(description), parent_pass, TestFunc, args...);
}

template<uint32 description_size, typename... Args>
void RunTest(FString<description_size>* description, bool* parent_pass, Func<bool, Args...> TestFunc, Args... args)
{
    RunTest(description->data, description_size, parent_pass, TestFunc, args...);
}

template<typename ReturnType, typename ...Args>
bool ExpectFatalError(Func<ReturnType, Args...> TestFunc, Args... args)
{
    bool pass = false;
    const char* actual = NULL;
    try
    {
        TestFunc(args...);
        actual = "No Fatal Error";
    }
    catch (sint32 e)
    {
        pass = true;
        actual = "Fatal Error";
    }

    if (g_testing_state.show_passed_tests || !pass)
    {
        PrintExpected("Fatal Error");
        PrintActual(pass, actual);
    }

    return pass;
}

/// Explicitly sized char arrays
////////////////////////////////////////////////////////////
bool ExpectEqual(const char* expected, uint32 expected_size, const char* actual, uint32 actual_size)
{
    bool pass = StringsMatch(expected, expected_size, actual, actual_size);

    if (g_testing_state.show_passed_tests || !pass)
    {
        PrintExpectedTag();
        Print('\"');
        PrintASCIIString(expected, expected_size, 128);
        Print('\"');
        PrintLine();

        PrintActualTag(pass);
        Print('\"');
        PrintASCIIString(actual, actual_size, 128);
        Print('\"');
        PrintLine();
    }

    return pass;
}

/// String
////////////////////////////////////////////////////////////
bool ExpectEqual(const char* expected, const char* actual)
{
    return ExpectEqual(expected, StringSize(expected), actual, StringSize(actual));
}

bool ExpectEqual(const char* expected, uint32 expected_count, String* actual)
{
    return ExpectEqual(expected, expected_count, actual->data, actual->count);
}

bool ExpectEqual(const char* expected, String* actual)
{
    return ExpectEqual(expected, StringSize(expected), actual->data, actual->count);
}

bool ExpectEqualFull(const char* expected, uint32 expected_count, String* actual)
{
    return ExpectEqual(expected, expected_count, actual->data, actual->size);
}

bool ExpectEqualFull(const char* expected, String* actual)
{
    return ExpectEqual(expected, StringSize(expected), actual->data, actual->size);
}

/// FString
////////////////////////////////////////////////////////////
template<uint32 size>
bool ExpectEqual(const char* expected, uint32 expected_count, FString<size>* actual)
{
    return ExpectEqual(expected, expected_count, actual->data, actual->count);
}

template<uint32 size>
bool ExpectEqual(const char* expected, FString<size>* actual)
{
    return ExpectEqual(expected, StringSize(expected), actual->data, actual->count);
}

template<uint32 size>
bool ExpectEqualFull(const char* expected, uint32 expected_count, FString<size>* actual)
{
    return ExpectEqual(expected, expected_count, actual->data, size);
}

template<uint32 size>
bool ExpectEqualFull(const char* expected, FString<size>* actual)
{
    return ExpectEqual(expected, StringSize(expected), actual->data, size);
}

/// Scalars
////////////////////////////////////////////////////////////
#define CTK_COMPARISON_FNS(TYPE, FORMAT, COMPARISON, COMPARISON_NAME, EXPECTED_VALUE_PRINT_ARG, ACTUAL_VALUE_PRINT_ARG) \
bool Expect##COMPARISON_NAME##(const char* name, TYPE expected_value, TYPE actual_value) \
{ \
    bool pass = actual_value COMPARISON expected_value; \
    if (g_testing_state.show_passed_tests || !pass) \
    { \
        PrintExpected("%s "#COMPARISON" " FORMAT, name, EXPECTED_VALUE_PRINT_ARG); \
        PrintActual(pass, "%s == " FORMAT, name, ACTUAL_VALUE_PRINT_ARG); \
    } \
    return pass; \
} \
template<uint32 size> \
bool Expect##COMPARISON_NAME##(FString<size>* name, TYPE expected_value, TYPE actual_value) \
{ \
    return Expect##COMPARISON_NAME##(name->data, expected_value, actual_value); \
} \
bool Expect##COMPARISON_NAME##(TYPE expected_value, TYPE actual_value) \
{ \
    return Expect##COMPARISON_NAME##(#TYPE " value", expected_value, actual_value); \
}

#define CTK_EXPECT_FNS(TYPE, FORMAT, EXPECTED_VALUE_PRINT_ARG, ACTUAL_VALUE_PRINT_ARG) \
CTK_COMPARISON_FNS(TYPE, FORMAT, >,  GT,       EXPECTED_VALUE_PRINT_ARG, ACTUAL_VALUE_PRINT_ARG) \
CTK_COMPARISON_FNS(TYPE, FORMAT, <,  LT,       EXPECTED_VALUE_PRINT_ARG, ACTUAL_VALUE_PRINT_ARG) \
CTK_COMPARISON_FNS(TYPE, FORMAT, >=, GTEqual,  EXPECTED_VALUE_PRINT_ARG, ACTUAL_VALUE_PRINT_ARG) \
CTK_COMPARISON_FNS(TYPE, FORMAT, <=, LTEqual,  EXPECTED_VALUE_PRINT_ARG, ACTUAL_VALUE_PRINT_ARG) \
CTK_COMPARISON_FNS(TYPE, FORMAT, ==, Equal,    EXPECTED_VALUE_PRINT_ARG, ACTUAL_VALUE_PRINT_ARG) \
CTK_COMPARISON_FNS(TYPE, FORMAT, !=, NotEqual, EXPECTED_VALUE_PRINT_ARG, ACTUAL_VALUE_PRINT_ARG)

CTK_EXPECT_FNS(uint64,  "%llu", expected_value, actual_value)
CTK_EXPECT_FNS(uint32,  "%u",   expected_value, actual_value)
CTK_EXPECT_FNS(uint16,  "%u",   expected_value, actual_value)
CTK_EXPECT_FNS(uint8,   "%u",   expected_value, actual_value)
CTK_EXPECT_FNS(sint64,  "%lli", expected_value, actual_value)
CTK_EXPECT_FNS(sint32,  "%i",   expected_value, actual_value)
CTK_EXPECT_FNS(sint16,  "%i",   expected_value, actual_value)
CTK_EXPECT_FNS(sint8,   "%i",   expected_value, actual_value)
CTK_EXPECT_FNS(float32, "%f",   expected_value, actual_value)
CTK_EXPECT_FNS(float64, "%f",   expected_value, actual_value)
CTK_EXPECT_FNS(bool,    "%s",   (expected_value ? "true" : "false"), (actual_value ? "true" : "false"))

void SetShowPassedTests(bool show_passed_tests)
{
    g_testing_state.show_passed_tests = show_passed_tests;
}

void ShowTestStats()
{
    PrintLine();
    PrintLine("Total Tests:  %u", g_testing_state.total_tests);
    PrintLine(CTK_ANSI_HIGHLIGHT(GREEN, "Passed") " Tests: %u", g_testing_state.passed_tests);
    PrintLine(CTK_ANSI_HIGHLIGHT(RED, "Failed") " Tests: %u", g_testing_state.failed_tests);
}

