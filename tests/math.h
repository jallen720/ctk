#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ctk/ctk.h"
#include "ctk/math.h"
#include "ctk/tests/utils.h"

#define MATRIX_COL_WIDTH    6
#define MATRIX_PRINT_WIDTH  MATRIX_COL_WIDTH * 4

struct MatrixGroup {
    CTK_Matrix ctk;
    glm::mat4 glm;
};

struct MatrixOutput {
    Surface surface;
    u32 col;
};

static MatrixGroup create_matrix_group() {
    return {
        CTK_MATRIX_ID,
        glm::mat4(1),
    };
}

static MatrixOutput create_matrix_output() {
    return {
        create_surface(156, 11),
        1,
    };
}

static void clear(MatrixOutput *mo) {
    clear(&mo->surface);
    mo->col = 1;
}

static void print_matrix(Surface *s, u32 x, u32 y, f32 *data) {
    for (u32 row = 0; row < 4; ++row)
    for (u32 col = 0; col < 4; ++col)
        write(s, x + (col * MATRIX_COL_WIDTH), y + row, "%.2f", data[(col * 4) + row]);
}

static void print_matrix_group(MatrixOutput *mo, MatrixGroup *mg) {
    u32 y = 0;
    write(&mo->surface, mo->col, y, "CTK");
    y += 1;
    print_matrix(&mo->surface, mo->col, y, mg->ctk.data);
    y += 5;
    write(&mo->surface, mo->col, y, "GLM");
    y += 1;
    print_matrix(&mo->surface, mo->col, y, &mg->glm[0][0]);
    mo->col += (MATRIX_COL_WIDTH * 4) + 2;
}

static void math_tests() {
    MatrixGroup m = create_matrix_group();
    MatrixOutput mo = create_matrix_output();
    print_matrix_group(&mo, &m);
    print_matrix_group(&mo, &m);
    print(&mo.surface);
}
