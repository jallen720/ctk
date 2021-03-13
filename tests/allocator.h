#include "ctk/ctk.h"
#include "ctk/memory.h"
#include "ctk/containers.h"

static void test_main() {
    CTK_Stack *stack = ctk_create_stack(128);
    ctk_visualize_stack(stack);
    auto chars = ctk_create_array<char>(8, 4, &stack->allocator);
    ctk_visualize_string(chars);
    ctk_visualize_stack(stack);
}
