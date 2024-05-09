#ifndef RENDERINGPARAMS_H
#define RENDERINGPARAMS_H

namespace Renderer {

enum class EASEL_PIPELINE_STAGE : UINT
{
    EPS_IA,     // Input Assembler
    EPS_VS,     // Vertex Shader
    EPS_GS,     // Geometry Shader
    EPS_RS,     // Rasterizer
    EPS_PS,     // Pixel Shader
    EPS_OM,     // Output Merger
    EPS_COUNT
};

enum class EASEL_SHADER_STAGE : UINT
{
    ESS_VS,
    ESS_PS,
    ESS_COUNT
};

// Reserved Constant Buffer Registers for Vertex Shader Stage
enum class VS_REGISTERS : UINT
{
    // ESS_VS
    CAMERA = 10,
    WORLD  = 11,
};

// Reserved Constant Buffer Registers for Pixel Shader Stage
enum class PS_REGISTERS : UINT
{
    // ESS_PS
    LIGHTS   = 10,
    MATERIAL = 11
};


}

#endif