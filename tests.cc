#include "ctk/ctk.h"
using namespace CTK;

#include "ctk/tests/f_array.h"
#include "ctk/tests/f_string.h"
#include "ctk/tests/array.h"
#include "ctk/tests/string.h"
#include "ctk/tests/stack.h"
#include "ctk/tests/free_list.h"
#include "ctk/tests/math.h"
#include "ctk/tests/json.h"

#include "ctk/tests/thread_pool.h"
#include "ctk/tests/profile.h"
#include "ctk/tests/window.h"

#include "ctk/tests/json_perf.h"
#include "ctk/tests/free_list_perf.h"
#include "ctk/tests/iterator_perf.h"

sint32 main()
{
    SetShowPassedTests(true);
    // RunTest("FArray",   NULL, FArrayTest::Run);
    // RunTest("FString",  NULL, FStringTest::Run);
    // RunTest("Array",    NULL, ArrayTest::Run);
    // RunTest("String",   NULL, StringTest::Run);
    RunTest("Stack",    NULL, StackTest::Run);
    // RunTest("FreeList", NULL, FreeListTest::Run);
    // RunTest("Math",     NULL, MathTest::Run);
    // RunTest("JSON",     NULL, JSONTest::Run);
    ShowTestStats();

    // ThreadPoolTest::Run();
    // ProfileTest::Run();
    // WindowTest::Run();

    // FreeListPerfTest::Run();
    // JSONPerfTest::Run();
    // IteratorPerfTest::Run();

    return 0;
}
