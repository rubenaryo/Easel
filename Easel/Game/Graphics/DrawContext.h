#ifndef DRAWCONTEXT_H
#define DRAWCONTEXT_H

#include "DXCore.h"

namespace Rendering {

struct InstancedDrawContext
{
    DirectX::XMFLOAT4X4*    WorldMatrices;
    ID3D11Buffer*           DynamicBuffer;
    uint32_t                MaterialIndex;
    MeshID                  InstancedMeshID;
    UINT                    InstanceCount;
};

}

#endif