/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Declation of structs used as constant buffers by various shaders
----------------------------------------------*/
#ifndef CBUFFERSTRUCTS_H
#define CBUFFERSTRUCTS_H

#include <DirectXMath.h>
#include "LightStructs.h"

namespace Graphics
{
using DirectX::XMFLOAT4X4;
using DirectX::XMFLOAT3;

struct alignas(16) VSBasicData
{
    XMFLOAT4X4 world;
    XMFLOAT4X4 view;
    XMFLOAT4X4 projection;
};

struct alignas(16) LightBuffer
{
    XMFLOAT3            ambientColor;
    DirectionalLight    lights[8];
    unsigned int        numLights;
};

}
#endif