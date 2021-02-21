#pragma once

#include <cmath>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ctk/ctk.h"
#include "ctk/math.h"
#include "ctk/tests/utils.h"

#define MATRIX_COL_WIDTH            8u
#define MATRIX_PRINT_WIDTH          MATRIX_COL_WIDTH * 4u
#define MATRIX_OUTPUT_COL_PADDING   2u

struct MatrixGroup {
    CTK_Matrix ctk;
    glm::mat4 glm;
};

struct MatrixOutput {
    Surface surface;
    u32 col;
};

static MatrixGroup create_matrix_group() {
    return { CTK_MATRIX_ID, glm::mat4(1) };
}

static u32 maj_min_idx(u32 maj, u32 min, u32 min_max) {
    return (maj * min_max) + min;
}

static void set(MatrixGroup *mg, u32 col, u32 row, f32 v) {
    u32 idx = maj_min_idx(col, row, 4);
    mg->ctk[col][row] = v;
    mg->glm[col][row] = v;
}

static MatrixOutput create_matrix_output() {
    return { create_surface(150, 11), 1 };
}

static void reset(MatrixOutput *mo) {
    clear(&mo->surface);
    mo->col = 1;
}

template<typename ...Args>
static u32 write(MatrixOutput *mo, u32 y, cstr msg, Args... args) {
    return write(&mo->surface, mo->col, y, msg, args...);
}

static void write_matrix(Surface *s, u32 x, u32 y, f32 *data) {
    for (u32 col = 0; col < 4; ++col)
    for (u32 row = 0; row < 4; ++row) {
        f32 val = data[maj_min_idx(col, row, 4)];

        // Prevent -0.00 from sign bit still being set.
        if (signbit(val) && fabs(val) < 0.000001f)
            val *= -1;

        write(s, (col * MATRIX_COL_WIDTH) + x, row + y, "%s%.2f", val < 0 ? "" : " ", val);
    }
}

static void write_vector(Surface *s, u32 x, u32 y, CTK_Vector3<f32> v) {
    for (u32 row = 0; row < 3; ++row)
        write(s, x, row + y, "%.2f", v[row]);
}

static void insert_matrix_group(MatrixOutput *mo, MatrixGroup *mg) {
    u32 y = 0;
    write(mo, y, "CTK");

    y += 1;
    write_matrix(&mo->surface, mo->col, y, mg->ctk.data);

    y += 5;
    write(mo, y, "GLM");

    y += 1;
    write_matrix(&mo->surface, mo->col, y, &mg->glm[0][0]);

    mo->col += (MATRIX_COL_WIDTH * 4) + MATRIX_OUTPUT_COL_PADDING;
}

static void insert_vector(MatrixOutput *mo, CTK_Vector3<f32> v, cstr name) {
    u32 op_width = write(mo, 0, name);
    write_vector(&mo->surface, mo->col, 1, v);
    mo->col += ctk_max(op_width, MATRIX_COL_WIDTH) + MATRIX_COL_WIDTH;
}

static void insert_op(MatrixOutput *mo, cstr op) {
    mo->col += write(mo, 0, op) + MATRIX_COL_WIDTH;
}

static MatrixGroup add(MatrixGroup *a, MatrixGroup *b, MatrixOutput *mo) {
    MatrixGroup res = create_matrix_group();
    res.ctk = a->ctk + b->ctk;
    res.glm = a->glm + b->glm;
    return res;
}

static void write_add(MatrixGroup *a, MatrixGroup *b, MatrixGroup *res, MatrixOutput *mo) {
    insert_matrix_group(mo, a);
    insert_op(mo, "ADD");
    insert_matrix_group(mo, b);
    insert_op(mo, "EQUALS");
    insert_matrix_group(mo, res);
}

static MatrixGroup multiply(MatrixGroup *a, MatrixGroup *b, MatrixOutput *mo) {
    MatrixGroup res = create_matrix_group();
    res.ctk = a->ctk * b->ctk;
    res.glm = a->glm * b->glm;
    return res;
}

static void write_multiply(MatrixGroup *a, MatrixGroup *b, MatrixGroup *res, MatrixOutput *mo) {
    insert_matrix_group(mo, a);
    insert_op(mo, "MULTIPLY");
    insert_matrix_group(mo, b);
    insert_op(mo, "EQUALS");
    insert_matrix_group(mo, res);
}

