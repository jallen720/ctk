#include "ctk/ctk_new.h"
#include "ctk/ctkd.h"

s32 main() {
    // struct ctk_node *cfg = ctk_read("data/config.ctk");
    struct ctk_heap heap = ctk_create_heap(64);
    _ctk_visualize_heap(&heap);
    for (u32 i = 0; i < 2; ++i) {
        auto a = ctk_alloc(&heap, 16);
        _ctk_visualize_heap(&heap);
        auto b = ctk_alloc(&heap, 16);
        _ctk_visualize_heap(&heap);
        auto c = ctk_alloc(&heap, 32);
        _ctk_visualize_heap(&heap);
        ctk_free(&heap, b);
        _ctk_visualize_heap(&heap);
        ctk_free(&heap, c);
        _ctk_visualize_heap(&heap);
        ctk_free(&heap, a);
        _ctk_visualize_heap(&heap);
    }

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
