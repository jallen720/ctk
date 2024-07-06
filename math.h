/// Data
////////////////////////////////////////////////////////////
template<typename Type>
struct Vec4
{
    struct { Type x, y, z, w; };
    Type array[4];
};

template<typename Type>
struct Vec3
{
    struct { Type x, y, z; };
    Type array[3];
};

template<typename Type>
struct Vec2
{
    struct { Type x, y; };
    Type array[2];
};

struct alignas(16) Matrix
{
    float32 data[16];
};

/// Constants
////////////////////////////////////////////////////////////
constexpr float32 PI = 3.141592f;

constexpr Matrix ID_MATRIX =
{
    .data =
    {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    }
};

/// Debugging
////////////////////////////////////////////////////////////
float32 Get(const Matrix* m, uint32 col, uint32 row);

void PrintMatrix(Matrix* m)
{
    PrintLine();
    for (uint32 row = 0; row < 4; ++row)
    {
        for (uint32 col = 0; col < 4; ++col)
        {
            float32 val = Get(m, col, row);

            // Prevent -0.00 from sign bit still being set.
            if (signbit(val) && fabs(val) < 0.000001f)
            {
                val *= -1;
            }

            Print("%s%.2f ", val < 0 ? "" : " ", val);
        }

        PrintLine();
    }
}

/// General
////////////////////////////////////////////////////////////
#define CTK_MINMAX_FUNCS(TYPE) \
TYPE Min(TYPE a, TYPE b) { return a < b ? a : b; } \
TYPE Max(TYPE a, TYPE b) { return a > b ? a : b; }
CTK_MINMAX_FUNCS(uint8)
CTK_MINMAX_FUNCS(uint16)
CTK_MINMAX_FUNCS(uint32)
CTK_MINMAX_FUNCS(uint64)
CTK_MINMAX_FUNCS(sint8)
CTK_MINMAX_FUNCS(sint16)
CTK_MINMAX_FUNCS(sint32)
CTK_MINMAX_FUNCS(sint64)
CTK_MINMAX_FUNCS(float32)
CTK_MINMAX_FUNCS(float64)

#define CTK_CLAMP_FUNC(TYPE) \
TYPE Clamp(TYPE val, TYPE min_val, TYPE max_val) { return Min(Max(val, min_val), max_val); }
CTK_CLAMP_FUNC(uint8)
CTK_CLAMP_FUNC(uint16)
CTK_CLAMP_FUNC(uint32)
CTK_CLAMP_FUNC(uint64)
CTK_CLAMP_FUNC(sint8)
CTK_CLAMP_FUNC(sint16)
CTK_CLAMP_FUNC(sint32)
CTK_CLAMP_FUNC(sint64)
CTK_CLAMP_FUNC(float32)
CTK_CLAMP_FUNC(float64)

float32 Log2(float32 val)
{
    return log2f(val);
}

float64 Log2(float64 val)
{
    return log2(val);
}

#define CTK_ALIGN_FUNC(TYPE) \
TYPE Align(TYPE val, TYPE alignment) { return ((val + (alignment - 1)) / alignment) * alignment; }
CTK_ALIGN_FUNC(uint8)
CTK_ALIGN_FUNC(uint16)
CTK_ALIGN_FUNC(uint32)
CTK_ALIGN_FUNC(uint64)
CTK_ALIGN_FUNC(sint8)
CTK_ALIGN_FUNC(sint16)
CTK_ALIGN_FUNC(sint32)
CTK_ALIGN_FUNC(sint64)

#define CTK_CTIME_ALIGN_FUNC(TYPE) \
constexpr TYPE CTime_Align(TYPE val, TYPE alignment) \
{ \
    return ((val + (alignment - 1)) / alignment) * alignment; \
}
CTK_CTIME_ALIGN_FUNC(uint8)
CTK_CTIME_ALIGN_FUNC(uint16)
CTK_CTIME_ALIGN_FUNC(uint32)
CTK_CTIME_ALIGN_FUNC(uint64)
CTK_CTIME_ALIGN_FUNC(sint8)
CTK_CTIME_ALIGN_FUNC(sint16)
CTK_CTIME_ALIGN_FUNC(sint32)
CTK_CTIME_ALIGN_FUNC(sint64)

