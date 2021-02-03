#include <cstdio>
#include <ctime>
#include "ctk/ctk.h"
#include "ctk/free_list.h"
#include "ctk/ctkd.h"

static void populate(CTK_FreeList *free_list, u32 *allocs, u32 num_allocs, void **output = NULL) {
    for (u32 i = 0; i < num_allocs; ++i) {
        void *alloc = ctk_alloc(free_list, allocs[i]);
        if (output)
            output[i] = alloc;
    }
}

static void interactive() {
    static const u32 BUF_SZ = 64;
    char buf[BUF_SZ] = {};
    CTK_FreeList free_list = ctk_create_free_list(4 * CTK_KILOBYTE);
    // u32 alloc_sizes[] = { 10, 20 };
    // populate(&free_list, alloc_sizes, CTK_ARRAY_COUNT(alloc_sizes));

    while (1) {
        _ctk_debug_free_list(&free_list);
        ctk_print("enter command: ");

        if (gets_s(buf, BUF_SZ) == NULL)
            continue;

        char *cmd = buf;

        if (*cmd == 'a') {
            char *arg = buf + 2;
            u32 alloc_size = strtoul(arg, NULL, 10);
            if (alloc_size > 0) {
                ctk_print_line("allocationg %u bytes", alloc_size);
                ctk_alloc(&free_list, alloc_size);
            }
        }
        else if (*cmd == 'f') {
            char *arg = buf + 2;
            u64 num = strtoull(arg, NULL, 16);
            auto mem_addr = (void *)num;
            if (mem_addr) {
                ctk_print_line("freeing block %p", mem_addr);
                ctk_free(&free_list, mem_addr);
            }
        }
        // else if (*cmd == 'r') {
        //     char *args[2] = { buf + 2 };

        //     for (u32 i = 3; i < BUF_SZ; ++i) {
        //         if (buf[i] == ' ') {
        //             buf[i] = '\0';
        //             args[1] = buf + i + 1;
        //             break;
        //         }
        //     }

        //     if (args[1] == NULL) {
        //         ctk_print_line("failed to parse second arg");
        //     }
        //     else {
        //         u32 realloc_size = strtoul(args[1], NULL, 10);
        //         auto mem_addr = (void *)strtoull(args[0], NULL, 16);
        //         if (mem_addr) {
        //             ctk_print_line("reallocating block %p", mem_addr);
        //             ctk_realloc(&free_list, mem_addr, realloc_size);
        //         }
        //     }
        // }
        else {
            break;
        }
    }
    ctk_free(&free_list);
}

static void performance() {
    static u64 const TEST_CYCLES = 10000000;
#if 1
    {
        auto allocs = (void **)malloc(TEST_CYCLES * sizeof(void *));

        // Alloc
        clock_t start = clock();

        for (u64 i = 0; i < TEST_CYCLES; ++i) {
            allocs[i] = malloc(64);
            CTK_ASSERT(allocs[i]);
        }

        clock_t end = clock();
        f64 ms = (f64)(end - start) / (CLOCKS_PER_SEC / 1000.0);
        ctk_print_line("malloc() ms: %f", ms);

        // Free
        start = clock();

        for (u64 i = 0; i < TEST_CYCLES; ++i) {
            free(allocs[i]);
        }

        end = clock();
        ms = (f64)(end - start) / (CLOCKS_PER_SEC / 1000.0);
        ctk_print_line("free() ms: %f", ms);

        free(allocs);
    }
    ctk_print_line();
#endif
#if 1
    {
        CTK_FreeList free_list = ctk_create_free_list(2 * CTK_GIGABYTE);
        auto allocs = (void **)ctk_alloc(&free_list, TEST_CYCLES * sizeof(void *));

        // Alloc
        clock_t start = clock();

        for (u64 i = 0; i < TEST_CYCLES; ++i) {
            allocs[i] = ctk_alloc(&free_list, 64);
            CTK_ASSERT(allocs[i]);
        }

        clock_t end = clock();
        f64 ms = (f64)(end - start) / (CLOCKS_PER_SEC / 1000.0);
        ctk_print_line("ctk_alloc(free_list) ms: %f", ms);

        // Free
        start = clock();

        for (u64 i = 0; i < TEST_CYCLES; ++i) {
            ctk_free(&free_list, allocs[i]);
        }

        end = clock();
        ms = (f64)(end - start) / (CLOCKS_PER_SEC / 1000.0);
        ctk_print_line("ctk_free(free_list) ms: %f", ms);

        ctk_free(&free_list);
    }
    ctk_print_line();
#endif
#if 0
    {
        CTK_Stack stack = ctk_create_stack(CTK_GIGABYTE);
        clock_t start = clock();

        for (u64 i = 0; i < TEST_CYCLES; ++i) {
            void *a = ctk_push(&stack, 64);
            CTK_ASSERT(a);
        }

        clock_t end = clock();
        f64 ms = (f64)(end - start) / (CLOCKS_PER_SEC / 1000.0);
        ctk_print_line("ctk_alloc(stack) ms: %f", ms);
    }
    ctk_print_line();
#endif
}

static void test_0() {
    CTK_FreeList free_list = ctk_create_free_list(1000);
    u32 alloc_sizes[] = { 10, 20, 30, 40, 50, 60, 70 };
    void *allocs[CTK_ARRAY_COUNT(alloc_sizes)] = {};
    populate(&free_list, alloc_sizes, CTK_ARRAY_COUNT(alloc_sizes), allocs);
    _ctk_debug_free_list(&free_list);
//  allocs[0] = ctk_alloc(&free_list, 10);
//  _ctk_debug_free_list(&free_list);
//  ctk_free(&free_list, allocs[0]);
//  _ctk_debug_free_list(&free_list);
//  allocs[0] = ctk_alloc(&free_list, 10);
//  _ctk_debug_free_list(&free_list);
//  ctk_free(&free_list, allocs[0]);
//  _ctk_debug_free_list(&free_list);
}

static void leak_test() {
    CTK_FreeList free_list = {};
    bool allocated = false;
    static const u32 BUF_SZ = 64;
    char buf[BUF_SZ] = {};

    while (1) {
        ctk_print("enter command: ");
        if (gets_s(buf, BUF_SZ) == NULL)
            continue;

        if (*buf == 's') {
            if (allocated)
                ctk_free(&free_list);
            else
                free_list = ctk_create_free_list(100 * CTK_MEGABYTE);

            allocated = !allocated;
        }
        else {
            break;
        }
    }
}

static void free_list_tests() {
    // interactive();
    performance();
    // test_0();
    // leak_test();
}

static void print_node(CTK_Node *n, u32 tab = 0) {
    _ctk_debug_node(n, tab);
    ctk_print_line();
}

static void print_char_array(CTK_Array<char> *a, u32 tab = 0) {
    ctk_print("\n");
    ctk_visualize_string(a->data, a->size);
    ctk_print_array(a);
}

s32 main() {
    free_list_tests();
    // CTK_Node *config = ctk_read("data/config.ctkd");
    // CTK_String output = ctk_create_string(CTK_KILOBYTE, CTK_KILOBYTE);
    // ctk_print_node_children(&output, config);
    // ctk_print("%.*s", output.size, output.data);
    // ctk_push_f32(config, "test", 12.34f);
    // ctk_clear(&output);
    // ctk_print_node_children(&output, config);
    // ctk_print("%.*s", output.size, output.data);
    // cstr search = "test";
    // ctk_print("%s %s", search, ctk_find(config, search)->value.data);

    return 0;
}