static MatrixGroup translate(MatrixGroup *mg, CTK_Vector3<f32> v) {
    MatrixGroup res = create_matrix_group();
    res.ctk = ctk_translate(mg->ctk, v);
    res.glm = glm::translate(mg->glm, { v.x, v.y, v.z });
    return res;
}

static MatrixGroup scale(MatrixGroup *mg, CTK_Vector3<f32> v) {
    MatrixGroup res = create_matrix_group();
    res.ctk = ctk_scale(mg->ctk, v);
    res.glm = glm::scale(mg->glm, { v.x, v.y, v.z });
    return res;
}

static MatrixGroup rotate(MatrixGroup *mg, f32 degrees, s32 axis) {
    MatrixGroup res = create_matrix_group();
    res.ctk = ctk_rotate(mg->ctk, degrees, axis);
    res.glm = glm::rotate(mg->glm, glm::radians(degrees), {
                              axis == CTK_AXIS_X ? 1 : 0,
                              axis == CTK_AXIS_Y ? 1 : 0,
                              axis == CTK_AXIS_Z ? 1 : 0
                          });
    return res;
}

static void populate_add(MatrixGroup *m) {
    for (u32 col = 0; col < 4; ++col)
    for (u32 row = 0; row < 4; ++row) {
        m->ctk[col][row] = col + row;
        m->glm[col][row] = col + row;
    }
}

static void populate_mul(MatrixGroup *m) {
    for (u32 col = 0; col < 4; ++col)
    for (u32 row = 0; row < 4; ++row) {
        m->ctk[col][row] = col * row;
        m->glm[col][row] = col * row;
    }
}

static void add_test(MatrixOutput *mo) {
    ctk_print_line("\n\n====================================== ADD TEST ======================================");

    MatrixGroup a = create_matrix_group();
    MatrixGroup b = create_matrix_group();
    populate_add(&a);
    populate_mul(&b);

    MatrixGroup res = add(&a, &b, mo);
    write_add(&a, &b, &res, mo);
    print(&mo->surface);
    reset(mo);

    res = add(&b, &a, mo);
    write_add(&b, &a, &res, mo);
    print(&mo->surface);
    reset(mo);
}

static void multiply_test(MatrixOutput *mo) {
    ctk_print_line("\n\n====================================== MULTIPLY TEST ======================================");

    MatrixGroup a = create_matrix_group();
    MatrixGroup b = create_matrix_group();
    populate_add(&a);
    populate_mul(&b);

    MatrixGroup res = multiply(&a, &b, mo);
    write_multiply(&a, &b, &res, mo);
    print(&mo->surface);
    reset(mo);

    res = multiply(&b, &a, mo);
    write_multiply(&b, &a, &res, mo);
    print(&mo->surface);
    reset(mo);
}

static void translate_test(MatrixOutput *mo) {
    ctk_print_line("\n\n====================================== TRANSLATE TEST ======================================");

    MatrixGroup mg = create_matrix_group();

    CTK_Vector3<f32> trans = { 0, 0, 1 };
    mg = rotate(&mg, 45, CTK_AXIS_Y);
    MatrixGroup res = translate(&mg, trans);

    insert_matrix_group(mo, &mg);
    insert_op(mo, "TRANSLATE");
    insert_vector(mo, trans, "TRANSLATION");
    insert_op(mo, "EQUALS");
    insert_matrix_group(mo, &res);

    print(&mo->surface);
    reset(mo);
}

static void rotate_test(MatrixOutput *mo, s32 axis) {
    ctk_print_line("\n\n====================================== ROTATE TEST ======================================");
    MatrixGroup mg = create_matrix_group();

    f32 degrees = 90;
    mg = rotate(&mg, degrees, axis);
    mg = rotate(&mg, degrees, axis);
    MatrixGroup res = translate(&mg, { 3, 1, 2 });

    insert_matrix_group(mo, &mg);
    insert_op(mo, "ROTATE");

    write(mo, 0, "ROTATION");
    write(mo, 1, "%.2f", degrees);
    mo->col += MATRIX_COL_WIDTH + write(mo, 2, "%s",
                                        axis == CTK_AXIS_X ? "CTK_AXIS_X" :
                                        axis == CTK_AXIS_Y ? "CTK_AXIS_Y" :
                                        axis == CTK_AXIS_Z ? "CTK_AXIS_Z" :
                                        "unknown");

    insert_op(mo, "EQUALS");
    insert_matrix_group(mo, &res);

    print(&mo->surface);
    reset(mo);
}

