/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Manager-level class for intelligently binding and drawing objects
----------------------------------------------*/
#ifndef RENDERER_H
#define RENDERER_H

#include "BatchRenderer.h"
#include "CBufferStructs.h"
#include "ConstantBuffer.h"
#include "DXCore.h"
#include "ResourceCodex.h"
#include "../Transform.h"

#include <EASTL/hash_map.h>
#include <EASTL/internal/thread_support.h>

namespace Rendering
{
    class DeviceResources;
    class Camera;

    struct InstancedDrawContext;
}

namespace Rendering {

struct Entity
{
    Game::Transform mTransform;
    MeshID          mMeshID;
    uint32_t        MaterialIndex;
};

class alignas(8) EntityRenderer
{
public:
    EntityRenderer();
    ~EntityRenderer();

    void Init(DeviceResources const& dr);

    // For now, the renderer will handle updating the entities, 
    // In the future, perhaps a Physics Manager or AI Manager would be a good solution?
    void Update(ID3D11DeviceContext* context, float dt);

    // Binds the fields necessary in the material, then draws every entity in m_EntityMap
    void Draw(ID3D11DeviceContext* context);

private:
    // Performs all the instanced draw steps
    void InstancedDraw(ID3D11DeviceContext* context);
    
    // Loads the necessary models into a collection
    void InitMeshes(DeviceResources const& dr);

    // Populates the Entity List
    void InitEntities();

    // Creates the necessary material keys within m_Map, 
    void InitDrawContexts(ID3D11Device* device);

private:

    // All the Entities
    Entity*  Entities;
    UINT     EntityCount;

    // Array of Instancing Information
    InstancedDrawContext* InstancingPasses;
    UINT                  InstancingPassCount;

    // Constant Buffer that holds material parameters
    ConstantBufferBindPacket MaterialParamsCB;

    // Constant Buffer that holds non-instanced entity world matrices
    ConstantBufferBindPacket EntityCB;

    // Distributes Draw and Update calls across multiple threads
    BatchRenderer EntityBatchRenderer;

public: // Enforce use of the default constructor
    EntityRenderer(EntityRenderer const&)               = delete;
    EntityRenderer& operator=(EntityRenderer const&)    = delete;
};

}
#endif