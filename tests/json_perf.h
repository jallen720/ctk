#pragma once

namespace JSONPerfTest
{

static void Run()
{
    PrintLine("\nJSON Performance Test");

    Profile prof = {};
    float64 file_size_gb = GetFileSize("tests/data/large.json") / 1000000000.0;
    static constexpr uint32 TEST_PASSES = 10;
    float64 total_ms = 0;
    for (uint32 pass = 0; pass < TEST_PASSES; ++pass)
    {
        FString<16> prof_name = {};
        Write(&prof_name, "pass %2u", pass);
        BeginProfile(&prof, prof_name.data);

        JSON json = LoadJSON(&std_allocator, "tests/data/large.json");
        DestroyJSON(&json);

        EndProfile(&prof);
        PrintProfile(&prof);
        total_ms += prof.ms;
        PrintLine(" (%.3f GB / sec)", file_size_gb / (prof.ms / 1000.0));
    }
    float64 average_ms = total_ms / TEST_PASSES;
    PrintLine("average: %.f ms (%.3f GB / sec)", average_ms, file_size_gb / (average_ms / 1000.0));
}

}
