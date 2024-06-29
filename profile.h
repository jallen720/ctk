/// Data
////////////////////////////////////////////////////////////
struct ProfileNode;
using ProfileNodeHnd = PoolHnd<ProfileNode>;

struct Profile
{
    const char* name;
    clock_t     begin;
    clock_t     end;
    float64     ms;
};

struct ProfileNode
{
    Profile        profile;
    ProfileNodeHnd parent_hnd;
    ProfileNodeHnd next_hnd;
    ProfileNodeHnd child_hnd;
};

struct ProfileTree
{
    Pool<ProfileNode> prof_node_pool;
    ProfileNodeHnd    curr_node_hnd;
};

/// Utils
////////////////////////////////////////////////////////////
static ProfileNode* GetProfileNode(ProfileTree* prof_tree, ProfileNodeHnd prof_node_hnd)
{
    return GetData(&prof_tree->prof_node_pool, prof_node_hnd);
}

static void ClearProfile(ProfileTree* prof_tree, ProfileNodeHnd prof_node_hnd)
{
    // Clear children.
    ProfileNodeHnd child_hnd = GetProfileNode(prof_tree, prof_node_hnd)->child_hnd;
    while (!IsNull(child_hnd))
    {
        ProfileNodeHnd next_hnd = GetProfileNode(prof_tree, child_hnd)->next_hnd;
        ClearProfile(prof_tree, child_hnd);
        child_hnd = next_hnd;
    }

    // Deallocate profile once all children have been deallocated;
    Deallocate(&prof_tree->prof_node_pool, prof_node_hnd);
}

/// Interface
////////////////////////////////////////////////////////////
static void InitProfileTree(ProfileTree* prof_tree, Allocator* allocator, uint32 max_profiles)
{
    InitPool(&prof_tree->prof_node_pool, allocator, max_profiles);

    // Create root profile to nest all created profiles under.
    prof_tree->curr_node_hnd = Allocate(&prof_tree->prof_node_pool);
}

static void DeinitProfileTree(ProfileTree* prof_tree, Allocator* allocator)
{
    DeinitPool(&prof_tree->prof_node_pool, allocator);
    prof_tree->curr_node_hnd.id = NULL_HND;
}

static ProfileTree* CreateProfileTree(Allocator* allocator, uint32 max_profiles)
{
    auto prof_tree = Allocate<ProfileTree>(allocator, 1);
    InitProfileTree(prof_tree, allocator, max_profiles);
    return prof_tree;
}

static void DestroyProfileTree(ProfileTree* prof_tree, Allocator* allocator)
{
    DeinitPool(&prof_tree->prof_node_pool, allocator);
    Deallocate(allocator, prof_tree);
}

static void BeginProfile(Profile* profile, const char* name)
{
    profile->name  = name;
    profile->begin = clock();
}

static ProfileNodeHnd BeginProfile(ProfileTree* prof_tree, const char* name)
{
    ProfileNodeHnd profile_node_hnd = Allocate(&prof_tree->prof_node_pool);

    ProfileNode* profile_node = GetProfileNode(prof_tree, profile_node_hnd);
    profile_node->parent_hnd = prof_tree->curr_node_hnd;

    // Place profile at end of parent's children list.
    ProfileNodeHnd* end = &GetProfileNode(prof_tree, profile_node->parent_hnd)->child_hnd;
    while (!IsNull(*end))
    {
        end = &GetProfileNode(prof_tree, *end)->next_hnd;
    }
    *end = profile_node_hnd;

    prof_tree->curr_node_hnd = profile_node_hnd;

    // Begin profile after profile node has been added to tree.
    BeginProfile(&profile_node->profile, name);

    return profile_node_hnd;
}

static void EndProfile(Profile* profile)
{
    profile->end = clock();
    profile->ms  = (float64)(profile->end - profile->begin) / (CLOCKS_PER_SEC / 1000);
}

static void EndProfile(ProfileTree* prof_tree)
{
    // End profile first.
    ProfileNode* curr_node = GetProfileNode(prof_tree, prof_tree->curr_node_hnd);
    EndProfile(&curr_node->profile);

    // The root profile is the only profile that should have an unset parent.
    if (IsNull(curr_node->parent_hnd))
    {
        CTK_FATAL("can't end profile: current profile has NULL parent (attempting to end root profile?)");
    }

    prof_tree->curr_node_hnd = curr_node->parent_hnd;
}

static void PrintProfile(Profile* profile)
{
    Print("%s: %.f ms", profile->name, profile->ms);
}

static void PrintProfileLine(Profile* profile)
{
    PrintLine("%s: %.f ms", profile->name, profile->ms);
}

static void PrintProfileNode(ProfileTree* prof_tree, ProfileNode* profile_node, uint32 tab_count);

static void PrintChildNodes(ProfileTree* prof_tree, ProfileNodeHnd child_hnd, uint32 tab_count)
{
    while (!IsNull(child_hnd))
    {
        ProfileNode* child_node = GetProfileNode(prof_tree, child_hnd);
        PrintProfileNode(prof_tree, child_node, tab_count);
        child_hnd = child_node->next_hnd;
    }
}

static void PrintProfileNode(ProfileTree* prof_tree, ProfileNode* profile_node, uint32 tab_count)
{
    PrintTabs(tab_count);
    PrintProfileLine(&profile_node->profile);
    PrintChildNodes(prof_tree, profile_node->child_hnd, tab_count + 1);
}

static void PrintProfileTree(ProfileTree* prof_tree)
{
    ProfileNode* curr_node = GetProfileNode(prof_tree, prof_tree->curr_node_hnd);
    if (!IsNull(curr_node->parent_hnd))
    {
        CTK_FATAL("can't print profile tree: profile tree's current profile is not root (still recording profiles)");
    }

    PrintChildNodes(prof_tree, curr_node->child_hnd, 0);
}

static void ClearProfileTree(ProfileTree* prof_tree)
{
    ProfileNode* current = GetProfileNode(prof_tree, prof_tree->curr_node_hnd);
    if (!IsNull(current->parent_hnd))
    {
        CTK_FATAL("can't clear profiles: profile tree's current profile is not root (still recording profiles)");
    }

    // Clear all child profiles of root node.
    ProfileNodeHnd prof_node_hnd = current->child_hnd;
    while (!IsNull(prof_node_hnd))
    {
        ProfileNodeHnd next_hnd = GetProfileNode(prof_tree, prof_node_hnd)->next_hnd;
        ClearProfile(prof_tree, prof_node_hnd);
        prof_node_hnd = next_hnd;
    }

    // Since root profile is not cleared, set children to NULL_HND.
    current->child_hnd.id = NULL_HND;
}

static Profile* GetProfile(ProfileTree* prof_tree, ProfileNodeHnd prof_node_hnd)
{
    return &GetProfileNode(prof_tree, prof_node_hnd)->profile;
}