template<typename Type>
Type* Align(Type* ptr, uint64 alignment)
{
    return (Type*)Align((uint64)ptr, alignment);
}

uint64 GetAlignment(uint64 value)
{
    for (uint32 i = 0; i < 64; ++i)
    {
        uint64 alignment = 1 << i;
        if (alignment & value)
        {
            return alignment;
        }
    }
    return 0;
}

uint64 GetAlignment(void* address)
{
    return GetAlignment((uint64)address);
}

float32 ToRadians(float32 degrees)
{
    return 2 * PI * (degrees / 360);
}

#define CTK_DISTANCE_2D_FUNC_32(TYPE) \
float32 Distance2D(TYPE x0, TYPE y0, TYPE x1, TYPE y1) \
{ \
    TYPE a = x1 - x0; \
    TYPE b = y1 - y0; \
    return sqrtf((a * a) + (b * b)); \
}
CTK_DISTANCE_2D_FUNC_32(sint8)
CTK_DISTANCE_2D_FUNC_32(sint16)
CTK_DISTANCE_2D_FUNC_32(sint32)
CTK_DISTANCE_2D_FUNC_32(float32)

#define CTK_DISTANCE_2D_FUNC_64(TYPE) \
float64 Distance2D(TYPE x0, TYPE y0, TYPE x1, TYPE y1) \
{ \
    TYPE a = x1 - x0; \
    TYPE b = y1 - y0; \
    return sqrt((a * a) + (b * b)); \
}
CTK_DISTANCE_2D_FUNC_64(sint64)
CTK_DISTANCE_2D_FUNC_64(float64)

#define CTK_ABSVAL_FUNC_SIGNED(TYPE) TYPE AbsVal(TYPE x) { return x < 0 ? -x : x; }
CTK_ABSVAL_FUNC_SIGNED(sint8)
CTK_ABSVAL_FUNC_SIGNED(sint16)
CTK_ABSVAL_FUNC_SIGNED(sint32)
CTK_ABSVAL_FUNC_SIGNED(sint64)
CTK_ABSVAL_FUNC_SIGNED(float32)
CTK_ABSVAL_FUNC_SIGNED(float64)

#define CTK_ABSVAL_FUNC_UNSIGNED(TYPE) TYPE AbsVal(TYPE x) { return x; }
CTK_ABSVAL_FUNC_UNSIGNED(uint8)
CTK_ABSVAL_FUNC_UNSIGNED(uint16)
CTK_ABSVAL_FUNC_UNSIGNED(uint32)
CTK_ABSVAL_FUNC_UNSIGNED(uint64)

void RandomSeed(uint32 seed = 0)
{
    srand(seed ? seed : _time32(NULL));
}

sint32 RandomRange(sint32 min_val, sint32 max_val)
{
    CTK_ASSERT(max_val >= min_val);

    return (rand() % (max_val - min_val)) + min_val;
}

uint32 RandomRange(uint32 min_val, uint32 max_val)
{
    CTK_ASSERT(max_val >= min_val);

    return ((uint32)rand() % (max_val - min_val)) + min_val;
}

float32 RandomRange(float32 min_val, float32 max_val)
{
    CTK_ASSERT(max_val >= min_val);

    return ((float32)rand() / (float32)(RAND_MAX)) * (max_val - min_val) + min_val;
}

/// Vec4
////////////////////////////////////////////////////////////
template<typename Type>
Type Get(Vec4<Type>* vec, uint32 index)
{
    CTK_ASSERT(index < 4);

    return (&vec->x)[index];
}

template<typename Type>
Type Set(Vec4<Type>* vec, uint32 index, Type value)
{
    CTK_ASSERT(index < 4);

    return (&vec->x)[index] = value;
}

