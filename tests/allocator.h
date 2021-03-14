#include "ctk/ctk.h"
#include "ctk/memory.h"
#include "ctk/containers.h"

static void test_main() {
    CTK_Stack *stack = ctk_create_stack(128);
    ctk_visualize_stack(stack);
    CTK_Array<char> *chars = ctk_create_array<char>(8, 4, &CTK_SYSTEM_ALLOCATOR);
    ctk_visualize_string(chars);
    ctk_visualize_stack(stack);
    ctk_concat(chars, "test 011");
    ctk_visualize_string(chars);
    ctk_visualize_stack(stack);
}
