#include "ctk/ctk.h"
#include "ctk/memory.h"
#include "ctk/containers.h"

static void test_main() {
    CTK_Stack *stack = ctk_create_stack(128);
    ctk_visualize_stack(stack);
    auto str = ctk_create_string("test", &stack->allocator);
    ctk_visualize_stack(stack);
    ctk_visualize_string(str);
}
