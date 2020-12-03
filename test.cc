#include "ctk/ctk_new.h"
#include "ctk/ctkd.h"

#define DEBUG(NODE)\
    ctk_print_line("============================================================================");\
    _ctkd_debug_node(NODE);\
    _ctkd_debug_arena(NODE->arena);\
    ctk_print_line("============================================================================\n");

s32 main() {
    char str_buf[32] = {};
    struct ctkd_node *root = ctkd_create(256, 10 * CTK_KILOBYTE, 16);
    struct ctkd_node *struc = ctkd_push_struct(root, 16, "struct");
    ctkd_push_f32(struc, 12.5f, "f32");
    ctkd_push_f64(struc, 13.510982398, "f64");
    ctkd_push_s32(struc, -2147483648, "s32");
    ctkd_push_s64(struc, -9223372036854775807, "s64");
    ctkd_push_u32(struc, 4294967295, "u32");
    ctkd_push_u64(struc, 18446744073709551615, "u64");
    ctkd_push_bool(struc, false, "bool0");
    ctkd_push_bool(struc, true, "bool1");
    ctkd_push_bool(struc, 0, "bool2");
    struct ctkd_node *array = ctkd_push_array(root, 16, "array");
    ctkd_push_f32(array, 12.5f);
    ctkd_push_f64(array, 13.510982398);
    ctkd_push_s32(array, -2147483648);
    ctkd_push_s64(array, -9223372036854775807);
    ctkd_push_u32(array, 4294967295);
    ctkd_push_u64(array, 18446744073709551615);
    ctkd_push_bool(array, false);
    ctkd_push_bool(array, true);
    ctkd_push_bool(array, 0);
    DEBUG(root)
    ctk_print_line("struct.f32: %f", ctkd_f32(struc, "f32"));
    ctk_print_line("array[0]: %f", ctkd_f32(array, 0u));
    ctkd_read("data/config.ctkd");
    return 0;
}