template<typename Type>
Vec4<Type> operator+(Vec4<Type> a, Vec4<Type> b)
{
    Vec4<Type> vec4 = {};
    vec4.x = a.x + b.x;
    vec4.y = a.y + b.y;
    vec4.z = a.z + b.z;
    vec4.w = a.w + b.w;
    return vec4;
}

template<typename Type>
Vec4<Type> operator-(Vec4<Type> a, Vec4<Type> b)
{
    Vec4<Type> vec4 = {};
    vec4.x = a.x - b.x;
    vec4.y = a.y - b.y;
    vec4.z = a.z - b.z;
    vec4.w = a.w - b.w;
    return vec4;
}

template<typename Type>
Vec4<Type> operator*(Vec4<Type> a, Vec4<Type> b)
{
    Vec4<Type> vec4 = {};
    vec4.x = a.x * b.x;
    vec4.y = a.y * b.y;
    vec4.z = a.z * b.z;
    vec4.w = a.w * b.w;
    return vec4;
}

template<typename Type>
Vec4<Type> operator/(Vec4<Type> a, Vec4<Type> b)
{
    Vec4<Type> vec4 = {};
    vec4.x = a.x / b.x;
    vec4.y = a.y / b.y;
    vec4.z = a.z / b.z;
    vec4.w = a.w / b.w;
    return vec4;
}

template<typename Type>
Vec4<Type> operator+(Vec4<Type> vec, Type scalar)
{
    Vec4<Type> vec4 = {};
    vec4.x = vec.x + scalar;
    vec4.y = vec.y + scalar;
    vec4.z = vec.z + scalar;
    vec4.w = vec.w + scalar;
    return vec4;
}

template<typename Type>
Vec4<Type> operator-(Vec4<Type> vec, Type scalar)
{
    Vec4<Type> vec4 = {};
    vec4.x = vec.x - scalar;
    vec4.y = vec.y - scalar;
    vec4.z = vec.z - scalar;
    vec4.w = vec.w - scalar;
    return vec4;
}

template<typename Type>
Vec4<Type> operator*(Vec4<Type> vec, Type scalar)
{
    Vec4<Type> vec4 = {};
    vec4.x = vec.x * scalar;
    vec4.y = vec.y * scalar;
    vec4.z = vec.z * scalar;
    vec4.w = vec.w * scalar;
    return vec4;
}

template<typename Type>
Vec4<Type> operator/(Vec4<Type> vec, Type scalar)
{
    Vec4<Type> vec4 = {};
    vec4.x = vec.x / scalar;
    vec4.y = vec.y / scalar;
    vec4.z = vec.z / scalar;
    vec4.w = vec.w / scalar;
    return vec4;
}

template<typename Type>
Type Length(Vec4<Type> vec)
{
    return sqrt((vec.x * vec.x) +
                (vec.y * vec.y) +
                (vec.z * vec.z) +
                (vec.w * vec.w));
}

template<typename Type>
Type Dot(Vec4<Type> a, Vec4<Type> b)
{
    return (a.x * b.x) +
           (a.y * b.y) +
           (a.z * b.z) +
           (a.w * b.w);
}

template<typename Type>
Vec4<Type> Normalize(Vec4<Type> vec)
{
    return vec / Length(vec);
}

/// Vec3
////////////////////////////////////////////////////////////
template<typename Type>
Type Get(Vec3<Type>* vec, uint32 index)
{
    CTK_ASSERT(index < 3);

    return (&vec->x)[index];
}

template<typename Type>
void Set(Vec3<Type>* vec, uint32 index, Type value)
{
    CTK_ASSERT(index < 3);

    (&vec->x)[index] = value;
}

template<typename Type>
Vec3<Type> operator+(Vec3<Type> a, Vec3<Type> b)
{
    Vec3<Type> vec3 = {};
    vec3.x = a.x + b.x;
    vec3.y = a.y + b.y;
    vec3.z = a.z + b.z;
    return vec3;
}

