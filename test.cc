#include "ctk/ctk.h"
#include "ctk/data.h"
#include "ctk/math.h"
#include "ctk/memory.h"

int main() {
    ctk::allocate_main_heap(64, 64);
    ctk::visualize_main_heap();
    CTK_ITERATE(2) {
        char* X = ctk::allocate(4);
        ctk::visualize_main_heap();
        ctk::ctk_free(X);
        ctk::visualize_main_heap();
    }
    {
        char* X = ctk::allocate(8);
        ctk::visualize_main_heap();
        ctk::ctk_free(X);
        ctk::visualize_main_heap();
    }
    return 0;
}
