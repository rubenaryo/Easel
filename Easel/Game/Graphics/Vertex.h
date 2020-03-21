/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Vertex structure definitions
----------------------------------------------*/
#ifndef VERTEX_H
#define VERTEX_H

#include "DXCore.h"

namespace Graphics {

struct Vertex
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT2 uv;
    DirectX::XMFLOAT3 tangent;
    DirectX::XMFLOAT3 binormal;
};
}

#endif