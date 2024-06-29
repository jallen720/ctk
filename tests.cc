#include "ctk/ctk.h"
using namespace CTK;

#include "ctk/tests/array.h"
#include "ctk/tests/farray.h"
#include "ctk/tests/free_list.h"
#include "ctk/tests/free_list_perf.h"
#include "ctk/tests/fstring.h"
#include "ctk/tests/iterator_perf.h"
#include "ctk/tests/json.h"
#include "ctk/tests/json_perf.h"
#include "ctk/tests/math.h"
#include "ctk/tests/profile.h"
#include "ctk/tests/stack.h"
#include "ctk/tests/string.h"
#include "ctk/tests/thread_pool.h"
#include "ctk/tests/window.h"

sint32 main()
{
    // SetShowPassedTests(true);
    RunTest("FreeList", NULL, FreeListTest::Run);
    RunTest("Stack",    NULL, StackTest::Run);
    RunTest("Array",    NULL, ArrayTest::Run);
    RunTest("FArray",   NULL, FArrayTest::Run);
    RunTest("String",   NULL, StringTest::Run);
    RunTest("FString",  NULL, FStringTest::Run);
    RunTest("JSON",     NULL, JSONTest::Run);
    RunTest("Math",     NULL, MathTest::Run);
    ShowTestStats();

    // ThreadPoolTest::Run();
    // ProfileTest::Run();
    // IteratorPerfTest::Run();
    // WindowTest::Run();

    // FreeListPerfTest::Run();
    // JSONPerfTest::Run();

    return 0;
}