static void scale_test(MatrixOutput *mo) {
    ctk_print_line("\n\n====================================== SCALE TEST ======================================");
    MatrixGroup mg = create_matrix_group();
    CTK_Vector3<f32> scl = { 1, 0, 0 };
    MatrixGroup res = scale(&mg, scl);

    insert_matrix_group(mo, &mg);
    insert_op(mo, "SCALE");
    insert_vector(mo, scl, "SCALE");
    insert_op(mo, "EQUALS");
    insert_matrix_group(mo, &res);

    print(&mo->surface);
    reset(mo);
}

static void matrix_tests() {
    MatrixOutput mo = create_matrix_output();
    // add_test(&mo);
    multiply_test(&mo);
    translate_test(&mo);
    rotate_test(&mo, CTK_AXIS_X);
    // rotate_test(&mo, CTK_AXIS_Y);
    // rotate_test(&mo, CTK_AXIS_Z);
    // scale_test(&mo);
}

static void perf_test() {

#if 1
#if _DEBUG
    static u32 const ROTATE_TEST_CYCLES = 1000000;
#else
    static u32 const ROTATE_TEST_CYCLES = 100000000;
#endif
    {
        CTK_Matrix m = CTK_MATRIX_ID;
        m = ctk_translate(m, { 1, 2, 3 });
        PROFILE_START(ctk_rotate, ROTATE_TEST_CYCLES)
            m = ctk_rotate_x(m, 90);
            // PROFILE_PROGRESS(ctk_rotate);
        PROFILE_END(ctk_rotate)
    }
    {
        glm::mat4 m(1);
        glm::vec3 axis(1, 0, 0);
        glm::translate(m, { 1, 2, 3 });
        PROFILE_START(glm_rotate, ROTATE_TEST_CYCLES)
            m = glm::rotate(m, glm::radians(90.0f), axis);
            // PROFILE_PROGRESS(glm_rotate);
        PROFILE_END(glm_rotate)
    }
#endif

#if 1
#if _DEBUG
    static u32 const TRANSLATE_TEST_CYCLES = 10000000;
#else
    static u32 const TRANSLATE_TEST_CYCLES = 1000000000;
#endif
    {
        CTK_Matrix m = CTK_MATRIX_ID;
        PROFILE_START(ctk_translate, TRANSLATE_TEST_CYCLES)
            m = ctk_translate(m, { 1, 2, 3 });
            // PROFILE_PROGRESS(ctk_translate);
        PROFILE_END(ctk_translate)
    }
    {
        glm::mat4 m(1);
        glm::vec3 axis(1, 0, 0);
        PROFILE_START(glm_translate, TRANSLATE_TEST_CYCLES)
            m = glm::translate(m, { 1, 2, 3 });
            // PROFILE_PROGRESS(glm_translate);
        PROFILE_END(glm_translate)
    }
#endif

#if 1
#if _DEBUG
    static u32 const MULTIPLY_TEST_CYCLES = 1000000;
#else
    static u32 const MULTIPLY_TEST_CYCLES = 100000000;
#endif
    {
        CTK_Matrix a = {{ 0,  1,  2,  3,
                          4,  5,  6,  7,
                          8,  9,  10, 11,
                          12, 13, 14, 15, }};
        CTK_Matrix b = a;
        PROFILE_START(ctk_multiply, MULTIPLY_TEST_CYCLES)
            a = a * b;
            // PROFILE_PROGRESS(ctk_multiply);
        PROFILE_END(ctk_multiply)
    }
    {
        glm::mat4 a(0,  1,  2,  3,
                    4,  5,  6,  7,
                    8,  9,  10, 11,
                    12, 13, 14, 15);
        glm::mat4 b = a;
        PROFILE_START(glm_multiply, MULTIPLY_TEST_CYCLES)
            a = a * b;
            // PROFILE_PROGRESS(glm_multiply);
        PROFILE_END(glm_multiply)
    }
#endif
}

static void gen_tests() {
}

static void math_tests() {
    matrix_tests();
    perf_test();
    // gen_tests();
}
