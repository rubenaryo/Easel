/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Declation of structs used as constant buffers by various shaders
----------------------------------------------*/
#ifndef CBUFFERSTRUCTS_H
#define CBUFFERSTRUCTS_H

#include <DirectXMath.h>
#include <DirectXColors.h>
#include "LightStructs.h"

namespace Renderer
{

struct alignas(16) cbCamera
{
    DirectX::XMFLOAT4X4 viewProjection;
};

struct alignas(16) cbPerEntity
{
    DirectX::XMFLOAT4X4 world;
};

struct alignas(16) cbLighting
{
    DirectX::XMFLOAT3A ambientColor;
    DirectionalLight   directionalLight;
    DirectX::XMFLOAT3A cameraWorldPos;
};

struct alignas(16) cbMaterialParams
{
    DirectX::XMFLOAT4  colorTint = DirectX::XMFLOAT4(DirectX::Colors::Black);
    float              specularExp = 0.0f;
};

}
#endif