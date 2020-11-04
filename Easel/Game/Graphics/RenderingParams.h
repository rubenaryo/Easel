#ifndef RENDERINGPARAMS_H
#define RENDERINGPARAMS_H

namespace Rendering {

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

}

#endif