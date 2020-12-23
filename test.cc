#include "ctk/ctk_2.h"
#include "ctk/ctkd.h"

s32 main() {
    // ctkd_node *cfg = ctkd_read("data/config.ctkd");
    ctk_heap *heap = ctk_create_heap(CTK_KILOBYTE);
    // ctkd_node *node = ctkd_create_node(&heap);
    _ctk_visualize_heap(heap);
    return 0;
}
