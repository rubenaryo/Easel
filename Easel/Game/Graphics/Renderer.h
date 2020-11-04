/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Manager-level class for intelligently binding and drawing objects
----------------------------------------------*/
#ifndef RENDERER_H
#define RENDERER_H

#include "CBufferStructs.h"
#include "DXCore.h"
#include "SkyRenderer.h"
#include "../Transform.h"

#include <EASTL/hash_map.h>

namespace Rendering
{
    class DeviceResources;
    class Camera;
    struct ConstantBuffer;
}

namespace Rendering {

struct TEntity
{
    Game::Transform mTransform;
    MeshID          mMeshID;
    uint32_t        MaterialIndex;
};

struct InstancedDrawContext
{
    DirectX::XMFLOAT4X4*    WorldMatrices;
    ID3D11Buffer*           DynamicBuffer;
    uint32_t                MaterialIndex;
    uint32_t                InstancedMeshID;
    UINT                    InstanceCount;
};

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void Init(DeviceResources* dr);

    // For now, the renderer will handle updating the entities, 
    // In the future, perhaps a Physics Manager or AI Manager would be a good solution?
    void Update(ID3D11DeviceContext* context, float dt);

    // Binds the fields necessary in the material, then draws every entity in m_EntityMap
    void Draw(ID3D11DeviceContext* context);

private:
    // Performs all the instanced draw steps
    void InstancedDraw(InstancedDrawContext* drawContexts, UINT drawCallCount, ID3D11DeviceContext* context);
    
    // Loads the necessary models into a collection
    void InitMeshes(DeviceResources* dr);

    // Creates the necessary material keys within m_Map, 
    void InitDrawContexts(ID3D11Device* device);

    // Self Contained DX Calls to Render the skybox
    void RenderSky(ID3D11DeviceContext* context);

private:

    // All the Entities
    TEntity* Entities;
    UINT     EntityCount;

    InstancedDrawContext* instancingPasses;

    TSkyRenderer mSkyRenderer;

public: // Enforce use of the default constructor
    Renderer(Renderer const&)               = delete;
    Renderer& operator=(Renderer const&)    = delete;
};
}
#endif