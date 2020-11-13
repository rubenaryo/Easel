/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Material class for shader information
----------------------------------------------*/
#ifndef MATERIAL_H
#define MATERIAL_H

#include "DXCore.h"
#include "CBufferStructs.h"

namespace Rendering
{
struct VertexShader;
struct PixelShader;
}

namespace Rendering {

enum class TextureSlots : UINT
{
    DIFFUSE   = 0U,
    NORMAL    = 1U,
    SPECULAR  = 2U,
    ROUGHNESS = 3U,
    CUBE      = 4U,
    COUNT
};

struct ResourceBindChord
{
    ID3D11ShaderResourceView*  SRVs[(UINT)TextureSlots::COUNT];
};

// Materials own both VS and PS because they must match in the pipeline
struct Material
{
    const VertexShader*        VS;
    const PixelShader*         PS;
    const ResourceBindChord*    Resources;
    cbMaterialParams            Description;
};
    

}
#endif