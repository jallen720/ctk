#pragma once

namespace ProfileTest
{

void ProfileTest()
{
    Profile test_profile = {};
    BeginProfile(&test_profile, "test profile");
    Sleep(500);
    EndProfile(&test_profile);
    PrintProfileLine(&test_profile);
}

void ProfileTreeTest()
{
    ProfileTree* profile_tree = CreateProfileTree(&std_allocator, 16);

    BeginProfile(profile_tree, "layer 0");
        BeginProfile(profile_tree, "layer 1 child 0");
            BeginProfile(profile_tree, "layer 2 child 0");
                Sleep(250);
            EndProfile(profile_tree);
            BeginProfile(profile_tree, "layer 2 child 1");
                Sleep(250);
            EndProfile(profile_tree);
        EndProfile(profile_tree);
        BeginProfile(profile_tree, "layer 1 child 1");
            Sleep(250);
        EndProfile(profile_tree);
    EndProfile(profile_tree);
    PrintProfileTree(profile_tree);

    DestroyProfileTree(profile_tree, &std_allocator);
}

void Run()
{
    ProfileTest();
    ProfileTreeTest();
}

}
