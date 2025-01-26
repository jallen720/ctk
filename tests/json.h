namespace JSONTest {

bool ValidTest() {
    bool pass = true;

    JSON json = LoadJSON(&g_std_allocator, "tests/data/valid.json");
    DestroyJSON(&json);

    return pass;
}

bool ScientificENotationTest() {
    bool pass = true;

    JSON json = LoadJSON(&g_std_allocator, "tests/data/scientific_e_notation.json");
    DestroyJSON(&json);

    return pass;
}

bool ObjectKeyTest() {
    bool pass = true;

    JSON json = LoadJSON(&g_std_allocator, "tests/data/object_key_test.json");
    DestroyJSON(&json);

    return pass;
}

bool GLTFTest() {
    bool pass = true;

    JSON json = LoadJSON(&g_std_allocator, "tests/data/gltf_test.json");
    DestroyJSON(&json);

    return pass;
}



bool EndlessStringTest() {
    bool pass = true;

    RunTest("LoadJSON(\"tests/data/endless_string.json\")", &pass,
            ExpectFatalError, LoadJSON, &g_std_allocator, "tests/data/endless_string.json");

    return pass;
}

bool MultiDecimalTest() {
    bool pass = true;

    RunTest("LoadJSON(\"tests/data/multi_decimal.json\")", &pass,
            ExpectFatalError, LoadJSON, &g_std_allocator, "tests/data/multi_decimal.json");

    return pass;
}

bool DecimalFirstTest() {
    bool pass = true;

    RunTest("LoadJSON(\"tests/data/decimal_first.json\")", &pass,
            ExpectFatalError, LoadJSON, &g_std_allocator, "tests/data/decimal_first.json");

    return pass;
}

bool NegativeNoNumTest() {
    bool pass = true;

    RunTest("LoadJSON(\"tests/data/negative_no_num.json\")", &pass,
            ExpectFatalError, LoadJSON, &g_std_allocator, "tests/data/negative_no_num.json");

    return pass;
}

bool NegativeDecimalFirstTest() {
    bool pass = true;

    RunTest("LoadJSON(\"tests/data/negative_decimal_first.json\")", &pass,
            ExpectFatalError, LoadJSON, &g_std_allocator, "tests/data/negative_decimal_first.json");

    return pass;
}

bool EOFBeforeEndOfStringTest() {
    bool pass = true;

    RunTest("LoadJSON(\"tests/data/eof_string.json\")", &pass,
            ExpectFatalError, LoadJSON, &g_std_allocator, "tests/data/eof_string.json");

    return pass;
}

bool NoRootTest() {
    bool pass = true;

    RunTest("LoadJSON(\"tests/data/no_root.json\")", &pass,
            ExpectFatalError, LoadJSON, &g_std_allocator, "tests/data/no_root.json");

    return pass;
}

bool LargeTest() {
    bool pass = true;

    JSON json = LoadJSON(&g_std_allocator, "tests/data/large.json");
    DestroyJSON(&json);

    return pass;
}

bool ValidSearchTest() {
    bool pass = true;

    struct JSONSearch {
        const char*  string;
        JSONNodeType type;
    };

    JSON json = LoadJSON(&g_std_allocator, "tests/data/valid.json");

    JSONSearch searches[] = { { "uint32",              JSONNodeType::UINT32  }, { "sint32",              JSONNodeType::SINT32  }, { "float32",             JSONNodeType::FLOAT32 }, { "string",              JSONNodeType::STRING  }, { "bool",                JSONNodeType::BOOLEAN }, { "null",                JSONNodeType::NULL_   }, { "empty_array",         JSONNodeType::ARRAY   }, { "empty_object",        JSONNodeType::OBJECT  }, { "array",               JSONNodeType::ARRAY   }, { "array[0]",            JSONNodeType::SINT32  }, { "array[1]",            JSONNodeType::UINT32  }, { "array[2]",            JSONNodeType::FLOAT32 }, { "array[3]",            JSONNodeType::STRING  }, { "array[4]",            JSONNodeType::BOOLEAN }, { "array[5]",            JSONNodeType::NULL_   }, { "array[6]",            JSONNodeType::ARRAY   }, { "array[7]",            JSONNodeType::OBJECT  }, { "array[8]",            JSONNodeType::ARRAY   }, { "array[8][0]",         JSONNodeType::STRING  }, { "array[9]",            JSONNodeType::OBJECT  }, { "array[9].name",       JSONNodeType::STRING  }, { "object",              JSONNodeType::OBJECT  }, { "object.uint32",       JSONNodeType::UINT32  }, { "object.sint32",       JSONNodeType::SINT32  }, { "object.float32",      JSONNodeType::FLOAT32 }, { "object.string",       JSONNodeType::STRING  }, { "object.bool",         JSONNodeType::BOOLEAN }, { "object.null",         JSONNodeType::NULL_   }, { "object.empty_array",  JSONNodeType::ARRAY   }, { "object.empty_object", JSONNodeType::OBJECT  }, { "object.array",        JSONNodeType::ARRAY   }, { "object.array[0]",     JSONNodeType::STRING  }, { "object.object",       JSONNodeType::OBJECT  }, { "object.object.name",  JSONNodeType::STRING  },
    };
    CTK_ITER_PTR(search, searches, CTK_ARRAY_SIZE(searches)) {
        JSONNode* node = SearchNode(&json, search->string);
        FString<32> name = {};
        Write(&name, "%s", search->string);
        RunTest(&name, &pass, ExpectEqual, NodeTypeName(search->type), NodeTypeName(node->type));
    }

    RunTest("SearchUInt32 (uint32)",  &pass, ExpectEqual, 1u,                   SearchUInt32 (&json, "uint32"));
    RunTest("SearchSInt32 (sint32)",  &pass, ExpectEqual, -1,                   SearchSInt32 (&json, "sint32"));
    RunTest("SearchFloat32(float32)", &pass, ExpectEqual, -2.3f,                SearchFloat32(&json, "float32"));
    RunTest("SearchString (string)",  &pass, ExpectEqual, "this is a \"test\"", SearchString (&json, "string"));
    RunTest("SearchBoolean(bool)",    &pass, ExpectEqual, true,                 SearchBoolean(&json, "bool"));

    JSONNode* array = SearchArray(&json, "array");
    RunTest("SearchSInt32 (array, [0])",      &pass, ExpectEqual, -1,             SearchSInt32 (&json, array, "[0]"));
    RunTest("SearchUInt32 (array, [1])",      &pass, ExpectEqual, 1u,             SearchUInt32 (&json, array, "[1]"));
    RunTest("SearchFloat32(array, [2])",      &pass, ExpectEqual, 2.3f,           SearchFloat32(&json, array, "[2]"));
    RunTest("SearchString (array, [3])",      &pass, ExpectEqual, "array string", SearchString (&json, array, "[3]"));
    RunTest("SearchBoolean(array, [4])",      &pass, ExpectEqual, true,           SearchBoolean(&json, array, "[4]"));
    RunTest("SearchBoolean(array, [8][0])",   &pass, ExpectEqual, "array array",  SearchString (&json, array, "[8][0]"));
    RunTest("SearchBoolean(array, [9].name)", &pass, ExpectEqual, "array object", SearchString (&json, array, "[9].name"));

    JSONNode* object = SearchObject(&json, "object");
    RunTest("SearchUInt32 (object, uint32)",      &pass, ExpectEqual, 4u,              SearchUInt32 (&json, object, "uint32"));
    RunTest("SearchSInt32 (object, sint32)",      &pass, ExpectEqual, -4,              SearchSInt32 (&json, object, "sint32"));
    RunTest("SearchFloat32(object, float32)",     &pass, ExpectEqual, 5.6f,            SearchFloat32(&json, object, "float32"));
    RunTest("SearchString (object, string)",      &pass, ExpectEqual, "object string", SearchString (&json, object, "string"));
    RunTest("SearchBoolean(object, bool)",        &pass, ExpectEqual, false,           SearchBoolean(&json, object, "bool"));
    RunTest("SearchBoolean(object, array[0])",    &pass, ExpectEqual, "object array",  SearchString (&json, object, "array[0]"));
    RunTest("SearchBoolean(object, object.name)", &pass, ExpectEqual, "object object", SearchString (&json, object, "object.name"));

    DestroyJSON(&json);

    return pass;
}

bool InvalidSearchTest() {
    bool pass = true;

    JSON json = LoadJSON(&g_std_allocator, "tests/data/valid.json");
    RunTest<Func<JSONNode*, JSON*, const char*>>(
        "SearchNode(&json, \"invalid_key\")", &pass, ExpectFatalError, SearchNode, &json, "invalid_key");
    DestroyJSON(&json);

    return pass;
}

bool ValidGetTest() {
    bool pass = true;

    JSON json = LoadJSON(&g_std_allocator, "tests/data/valid.json");

    RunTest("GetUInt32 (uint32)",  &pass, ExpectEqual, 1u,                   GetUInt32 (&json, "uint32"));
    RunTest("GetSInt32 (sint32)",  &pass, ExpectEqual, -1,                   GetSInt32 (&json, "sint32"));
    RunTest("GetFloat32(float32)", &pass, ExpectEqual, -2.3f,                GetFloat32(&json, "float32"));
    RunTest("GetString (string)",  &pass, ExpectEqual, "this is a \"test\"", GetString(&json, "string"));
    RunTest("GetBoolean(bool)",    &pass, ExpectEqual, true,                 GetBoolean(&json, "bool"));

    JSONNode* array = GetArray(&json, "array");
    RunTest("GetSInt32 (array, 0)", &pass, ExpectEqual, -1,             GetSInt32 (&json, array, 0u));
    RunTest("GetUInt32 (array, 1)", &pass, ExpectEqual, 1u,             GetUInt32 (&json, array, 1));
    RunTest("GetFloat32(array, 2)", &pass, ExpectEqual, 2.3f,           GetFloat32(&json, array, 2));
    RunTest("GetString (array, 3)", &pass, ExpectEqual, "array string", GetString (&json, array, 3));
    RunTest("GetBoolean(array, 4)", &pass, ExpectEqual, true,           GetBoolean(&json, array, 4));

    JSONNode* array_array = GetArray(&json, array, 8);
    RunTest("GetString(array[8], 0)", &pass, ExpectEqual, "array array", GetString(&json, array_array, 0u));

    JSONNode* array_object = GetObject(&json, array, 9);
    RunTest("GetString(array[9], name)", &pass, ExpectEqual, "array object", GetString(&json, array_object, "name"));

    JSONNode* object = GetObject(&json, "object");
    RunTest("GetUInt32 (object, uint32)",          &pass, ExpectEqual, 4u,              GetUInt32 (&json, object, "uint32"));
    RunTest("GetSInt32 (object, sint32)",          &pass, ExpectEqual, -4,              GetSInt32 (&json, object, "sint32"));
    RunTest("GetFloat32(object, float32)",         &pass, ExpectEqual, 5.6f,            GetFloat32(&json, object, "float32"));
    RunTest("GetString (object, string)",          &pass, ExpectEqual, "object string", GetString (&json, object, "string"));
    RunTest("GetBoolean(object, 4 (key == bool))", &pass, ExpectEqual, false,           GetBoolean(&json, object, 4));

    JSONNode* object_array = GetArray(&json, object, "array");
    RunTest("GetString(object.array, 0)", &pass, ExpectEqual, "object array", GetString(&json, object_array, 0u));

    JSONNode* object_object = GetObject(&json, object, "object");
    RunTest("GetString(object.object, name)", &pass, ExpectEqual, "object object", GetString(&json, object_object, "name"));

    DestroyJSON(&json);

    return pass;
}

bool Run() {
    bool pass = true;

    TempStack_Init(&g_std_allocator, Kilobyte32<4>());

    RunTest("ValidTest()",                &pass, ValidTest);
    RunTest("ScientificENotationTest()",  &pass, ScientificENotationTest);
    RunTest("ObjectKeyTest()",            &pass, ObjectKeyTest);
    RunTest("GLTFTest()",                 &pass, GLTFTest);
    RunTest("EndlessStringTest()",        &pass, EndlessStringTest);
    RunTest("MultiDecimalTest()",         &pass, MultiDecimalTest);
    RunTest("DecimalFirstTest()",         &pass, DecimalFirstTest);
    RunTest("NegativeNoNumTest()",        &pass, NegativeNoNumTest);
    RunTest("NegativeDecimalFirstTest()", &pass, NegativeDecimalFirstTest);
    RunTest("EOFBeforeEndOfStringTest()", &pass, EOFBeforeEndOfStringTest);
    RunTest("NoRootTest()",               &pass, NoRootTest);
    RunTest("LargeTest()",                &pass, LargeTest);
    RunTest("ValidSearchTest()",          &pass, ValidSearchTest);
    RunTest("InvalidSearchTest()",        &pass, InvalidSearchTest);
    RunTest("ValidGetTest()",             &pass, ValidGetTest);

    TempStack_Deinit();

    return pass;
}

}
