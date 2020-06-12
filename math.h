#pragma once

namespace ctk
{

////////////////////////////////////////////////////////////
/// Constants
////////////////////////////////////////////////////////////
static const f32 PI = 3.1415926f;

////////////////////////////////////////////////////////////
/// Data Structures
////////////////////////////////////////////////////////////
template<typename type>
struct vec4
{
    type X;
    type Y;
    type Z;
    type W;
};

template<typename type>
struct vec3
{
    type X;
    type Y;
    type Z;
};

template<typename type>
struct vec2
{
    type X;
    type Y;
};

////////////////////////////////////////////////////////////
/// Interface
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Operators
////////////////////////////////////////////////////////////
template<typename type>
static vec3<type>
operator+(vec3<type> Left, vec3<type> Right)
{
    return
    {
        Left.X + Right.X,
        Left.Y + Right.Y,
        Left.Z + Right.Z,
    };
}

template<typename type>
static vec3<type>
operator-(vec3<type> Left, vec3<type> Right)
{
    return
    {
        Left.X - Right.X,
        Left.Y - Right.Y,
        Left.Z - Right.Z,
    };
}

template<typename type>
static vec2<type>
operator-(vec2<type> Left, vec2<type> Right)
{
    return
    {
        Left.X - Right.X,
        Left.Y - Right.Y,
    };
}

template<typename type>
static vec3<type>
operator*(vec3<type> Left, type Right)
{
    return
    {
        Left.X * Right,
        Left.Y * Right,
        Left.Z * Right,
    };
}

template<typename type>
static vec3<type>
operator*(vec3<type> Left, vec3<type> Right)
{
    return
    {
        Left.X * Right.X,
        Left.Y * Right.Y,
        Left.Z * Right.Z,
    };
}

template<typename type>
static vec4<type>
operator*(vec4<type> Left, type Right)
{
    return
    {
        Left.X * Right,
        Left.Y * Right,
        Left.Z * Right,
        Left.W * Right,
    };
}

template<typename type>
static vec3<type>
operator/(vec3<type> Left, type Right)
{
    return
    {
        Left.X / Right,
        Left.Y / Right,
        Left.Z / Right,
    };
}

template<typename type>
static b32
operator==(vec2<type> Left, vec2<type> Right)
{
    return Left.X == Right.X && Left.Y == Right.Y;
}

template<typename type>
static b32
operator!=(vec2<type> Left, vec2<type> Right)
{
    return Left.X != Right.X || Left.Y != Right.Y;
}

////////////////////////////////////////////////////////////
/// Misc.
////////////////////////////////////////////////////////////
template<typename type_a, typename type_b>
static vec3<type_a>
Cast(vec3<type_b> Vec3)
{
    return
    {
        (type_a)Vec3.X,
        (type_a)Vec3.Y,
        (type_a)Vec3.Z,
    };
}

template<typename type>
static f32
Distance(vec3<type> A, vec3<type> B)
{
    return sqrt(pow(B.X - A.X, 2) + pow(B.Y - A.Y, 2) + pow(B.Z - A.Z, 2));
}

template<typename type>
static type
Max(type A, type B)
{
    return A > B ? A : B;
}

template<typename type>
static type
Min(type A, type B)
{
    return A < B ? A : B;
}

template<typename type>
static type
Clamp(type Value, type Min, type Max)
{
    return Min(Max(Value, Min), Max);
}

static s32
RandomRange(s32 Min, s32 Max)
{
    return (rand() % (Max - Min)) + Min;
}

static f32
Lerp(f32 Start, f32 End, f32 Value)
{
    return ((1 - Value) * Start) + (Value * End);
}

////////////////////////////////////////////////////////////
/// Debugging
////////////////////////////////////////////////////////////
// static void
// PrintFloat(f32 Value, u32 Precision)
// {
//     CTK_ASSERT(Precision <= 6);
//     static const cstr FORMAT_STRINGS[] { "%.0f", "%.1f", "%.2f", "%.3f", "%.4f", "%.5f", "%.6f" };
//     if(Value >= 0)
//     {
//         Print(" ");
//         if(Value == 0)
//         {
//             Value = 0; // Ensure negative bit is 0.
//         }
//     }
//     Print(FORMAT_STRINGS[Precision], Value);
// }

// static void
// PrintMat4(glm::mat4 * matrix, u32 Precision, u32 tab_count = 0)
// {
//     for(u32 Y = 0; Y < 4; Y++)
//     {
//         Print(tab_count, "[ ");
//         for(u32 X = 0; X < 4; X++)
//         {
//             PrintFloat((*matrix)[X][Y], Precision);
//             Print(" ");
//         }
//         PrintLine(" ]");
//     }
// }

} // ctk
