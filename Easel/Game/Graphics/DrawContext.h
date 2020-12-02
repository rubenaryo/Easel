#ifndef DRAWCONTEXT_H
#define DRAWCONTEXT_H

#include "DXCore.h"

namespace Rendering {

struct InstancedDrawContext
{
    const EntityGroup*      Entities;
    const Mesh*             InstancedMesh;
    const Material*         EntityMaterial;
    ID3D11Buffer*           DynamicBuffer;  // Cached GPU-friendly data of all the world matrices
};

}

#endif