template<typename Type>
Vec3<Type> operator-(Vec3<Type> a, Vec3<Type> b)
{
    Vec3<Type> vec3 = {};
    vec3.x = a.x - b.x;
    vec3.y = a.y - b.y;
    vec3.z = a.z - b.z;
    return vec3;
}

template<typename Type>
Vec3<Type> operator*(Vec3<Type> a, Vec3<Type> b)
{
    Vec3<Type> vec3 = {};
    vec3.x = a.x * b.x;
    vec3.y = a.y * b.y;
    vec3.z = a.z * b.z;
    return vec3;
}

template<typename Type>
Vec3<Type> operator/(Vec3<Type> a, Vec3<Type> b)
{
    Vec3<Type> vec3 = {};
    vec3.x = a.x / b.x;
    vec3.y = a.y / b.y;
    vec3.z = a.z / b.z;
    return vec3;
}

template<typename Type>
Vec3<Type> operator+(Vec3<Type> vec, Type scalar)
{
    Vec3<Type> vec3 = {};
    vec3.x = vec.x + scalar;
    vec3.y = vec.y + scalar;
    vec3.z = vec.z + scalar;
    return vec3;
}

template<typename Type>
Vec3<Type> operator-(Vec3<Type> vec, Type scalar)
{
    Vec3<Type> vec3 = {};
    vec3.x = vec.x - scalar;
    vec3.y = vec.y - scalar;
    vec3.z = vec.z - scalar;
    return vec3;
}

template<typename Type>
Vec3<Type> operator*(Vec3<Type> vec, Type scalar)
{
    Vec3<Type> vec3 = {};
    vec3.x = vec.x * scalar;
    vec3.y = vec.y * scalar;
    vec3.z = vec.z * scalar;
    return vec3;
}

template<typename Type>
Vec3<Type> operator/(Vec3<Type> vec, Type scalar)
{
    Vec3<Type> vec3 = {};
    vec3.x = vec.x / scalar;
    vec3.y = vec.y / scalar;
    vec3.z = vec.z / scalar;
    return vec3;
}

template<typename Type>
Type Length(Vec3<Type> vec)
{
    return sqrtf((vec.x * vec.x) +
                 (vec.y * vec.y) +
                 (vec.z * vec.z));
}

template<typename Type>
Type Dot(Vec3<Type> a, Vec3<Type> b)
{
    return (a.x * b.x) +
           (a.y * b.y) +
           (a.z * b.z);
}

template<typename Type>
Vec3<Type> Normalize(Vec3<Type> vec)
{
    return vec / Length(vec);
}

template<typename Type>
Vec3<Type> Cross(Vec3<Type> a, Vec3<Type> b)
{
    Vec3<Type> vec3 = {};
    vec3.x = (a.y * b.z) - (b.y * a.z);
    vec3.y = (a.z * b.x) - (b.z * a.x);
    vec3.z = (a.x * b.y) - (b.x * a.y);
    return vec3;
}

/// Vec2
////////////////////////////////////////////////////////////
template<typename Type>
Type Get(Vec2<Type>* vec, uint32 index)
{
    CTK_ASSERT(index < 2);

    return (&vec->x)[index];
}

template<typename Type>
void Set(Vec2<Type>* vec, uint32 index, Type value)
{
    CTK_ASSERT(index < 2);

    (&vec->x)[index] = value;
}

template<typename Type>
Vec2<Type> operator+(Vec2<Type> a, Vec2<Type> b)
{
    Vec2<Type> vec2 = {};
    vec2.x = a.x + b.x;
    vec2.y = a.y + b.y;
    return vec2;
}

template<typename Type>
Vec2<Type> operator-(Vec2<Type> a, Vec2<Type> b)
{
    Vec2<Type> vec2 = {};
    vec2.x = a.x - b.x;
    vec2.y = a.y - b.y;
    return vec2;
}

