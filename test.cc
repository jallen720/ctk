#include "ctk/ctk_new.h"
#include "ctk/ctkd.h"

s32 main() {
    // struct ctk_node *cfg = ctk_read("data/config.ctk");
    struct ctk_heap heap = ctk_create_heap(32);
    _ctk_visualize_heap(&heap);
    char *allocs[4];
    allocs[0] = ctk_alloc_z<char>(&heap, 16);
    _ctk_visualize_heap(&heap);
    allocs[1] = ctk_alloc_z<char>(&heap, 8);
    _ctk_visualize_heap(&heap);
    ctk_free(&heap, allocs[0]);
    _ctk_visualize_heap(&heap);
    allocs[1] = ctk_alloc_z<char>(&heap, 8);
    _ctk_visualize_heap(&heap);

    // auto b = ctk_alloc(&heap, 8);
    // _ctk_visualize_heap(&heap);
    // auto c = ctk_alloc(&heap, 8);
    // _ctk_visualize_heap(&heap);
    // ctk_free(&heap, a);
    // _ctk_visualize_heap(&heap);
    // ctk_free(&heap, c);
    // _ctk_visualize_heap(&heap);
    // ctk_free(&heap, b);
    // _ctk_visualize_heap(&heap);

    return 0;
}
