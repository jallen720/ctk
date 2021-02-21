#pragma once

#include <cmath>
#include "ctk/ctk.h"
#include "nmmintrin.h" // SSE4.2

#define CTK_PI 3.141592f

////////////////////////////////////////////////////////////
/// Data
////////////////////////////////////////////////////////////
template<typename Type>
struct CTK_Vector4 {
    Type x;
    Type y;
    Type z;
    Type w;
    Type operator[](u32 i);

    template<typename RType>
    CTK_Vector4<Type> &operator+=(CTK_Vector4<RType> const &r);
};

template<typename Type>
struct CTK_Vector3 {
    Type x;
    Type y;
    Type z;
    Type operator[](u32 i);

    template<typename RType>
    CTK_Vector3<Type> &operator+=(CTK_Vector3<RType> const &r);

    template<typename RType>
    CTK_Vector3<Type> &operator*=(RType r);
};

template<typename Type>
struct CTK_Vector2 {
    Type x;
    Type y;
    Type operator[](u32 i);

    template<typename RType>
    CTK_Vector2<Type> &operator+=(CTK_Vector2<RType> const &r);
};

struct CTK_Matrix {
    union {
        f32 data[16];
        CTK_Vector4<f32> cols[4];
    };
    f32 *operator[](u32 row);
};

enum {
    CTK_AXIS_X,
    CTK_AXIS_Y,
    CTK_AXIS_Z,
};

////////////////////////////////////////////////////////////
/// General
////////////////////////////////////////////////////////////
template<typename Type>
static Type ctk_max(Type a, Type b) {
    return a > b ? a : b;
}

template<typename Type>
static Type ctk_min(Type a, Type b) {
    return a < b ? a : b;
}

template<typename Type>
static Type ctk_clamp(Type val, Type min, Type max) {
    return ctk_min(ctk_max(val, min), max);
}

template<typename Type>
static Type ctk_random_range(Type min, Type max) {
    return (max <= min) ? min : ((rand() % (max - min)) + min);
}

static inline u32 ctk_total_chunk_size(u32 min_size, u32 chunk_size) {
    return chunk_size ? (((min_size - 1) / chunk_size) + 1) * chunk_size : min_size;
}

static f32 ctk_rads(f32 degs) {
    return 2 * CTK_PI * (degs / 360);
}

////////////////////////////////////////////////////////////
/// Intrinsics
////////////////////////////////////////////////////////////
static __m128 operator*(__m128 l, __m128 r) {
    return _mm_mul_ps(l, r);
}

static __m128 operator+(__m128 l, __m128 r) {
    return _mm_add_ps(l, r);
}

////////////////////////////////////////////////////////////
/// Vector4
////////////////////////////////////////////////////////////
template<typename Type>
Type CTK_Vector4<Type>::operator[](u32 i) {
    CTK_ASSERT(i < 4);
    return *(&this->x + i);
}

template<typename Type>
template<typename RType>
CTK_Vector4<Type> &CTK_Vector4<Type>::operator+=(CTK_Vector4<RType> const &r) {
    this->x += r.x;
    this->y += r.y;
    this->z += r.z;
    this->w += r.w;
    return *this;
}

template<typename LType, typename RType>
static CTK_Vector4<LType> operator*(CTK_Vector4<LType> const &l, RType r) {
    return {
        l.x * r,
        l.y * r,
        l.z * r,
        l.w * r,
    };
}

template<typename LType, typename RType>
static CTK_Vector4<LType> operator+(CTK_Vector4<LType> const &l, CTK_Vector4<RType> const &r) {
    return {
        l.x + r.x,
        l.y + r.y,
        l.z + r.z,
        l.w + r.w,
    };
}

////////////////////////////////////////////////////////////
/// Vector3
////////////////////////////////////////////////////////////
template<typename Type>
Type CTK_Vector3<Type>::operator[](u32 i) {
    CTK_ASSERT(i < 3);
    return *(&this->x + i);
}

template<typename Type>
template<typename RType>
CTK_Vector3<Type> &CTK_Vector3<Type>::operator+=(CTK_Vector3<RType> const &r) {
    this->x += r.x;
    this->y += r.y;
    this->z += r.z;
    return *this;
}

template<typename Type>
template<typename RType>
CTK_Vector3<Type> &CTK_Vector3<Type>::operator*=(RType r) {
    this->x *= r;
    this->y *= r;
    this->z *= r;
    return *this;
}

template<typename LType, typename RType>
static CTK_Vector3<LType> operator+(CTK_Vector3<LType> const &l, CTK_Vector3<RType> const &r) {
    return {
        l.x + r.x,
        l.y + r.y,
        l.z + r.z,
    };
}

template<typename LType, typename RType>
static CTK_Vector3<LType> operator-(CTK_Vector3<LType> const &l, CTK_Vector3<RType> const &r) {
    return {
        l.x - r.x,
        l.y - r.y,
        l.z - r.z,
    };
}

template<typename LType, typename RType>
static CTK_Vector3<LType> operator*(CTK_Vector3<LType> const &l, RType r) {
    return {
        l.x * r,
        l.y * r,
        l.z * r,
    };
}

template<typename LType, typename RType>
static CTK_Vector3<LType> operator*(CTK_Vector3<LType> const &l, CTK_Vector3<RType> const &r) {
    return {
        l.x * r.x,
        l.y * r.y,
        l.z * r.z,
    };
}

template<typename LType, typename RType>
static CTK_Vector3<LType> operator/(CTK_Vector3<LType> const &l, RType r) {
    return {
        l.x / r,
        l.y / r,
        l.z / r,
    };
}

