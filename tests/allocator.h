#include "ctk/ctk.h"
#include "ctk/memory.h"
#include "ctk/container.h"

static void *ctk_stack_allocate_fn(void *data, u32 size) {
    auto stack = (CTK_Stack *)data;
    return ctk_alloc(stack, size);
}

static CTK_Allocator const STACK_ALLOCATOR_DEFAULT = {
    NULL,
    ctk_stack_allocate_fn,
    NULL,
    NULL,
};

static void test_main() {

}
