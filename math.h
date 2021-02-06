#pragma once

#include <cmath>
#include "ctk/ctk.h"

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
    f32 data[16];
    f32 *operator[](u32 row);
};

////////////////////////////////////////////////////////////
/// Vector4
////////////////////////////////////////////////////////////
template<typename Type>
Type CTK_Vector4<Type>::operator[](u32 i) {
    CTK_ASSERT(i < 4);
    return *(&this->x + i);
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
static CTK_Matrix const CTK_MATRIX_ID = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
};

static CTK_Matrix operator*(CTK_Matrix &l, CTK_Matrix &r) {
    CTK_Matrix res = {};

    for (u32 row = 0; row < 4; ++row)
        for (u32 col = 0; col < 4; ++col)
            for (u32 i = 0; i < 4; ++i)
                res[row][col] += l[i][col] * r[row][i];

    return res;
}

static CTK_Matrix ctk_translate(CTK_Matrix m, CTK_Vector3<f32> v) {
    CTK_Matrix res = m;

    for (u32 row = 0; row < 3; ++row)
        for (u32 col = 0; col < 3; ++col)
            res[3][col] += v[row] * res[row][col];

    return res;
}

static CTK_Matrix ctk_scale(CTK_Matrix m, CTK_Vector3<f32> v) {
    CTK_Matrix res = m;

    for (u32 row = 0; row < 3; ++row)
        for (u32 col = 0; col < 3; ++col)
            res[row][col] *= v[col];

    return res;
}

static CTK_Matrix ctk_rotate(CTK_Matrix m, f32 angle, CTK_Vector3<f32> axis) {
    // CTK_Vector3<f32> norm_axis = ctk_normalize(axis);
}

f32 *CTK_Matrix::operator[](u32 row) {
    return this->data + (row * 4);
}

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
