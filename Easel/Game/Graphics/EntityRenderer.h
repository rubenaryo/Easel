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

#include <random>

namespace Rendering
{
    class DeviceResources;
    class Camera;
    class LightingManager;

    struct InstancedDrawContext;
}

namespace Rendering {

struct EntityGroup
{
    DirectX::XMFLOAT4X4*    WorldMatrices; // Mappable world matrices
    Transform*              Transforms;    // Parameters used to generate each world matrix (Pos, Scale, Rot)
    uint64_t                Count;
};
#define ENTITY_GROUP_COUNT 4

class alignas(8) EntityRenderer
{
public:
    EntityRenderer();

    void Init(DeviceResources const& dr);

    // For now, the renderer will handle updating the entities, 
    // In the future, perhaps a Physics Manager or AI Manager would be a good solution?
    void Update(DeviceResources* dr, const ConstantBufferBindPacket* camPacket, const ConstantBufferBindPacket* lightPacket, float dt);

    // Binds the fields necessary in the material, then draws every entity in m_EntityMap
    void Draw(DeviceResources* dr);

    void Shutdown();

private:
    // Performs all the instanced draw steps
    void InstancedDraw(InstancedDrawContext* pDrawContext, ID3D11DeviceContext* context, ID3D11CommandList** out_cmdList);
    
    // Loads the necessary models into a collection
    void InitMeshes(DeviceResources const& dr);

    // Populates the Entity List
    void InitEntities();

    void InitEntityGroup(EntityGroup* grp, const UINT numEntities, std::uniform_real_distribution<float> dist);

    // Creates the necessary material keys within m_Map, 
    void InitDrawContexts(ID3D11Device* device);

    //void PrepareForDeferredRender(InstancedDrawContext* pDrawContext, ID3D11DeviceContext* def_Context, 

private:
    // All the Entities
    EntityGroup EntityGroups[ENTITY_GROUP_COUNT];

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