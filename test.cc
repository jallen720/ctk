#include "ctk/ctk_new.h"
#include "ctk/ctkd.h"

s32 main() {
    // struct ctkd_node *cfg = ctkd_read("data/config.ctkd");
    struct ctkd_heap heap = ctkd_create_heap(32);
    auto a = ctkd_alloc(&heap, 8);
    _ctkd_visualize_heap(&heap);
    auto b = ctkd_alloc(&heap, 8);
    _ctkd_visualize_heap(&heap);
    auto c = ctkd_alloc(&heap, 8);
    _ctkd_visualize_heap(&heap);
    ctkd_free(&heap, a);
    _ctkd_visualize_heap(&heap);
    ctkd_free(&heap, c);
    _ctkd_visualize_heap(&heap);
    ctkd_free(&heap, b);
    _ctkd_visualize_heap(&heap);

    return 0;
}