static f32 ctk_length(CTK_Vector3<f32> v) {
    return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

static CTK_Vector3<f32> ctk_normalize(CTK_Vector3<f32> v) {
    return v / ctk_length(v);
}

////////////////////////////////////////////////////////////
/// Vector2
////////////////////////////////////////////////////////////
template<typename Type>
Type CTK_Vector2<Type>::operator[](u32 i) {
    CTK_ASSERT(i < 2);
    return *(&this->x + i);
}

template<typename Type>
template<typename RType>
CTK_Vector2<Type> &CTK_Vector2<Type>::operator+=(CTK_Vector2<RType> const &r) {
    this->x += r.x;
    this->y += r.y;
    return *this;
}

template<typename LType, typename RType>
static CTK_Vector2<LType> operator+(CTK_Vector2<LType> const &l, CTK_Vector2<RType> const &r) {
    return {
        l.x + r.x,
        l.y + r.y,
    };
}

template<typename LType, typename RType>
static CTK_Vector2<LType> operator-(CTK_Vector2<LType> const &l, CTK_Vector2<RType> const &r) {
    return {
        l.x - r.x,
        l.y - r.y,
    };
}

template<typename LType, typename RType>
static CTK_Vector2<LType> operator*(CTK_Vector2<LType> const &l, RType r) {
    return {
        l.x * r,
        l.y * r,
    };
}

template<typename LType, typename RType>
static bool operator==(CTK_Vector2<LType> const &l, CTK_Vector2<RType> const &r) {
    return l.x == r.x && l.y == r.y;
}

template<typename LType, typename RType>
static bool operator!=(CTK_Vector2<LType> const &l, CTK_Vector2<RType> const &r) {
    return l.x != r.x || l.y != r.y;
}

////////////////////////////////////////////////////////////
/// Matrix
////////////////////////////////////////////////////////////
#define _CTK_COL_ROW_LOOP(MATRIX_SIZE)\
    for (u32 col = 0; col < MATRIX_SIZE; ++col)\
    for (u32 row = 0; row < MATRIX_SIZE; ++row)

static CTK_Matrix const CTK_MATRIX_ID = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
};

static CTK_Matrix operator*(CTK_Matrix &l, CTK_Matrix &r) {
    CTK_Matrix res = {};

    for (u32 col = 0; col < 4; ++col) {
        _mm_store_ps(
            res[col],
            _mm_load_ps(l[0]) * _mm_set_ps1(r[col][0]) +
            _mm_load_ps(l[1]) * _mm_set_ps1(r[col][1]) +
            _mm_load_ps(l[2]) * _mm_set_ps1(r[col][2]) +
            _mm_load_ps(l[3]) * _mm_set_ps1(r[col][3])
        );
    }

    return res;
}

static CTK_Matrix operator+(CTK_Matrix &l, CTK_Matrix &r) {
    CTK_Matrix res = {};

    _CTK_COL_ROW_LOOP(4)
        res[col][row] = l[col][row] + r[col][row];

    return res;
}

static CTK_Matrix ctk_translate(CTK_Matrix m, CTK_Vector3<f32> trans) {
    for (u32 col = 0; col < 3; ++col) {
        _mm_store_ps(
            m[3],
            (_mm_load_ps(m[col]) * _mm_setr_ps(trans[col], trans[col], trans[col], 1.0f)) + _mm_load_ps(m[3])
        );
    }

    return m;
}

static CTK_Matrix ctk_rotate_x(CTK_Matrix m, f32 degrees) {
    f32 rads = ctk_rads(degrees);

    CTK_Matrix rot_mtx = CTK_MATRIX_ID;
    rot_mtx[1][1] = cos(rads);
    rot_mtx[1][2] = sin(rads);
    rot_mtx[2][1] = -sin(rads);
    rot_mtx[2][2] = cos(rads);

    return m * rot_mtx;
}

static CTK_Matrix ctk_rotate_y(CTK_Matrix m, f32 degrees) {
    f32 rads = ctk_rads(degrees);

    CTK_Matrix rot_mtx = CTK_MATRIX_ID;
    rot_mtx[0][0] = cos(rads);
    rot_mtx[0][2] = -sin(rads);
    rot_mtx[2][0] = sin(rads);
    rot_mtx[2][2] = cos(rads);

    return m * rot_mtx;
}

static CTK_Matrix ctk_rotate_z(CTK_Matrix m, f32 degrees) {
    f32 rads = ctk_rads(degrees);

    CTK_Matrix rot_mtx = CTK_MATRIX_ID;
    rot_mtx[0][0] = cos(rads);
    rot_mtx[0][1] = sin(rads);
    rot_mtx[1][0] = -sin(rads);
    rot_mtx[1][1] = cos(rads);

    return m * rot_mtx;
}

static CTK_Matrix ctk_rotate(CTK_Matrix m, f32 degrees, s32 axis) {
    if (axis == CTK_AXIS_X) return ctk_rotate_x(m, degrees);
    if (axis == CTK_AXIS_Y) return ctk_rotate_y(m, degrees);
    if (axis == CTK_AXIS_Z) return ctk_rotate_z(m, degrees);
}

static CTK_Matrix ctk_scale(CTK_Matrix m, CTK_Vector3<f32> v) {
    CTK_Matrix res = m;

    _CTK_COL_ROW_LOOP(3)
        res[col][row] *= v[col];

    return res;
}

f32 *CTK_Matrix::operator[](u32 row) {
    return this->data + (row * 4);
}
