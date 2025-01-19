#include "ctk/ctk.h"
using namespace CTK;

#include "ctk/tests/utils.h"

// Core
#include "ctk/tests/f_array.h"
#include "ctk/tests/f_string.h"
#include "ctk/tests/math.h"

// Allocators
#include "ctk/tests/stack.h"
#include "ctk/tests/free_list.h"

// Collections
#include "ctk/tests/array.h"
#include "ctk/tests/string.h"

// System
#include "ctk/tests/json.h"
#include "ctk/tests/window.h"
#include "ctk/tests/thread_pool.h"

// Utils
#include "ctk/tests/profile.h"

// Performance Tests
#include "ctk/tests/json_perf.h"
#include "ctk/tests/free_list_perf.h"
#include "ctk/tests/iterator_perf.h"

sint32 main()
{
    SetShowPassedTests(true);

    // Core
    RunTest("FArray",   NULL, FArrayTest::Run);
    RunTest("FString",  NULL, FStringTest::Run);
    RunTest("Math",     NULL, MathTest::Run);

    // Allocators
    RunTest("Stack",    NULL, StackTest::Run);
    RunTest("FreeList", NULL, FreeListTest::Run);

    // Collections
    RunTest("Array",    NULL, ArrayTest::Run);
    RunTest("String",   NULL, StringTest::Run);

    // System
    RunTest("JSON",     NULL, JSONTest::Run);

    ShowTestStats();

    // ThreadPoolTest::Run();
    // ProfileTest::Run();
    // WindowTest::Run();

    // FreeListPerfTest::Run();
    // JSONPerfTest::Run();
    // IteratorPerfTest::Run();

    return 0;
}
