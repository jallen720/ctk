#include "ctk/ctk_2.h"
#include "ctk/ctkd.h"

s32 main() {
    // ctkd_node *cfg = ctkd_read("data/config.ctkd");
    ctk_heap *heap = ctk_create_heap(200);
    // ctkd_node *node = ctkd_create_node(&heap);
    _ctk_visualize_heap(heap);

    // Alloc
    auto a = (char *)ctk_alloc_z(heap, 4);
    _ctk_visualize_heap(heap);
    auto b = (char *)ctk_alloc_z(heap, 4);
    _ctk_visualize_heap(heap);
    // memset(a, 'a', 4);
    // memset(b, 'b', 4);
    // ctk_visualize_string((char *)heap->mem, heap->size, false);

    for (u32 i = 1; i < 20; ++i) {
        u32 mem_size = (i + 1) * 4;
        a = (char *)ctk_realloc(heap, a, mem_size);
        _ctk_visualize_heap(heap);
        b = (char *)ctk_realloc(heap, b, mem_size);
        _ctk_visualize_heap(heap);
        ctk_print_line();
        // memset(a, 'a', mem_size);
        // memset(b, 'b', mem_size);
        // ctk_visualize_string((char *)heap->mem, heap->size, false);

    }

    _ctk_validate_heap(heap);

    // a = ctk_realloc(heap, a, 16);
    // _ctk_visualize_heap(heap);
    return 0;
}
