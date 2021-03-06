#pragma once

namespace ctk {

////////////////////////////////////////////////////////////
/// Constants
////////////////////////////////////////////////////////////
static const f32 PI = 3.1415926f;

////////////////////////////////////////////////////////////
/// Data Structures
////////////////////////////////////////////////////////////
template<typename type>
struct vec4 {
    type X;
    type Y;
    type Z;
    type W;
};

template<typename type>
struct vec3 {
    type X;
    type Y;
    type Z;
    vec3<type> &operator +=(vec3<type> Value);
};

template<typename type>
struct vec2 {
    type X;
    type Y;
};

////////////////////////////////////////////////////////////
/// Interface
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Global Operators
////////////////////////////////////////////////////////////
template<typename type>
static vec3<type> operator +(vec3<type> Left, vec3<type> Right) {
    return {
        Left.X + Right.X,
        Left.Y + Right.Y,
        Left.Z + Right.Z,
    };
}

template<typename type>
static vec3<type> operator -(vec3<type> Left, vec3<type> Right) {
    return {
        Left.X - Right.X,
        Left.Y - Right.Y,
        Left.Z - Right.Z,
    };
}

template<typename type>
static vec2<type> operator -(vec2<type> Left, vec2<type> Right) {
    return {
        Left.X - Right.X,
        Left.Y - Right.Y,
    };
}

template<typename type>
static vec3<type> operator *(vec3<type> Left, type Right) {
    return {
        Left.X * Right,
        Left.Y * Right,
        Left.Z * Right,
    };
}

template<typename type>
static vec3<type> operator *(vec3<type> Left, vec3<type> Right) {
    return {
        Left.X * Right.X,
        Left.Y * Right.Y,
        Left.Z * Right.Z,
    };
}

template<typename type>
static vec4<type> operator *(vec4<type> Left, type Right) {
    return {
        Left.X * Right,
        Left.Y * Right,
        Left.Z * Right,
        Left.W * Right,
    };
}

template<typename type>
static vec3<type> operator /(vec3<type> Left, type Right) {
    return {
        Left.X / Right,
        Left.Y / Right,
        Left.Z / Right,
    };
}

template<typename type>
static b32 operator ==(vec2<type> Left, vec2<type> Right) {
    return Left.X == Right.X && Left.Y == Right.Y;
}

template<typename type>
static b32 operator !=(vec2<type> Left, vec2<type> Right) {
    return Left.X != Right.X || Left.Y != Right.Y;
}

////////////////////////////////////////////////////////////
/// Member Operators
////////////////////////////////////////////////////////////
template<typename type>
vec3<type> &vec3<type>::operator +=(vec3<type> Value) {
    X += Value.X;
    Y += Value.Y;
    Z += Value.Z;
    return *this;
}

////////////////////////////////////////////////////////////
/// Misc.
////////////////////////////////////////////////////////////
template<typename type_a, typename type_b>
static vec3<type_a> cast(vec3<type_b> Vec3) {
    return {
        (type_a)Vec3.X,
        (type_a)Vec3.Y,
        (type_a)Vec3.Z,
    };
}

template<typename type>
static f32 distance(vec3<type> A, vec3<type> B) {
    return sqrt(pow(B.X - A.X, 2) + pow(B.Y - A.Y, 2) + pow(B.Z - A.Z, 2));
}

template<typename type>
static type max(type A, type B) {
    return A > B ? A : B;
}

template<typename type>
static type min(type A, type B) {
    return A < B ? A : B;
}

template<typename type>
static type clamp(type Value, type MinValue, type MaxValue) {
    return min(max(Value, MinValue), MaxValue);
}

static s32 random_range(s32 Min, s32 Max) {
    return (rand() % (Max - Min)) + Min;
}

static f32 lerp(f32 Start, f32 End, f32 Value) {
    return ((1 - Value) * Start) + (Value * End);
}

} // ctk