template<typename Type>
Vec2<Type> operator*(Vec2<Type> a, Vec2<Type> b)
{
    Vec2<Type> vec2 = {};
    vec2.x = a.x * b.x;
    vec2.y = a.y * b.y;
    return vec2;
}

template<typename Type>
Vec2<Type> operator/(Vec2<Type> a, Vec2<Type> b)
{
    Vec2<Type> vec2 = {};
    vec2.x = a.x / b.x;
    vec2.y = a.y / b.y;
    return vec2;
}

template<typename Type>
Vec2<Type> operator+(Vec2<Type> vec, Type scalar)
{
    Vec2<Type> vec2 = {};
    vec2.x = vec.x + scalar;
    vec2.y = vec.y + scalar;
    return vec2;
}

template<typename Type>
Vec2<Type> operator-(Vec2<Type> vec, Type scalar)
{
    Vec2<Type> vec2 = {};
    vec2.x = vec.x - scalar;
    vec2.y = vec.y - scalar;
    return vec2;
}

template<typename Type>
Vec2<Type> operator*(Vec2<Type> vec, Type scalar)
{
    Vec2<Type> vec2 = {};
    vec2.x = vec.x * scalar;
    vec2.y = vec.y * scalar;
    return vec2;
}

template<typename Type>
Vec2<Type> operator/(Vec2<Type> vec, Type scalar)
{
    Vec2<Type> vec2 = {};
    vec2.x = vec.x / scalar;
    vec2.y = vec.y / scalar;
    return vec2;
}

template<typename Type>
Type Length(Vec2<Type> vec)
{
    return sqrt((vec.x * vec.x) +
                (vec.y * vec.y));
}

template<typename Type>
Type Dot(Vec2<Type> a, Vec2<Type> b)
{
    return (a.x * b.x) +
           (a.y * b.y);
}

template<typename Type>
Vec2<Type> Normalize(Vec2<Type> vec)
{
    return vec / Length(vec);
}

/// Matrix
////////////////////////////////////////////////////////////
const float32* GetCol(const Matrix* m, uint32 col)
{
    return &m->data[col * 4];
}

float32* GetCol(Matrix* m, uint32 col)
{
    return &m->data[col * 4];
}

float32 Get(const Matrix* m, uint32 col, uint32 row)
{
    CTK_ASSERT(col < 4);
    CTK_ASSERT(row < 4);

    return GetCol(m, col)[row];
}

void Set(Matrix* m, uint32 col, uint32 row, float32 value)
{
    CTK_ASSERT(col < 4);
    CTK_ASSERT(row < 4);

    GetCol(m, col)[row] = value;
}

Matrix operator*(const Matrix& l, const Matrix& r)
{
    Matrix res = {};

    for (uint32 col = 0; col < 4; ++col)
    {
        _mm_store_ps
        (
            GetCol(&res, col),
            _mm_add_ps
            (
                _mm_mul_ps(_mm_load_ps(GetCol(&l, 0)), _mm_set_ps1(Get(&r, col, 0))),
                _mm_add_ps
                (
                    _mm_mul_ps(_mm_load_ps(GetCol(&l, 1)), _mm_set_ps1(Get(&r, col, 1))),
                    _mm_add_ps
                    (
                        _mm_mul_ps(_mm_load_ps(GetCol(&l, 2)), _mm_set_ps1(Get(&r, col, 2))),
                        _mm_mul_ps(_mm_load_ps(GetCol(&l, 3)), _mm_set_ps1(Get(&r, col, 3)))
                    )
                )
            )
        );
    }

    return res;
}

Matrix operator+(const Matrix& l, const Matrix& r)
{
    Matrix res = {};

    for (uint32 col = 0; col < 4; ++col)
    {
        _mm_store_ps(GetCol(&res, col), _mm_add_ps(_mm_load_ps(GetCol(&l, col)), _mm_load_ps(GetCol(&r, col))));
    }

    return res;
}

