/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/10
Description : Mesh stores the vertex,index buffers ready to be drawn by DirectX
----------------------------------------------*/
#ifndef EASEL_MESH_H
#define EASEL_MESH_H

#include "DXCore.h"
#include "Shader.h"

namespace Rendering {

struct Mesh
{
    ID3D11Buffer* VertexBuffer;
    ID3D11Buffer* IndexBuffer;
    UINT          IndexCount;
    UINT          Stride;
};

}

#endif