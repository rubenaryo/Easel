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

struct alignas(16) VSBasicData
{
    DirectX::XMFLOAT4X4 world;
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 projection;
};

struct alignas(16) LightBuffer
{
    DirectX::XMFLOAT3            ambientColor;
    DirectionalLight    lights[8];
    unsigned int        numLights;
};

}
#endif