Matrix Translate(Matrix m, Vec3<float32> trans)
{
    for (uint32 col = 0; col < 3; ++col)
    {
        float32 trans_val = Get(&trans, col);
        _mm_store_ps
        (
            GetCol(&m, 3),
            _mm_add_ps
            (
                _mm_mul_ps(_mm_load_ps(GetCol(&m, col)), _mm_setr_ps(trans_val, trans_val, trans_val, 1.0f)),
                _mm_load_ps(GetCol(&m, 3))
            )
        );
    }

    return m;
}

Matrix RotateX(Matrix m, float32 degrees)
{
    float32 rads = ToRadians(degrees);

    Matrix rot_mtx = ID_MATRIX;
    Set(&rot_mtx, 1, 1, cosf(rads));
    Set(&rot_mtx, 1, 2, sinf(rads));
    Set(&rot_mtx, 2, 1, -sinf(rads));
    Set(&rot_mtx, 2, 2, cosf(rads));

    return m * rot_mtx;
}

Matrix RotateY(Matrix m, float32 degrees)
{
    float32 rads = ToRadians(degrees);

    Matrix rot_mtx = ID_MATRIX;
    Set(&rot_mtx, 0, 0, cosf(rads));
    Set(&rot_mtx, 0, 2, -sinf(rads));
    Set(&rot_mtx, 2, 0, sinf(rads));
    Set(&rot_mtx, 2, 2, cosf(rads));

    return m * rot_mtx;
}

Matrix RotateZ(Matrix m, float32 degrees)
{
    float32 rads = ToRadians(degrees);

    Matrix rot_mtx = ID_MATRIX;
    Set(&rot_mtx, 0, 0, cosf(rads));
    Set(&rot_mtx, 0, 1, sinf(rads));
    Set(&rot_mtx, 1, 0, -sinf(rads));
    Set(&rot_mtx, 1, 1, cosf(rads));

    return m * rot_mtx;
}

Matrix Scale(Matrix m, Vec3<float32> v)
{
    for (uint32 col = 0; col < 3; ++col)
    for (uint32 row = 0; row < 3; ++row)
    {
        Set(&m, col, row, Get(&m, col, row) * Get(&v, col));
    }

    return m;
}

Matrix GetPerspectiveMatrix(float32 vertical_fov, float32 aspect, float32 z_near, float32 z_far)
{
    Matrix res = {}; // Must be 0 matrix.

    // Ripped off from GLM.
    float32 half_tan_vertical_fov = tanf(ToRadians(vertical_fov) / 2.0f);
    Set(&res, 0, 0, 1.0f / (aspect * half_tan_vertical_fov));
    Set(&res, 1, 1, 1.0f / (half_tan_vertical_fov));
    Set(&res, 2, 2, z_far / (z_near - z_far));
    Set(&res, 2, 3, -1.0f);
    Set(&res, 3, 2, -(z_far * z_near) / (z_far - z_near));

    return res;
}

Matrix GetOrthographicMatrix()
{
    return ID_MATRIX;
}

Matrix LookAt(Vec3<float32> eye, Vec3<float32> center, Vec3<float32> up)
{
    Matrix res = ID_MATRIX;

    // Ripped off from GLM.
    Vec3 f = Normalize(center - eye);
    Vec3 s = Normalize(Cross(f, up));
    Vec3 u = Cross(s, f);

    Set(&res, 0, 0, s.x);
    Set(&res, 1, 0, s.y);
    Set(&res, 2, 0, s.z);
    Set(&res, 0, 1, u.x);
    Set(&res, 1, 1, u.y);
    Set(&res, 2, 1, u.z);
    Set(&res, 0, 2, -f.x);
    Set(&res, 1, 2, -f.y);
    Set(&res, 2, 2, -f.z);
    Set(&res, 3, 0, -Dot(s, eye));
    Set(&res, 3, 1, -Dot(u, eye));
    Set(&res, 3, 2, Dot(f, eye));

    return res;
}

