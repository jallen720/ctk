namespace MathTest {

bool AlignTest() {
    bool pass = true;

    for (uint32 alignment = 1; alignment <= 16; alignment *= 2) {
        for (uint32 i = 1; i < alignment; i += 1) {
            FString<32> description;
            Write(&description, "Align(%u, %u)", i, alignment);
            RunTest(&description, &pass, ExpectEqual, alignment, Align(i, alignment));
        }
        for (uint32 i = alignment + 1; i < alignment * 2; i += 1) {
            FString<32> description;
            Write(&description, "Align(%u, %u)", i, alignment);
            RunTest(&description, &pass, ExpectEqual, alignment * 2, Align(i, alignment));
        }
    }

    return pass;
}

bool Run() {
    bool pass = true;

    RunTest("AlignTest", &pass, AlignTest);

    return pass;
}

}
