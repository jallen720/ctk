#pragma once

namespace IteratorPerfTest {

void Run() {
    PrintLine("\nIterator Performance Test");

    constexpr uint32 TEST_COUNT = 1000000000;
    auto a = CreateArrayFull<uint32>(&g_std_allocator, TEST_COUNT);
    uint32 total;
    for (uint32 i = 0; i < TEST_COUNT; i += 1) {
        a.data[i] = i;
    }

    Profile profile = BeginProfile("index");
    total = 0;
    for (uint32 i = 0; i < TEST_COUNT; i += 1) {
        total += a.data[i];
    }
    EndProfile(&profile);
    PrintProfileLine(&profile);

    profile = BeginProfile("Get()");
    total = 0;
    for (uint32 i = 0; i < TEST_COUNT; i += 1) {
        total += Get(&a, i);
    }
    EndProfile(&profile);
    PrintProfileLine(&profile);

    profile = BeginProfile("GetPtr()");
    total = 0;
    for (uint32 i = 0; i < TEST_COUNT; i += 1) {
        total += *GetPtr(&a, i);
    }
    EndProfile(&profile);
    PrintProfileLine(&profile);

    profile = BeginProfile("CTK_ITER");
    total = 0;
    CTK_ITER(a_val, &a) {
        total += *a_val;
    }
    EndProfile(&profile);
    PrintProfileLine(&profile);
}

}
