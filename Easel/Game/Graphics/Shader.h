/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Wrapper for Vertex/Pixel/other shader code
----------------------------------------------*/
#ifndef SHADER_H
#define SHADER_H

#include "DXCore.h"
#include "ThrowMacros.h"

namespace Rendering {

#pragma region VertexShader Stuff
typedef uint8_t semantic_t;
enum class Semantics : semantic_t
{
    POSITION,
    NORMAL,
    TEXCOORD,
    TANGENT,
    BINORMAL,
    COLOR,
    BLENDINDICES,
    BLENDWEIGHTS,
    WORLDMATRIX,
    COUNT
};

struct VertexBufferDescription
{
    Semantics* SemanticsArr;
    uint16_t*  ByteOffsets;
    uint16_t   AttrCount;
    uint16_t   ByteSize;
};
#pragma endregion

struct VertexShader
{
    ID3D11InputLayout*  InputLayout;
    ID3D11VertexShader* Shader;
    VertexBufferDescription VertexDesc;
    VertexBufferDescription InstanceDesc; // Note: The allocated memory inside this one is contiguous with VertexDesc, so no free's are required.
    bool8_t Instanced;
};

struct PixelShader
{
    ID3D11SamplerState* SamplerState;
    ID3D11PixelShader*  Shader;
};
}

#endif