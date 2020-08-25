#include "ctk/ctk.h"
#include "ctk/data.h"
#include "ctk/math.h"
#include "ctk/memory.h"

int main() {
    ctk::allocate_main_heap(64, 64);
    ctk::visualize_main_heap();
    CTK_ITERATE(2) {
        ctk::print_line("ITERATION ===========================");
        u8* X = ctk::allocate(4);
        ctk::visualize_main_heap();
        u8* Y = ctk::allocate(4);
        ctk::visualize_main_heap();
        ctk::ctk_free(X);
        ctk::visualize_main_heap();
        ctk::ctk_free(Y);
        ctk::visualize_main_heap();
        ctk::print_line("ITERATION ===========================");
    }
    return 0;
}
