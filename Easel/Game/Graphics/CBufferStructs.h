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

struct alignas(16) cbPerFrame
{
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 projection;
};

struct alignas(16) cbPerEntity
{
    DirectX::XMFLOAT4X4 world;
};

struct alignas(16) LightBuffer
{
    DirectX::XMFLOAT3A ambientColor;
    DirectionalLight   directionalLight;
};

struct alignas(16) cbMaterialParams
{
    DirectX::XMFLOAT4  m_ColorTint;
    float               m_Specularity;
};

}
#endif