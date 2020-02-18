/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Vertex structure definitions
----------------------------------------------*/
#ifndef VERTEX_H
#define VERTEX_H

#include "D3D11App.h"

namespace Graphics {

struct Vertex
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT4 color;
};
}

#endif