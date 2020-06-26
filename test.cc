#include <cstdlib>
#include <ctime>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ctk/ctk.h"
#include "ctk/data.h"
#include "ctk/math.h"

#define MIL 1000000
#define BIL MIL * 1000
#define DATA_SIZE MIL

static void
PrintF32(f32 value, u32 precision)
{
    CTK_ASSERT(precision <= 6);
    static const cstr FORMAT_STRINGS[] { "%.0f", "%.1f", "%.2f", "%.3f", "%.4f", "%.5f", "%.6f" };
    if(value >= 0)
    {
        ctk::Print(" ");
        if(value == 0.0f)
        {
            value = abs(0); // Ensure negative bit is 0.
        }
    }
    ctk::Print(FORMAT_STRINGS[precision], value);
}

static void
PrintMat4(glm::mat4 *Matrix, u32 Precision, u32 TabCount = 0)
{
    Precision = 6;
    for(u32 Row = 0; Row < 4; ++Row)
    {
        ctk::PrintTabs(TabCount);
        if (Row == 0)     ctk::Print("R");
        else if(Row == 1) ctk::Print("U");
        else if(Row == 2) ctk::Print("F");
        else              ctk::Print(" ");
        ctk::Print(" [ ");
        for(u32 Column = 0; Column < 4; ++Column)
        {
            PrintF32((*Matrix)[Column][Row], Precision);
            ctk::Print(" ");
        }
        ctk::PrintLine(" ]");
    }
    ctk::PrintLine();
}

using mat4 = f32[16];

static void
PerformanceTest()
{
    glm::mat4 *Matrixes = ctk::Alloc<glm::mat4>(DATA_SIZE);
    clock_t Start = clock();
    glm::vec3 Position = { 1, 2, 3 };
    glm::vec3 Rotation = { 4, 5, 6 };
    glm::vec3 Scale = { 1, 1, 1 };
    for(u32 MatrixIndex = 0; MatrixIndex < DATA_SIZE; ++MatrixIndex)
    {
        glm::mat4 Matrix(1.0f);
        Matrix = glm::translate(Matrix, { Position.x, Position.y, Position.z });
        Matrix = glm::rotate(Matrix, glm::radians(Rotation.x), { 1.0f, 0.0f, 0.0f });
        Matrix = glm::rotate(Matrix, glm::radians(Rotation.y), { 0.0f, 1.0f, 0.0f });
        Matrix = glm::rotate(Matrix, glm::radians(Rotation.z), { 0.0f, 0.0f, 1.0f });
        Matrix = glm::scale(Matrix, { Scale.x, Scale.y, Scale.z });
        Matrixes[MatrixIndex] = Matrix;
        ++Position.x;
        --Rotation.y;
        ++Scale.z;
    }
    clock_t Elapsed = clock() - Start;
    double ElapsedMS = ((double)Elapsed) / (double)CLOCKS_PER_SEC * 1000.0; // in seconds
    ctk::PrintLine("elapsed ms: %f", ElapsedMS);
    free(Matrixes);
}

enum AxesBits
{
    AXES_X = (1 << 0),
    AXES_Y = (1 << 1),
    AXES_Z = (1 << 2),
};

static f32
Radians(f32 Degrees)
{
    return Degrees * (ctk::PI / 180);
}

using mat4 = f32[16];

static void
Rotate(mat4 *Matrix, f32 Degrees, u32 AxesFlags)
{
    ctk::vec3<f32> Axes = {};
    if(AxesFlags & AXES_X) Axes.X = 1;
    if(AxesFlags & AXES_Y) Axes.Y = 1;
    if(AxesFlags & AXES_Z) Axes.Z = 1;
    f32 Radians = ::Radians(Degrees);

}

static void
MatrixTest()
{
    glm::mat4 Matrix(1.0f);
    ctk::PrintLine("base:");
    PrintMat4(&Matrix, 1, 1);

    glm::vec3 Translation = { 1, 2, 3 };
    glm::mat4 TranslationMatrix = glm::translate(Matrix, Translation);
    ctk::PrintLine("TranslationMatrix (%f, %f, %f):", Translation.x, Translation.y, Translation.z);
    PrintMat4(&TranslationMatrix, 1, 1);

    glm::vec3 RotationAxesVec = { 0, 1, 0 };
    f32 RotationDegrees = 45.0f;
    glm::mat4 RotationMatrix = glm::rotate(Matrix, glm::radians(RotationDegrees), RotationAxesVec);
    ctk::PrintLine("RotationMatrix: degrees: %f axes: (%f, %f, %f):",
                   RotationDegrees, RotationAxesVec.x, RotationAxesVec.y, RotationAxesVec.z);
    PrintMat4(&RotationMatrix, 1, 1);

    glm::mat4 RotationMatrix2 = glm::rotate(RotationMatrix, glm::radians(RotationDegrees), RotationAxesVec);
    ctk::PrintLine("RotationMatrix2: degrees: %f axes: (%f, %f, %f):",
                   RotationDegrees, RotationAxesVec.x, RotationAxesVec.y, RotationAxesVec.z);
    PrintMat4(&RotationMatrix2, 1, 1);

    // u32 CustomRotationAxes = AXES_Y;
    // f32 CustomRotationDegrees = 45.0f;
    // glm::mat4 CustomRotationMatrix = Rotate(&Matrix, CustomRotationDegrees, CustomRotationAxes);
    // ctk::PrintLine("CustomRotationMatrix: degrees: %f axes: %u:",
    //                CustomRotationDegrees, CustomRotationAxes);
    // PrintMat4(&CustomRotationMatrix, 1, 1);
}

template<typename type, u32 size>
static void
PrintStaticArray(ctk::sarray<type, size> *StaticArray)
{
    ctk::Print("static array (size=%u count=%u): ", StaticArray->Size, StaticArray->Count);
    for(u32 Index = 0; Index < StaticArray->Count; ++Index)
    {
        ctk::Print("%u ", *At(StaticArray, Index));
    }
    ctk::PrintLine();
}

static void
StaticArrayTest()
{
    ctk::sarray<u32, 8> StaticArray = {};
    PrintStaticArray(&StaticArray);
    ctk::Push(&StaticArray, 1u);
    PrintStaticArray(&StaticArray);
    ctk::Push(&StaticArray);
    PrintStaticArray(&StaticArray);
}

static void
CopyArrayTest()
{
    auto CopyArray = ctk::CreateArrayEmpty<u32>(3);
    ctk::Push(&CopyArray, 2u);
    ctk::Push(&CopyArray, 1u);
    auto Array = ctk::CreateArray(&CopyArray);
    ctk::Print("array (size=%u count=%u)", Array.Size, Array.Count);
    for(u32 i = 0; i < Array.Count; ++i)
    {
        ctk::Print(" %u", Array[i]);
    }
    ctk::PrintLine();
}

static void
OptionalTest()
{
    ctk::optional<int> X = {};
    ctk::PrintLine("X.Set? %s X.Value %i", X.Set ? "true" : "false", X.Value);
    X = 1;
    ctk::PrintLine("X.Set? %s X.Value %i", X.Set ? "true" : "false", X.Value);
    X = 2;
    ctk::PrintLine("X.Set? %s X.Value %i", X.Set ? "true" : "false", X.Value);
}

static void
AllocTest()
{
    auto Allocation = ctk::Alloc<u32>(4);
    for(u32 Index = 0; Index < 4; Index++)
    {
        ctk::PrintLine("%u", Allocation[Index]);
    }
}

int
main()
{
    AllocTest();
    return 0;
}
