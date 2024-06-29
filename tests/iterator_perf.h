#pragma once

namespace IteratorPerfTest
{

static void Run()
{
    PrintLine("\nIterator Performance Test");

    static constexpr uint32 TEST_COUNT = 1000000000;
    auto a = CreateArrayFull<uint32>(&std_allocator, TEST_COUNT);
    uint32 total;
    for (uint32 i = 0; i < TEST_COUNT; ++i)
    {
        a.data[i] = i;
    }

    Profile profile = {};
    BeginProfile(&profile, "index");
    total = 0;
    for (uint32 i = 0; i < TEST_COUNT; ++i)
    {
        total += a.data[i];
    }
    EndProfile(&profile);
    PrintProfileLine(&profile);

    BeginProfile(&profile, "Get()");
    total = 0;
    for (uint32 i = 0; i < TEST_COUNT; ++i)
    {
        total += Get(&a, i);
    }
    EndProfile(&profile);
    PrintProfileLine(&profile);

    BeginProfile(&profile, "GetPtr()");
    total = 0;
    for (uint32 i = 0; i < TEST_COUNT; ++i)
    {
        total += *GetPtr(&a, i);
    }
    EndProfile(&profile);
    PrintProfileLine(&profile);

    BeginProfile(&profile, "CTK_ITER");
    total = 0;
    CTK_ITER(a_val, &a)
    {
        total += *a_val;
    }
    EndProfile(&profile);
    PrintProfileLine(&profile);
}

}
