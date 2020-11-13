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
    struct InstancedDrawContext;
}

namespace Rendering {

struct Entity
{
    Game::Transform mTransform;
    MeshID          mMeshID;
    uint32_t        MaterialIndex;
};

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void Init(DeviceResources const& dr);

    // For now, the renderer will handle updating the entities, 
    // In the future, perhaps a Physics Manager or AI Manager would be a good solution?
    void Update(ID3D11DeviceContext* context, float dt);

    // Binds the fields necessary in the material, then draws every entity in m_EntityMap
    void Draw(ID3D11DeviceContext* context);

private:
    // Performs all the instanced draw steps
    void InstancedDraw(InstancedDrawContext* drawContexts, UINT drawCallCount, ID3D11DeviceContext* context);
    
    // Loads the necessary models into a collection
    void InitMeshes(DeviceResources const& dr);

    void LoadShaders();

    void InitEntities();

    // Creates the necessary material keys within m_Map, 
    void InitDrawContexts(ID3D11Device* device);

    // Self Contained DX Calls to Render the skybox
    void RenderSky(ID3D11DeviceContext* context);

private:

    // All the Entities
    Entity* Entities;
    UINT     EntityCount;

    InstancedDrawContext* InstancingPasses;
    UINT                  InstancingPassCount;

    SkyRenderer mSkyRenderer;

public: // Enforce use of the default constructor
    Renderer(Renderer const&)               = delete;
    Renderer& operator=(Renderer const&)    = delete;
};
}
#endif