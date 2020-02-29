/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Defines the structures of different lights in the scene.
----------------------------------------------*/
#ifndef LIGHTS_H
#define LIGHTS_H

#include <DirectXMath.h>
namespace Graphics {

// A Directional Light is just a color and a direction
// alignas(16) is required, since every element needs to be on a 16 byte boundary for the cbuffer
struct DirectionalLight
{
    alignas(16) DirectX::XMFLOAT3 diffuseColor;
    alignas(16) DirectX::XMFLOAT3 direction;
};

}
#endif