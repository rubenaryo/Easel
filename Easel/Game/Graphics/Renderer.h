/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Manager-level class for intelligently binding and drawing objects
----------------------------------------------*/
#ifndef RENDERER_H
#define RENDERER_H

#include "DXCore.h"
#include "DeviceResources.h"
#include "Material.h"
#include "../Mesh.h"
#include "../Entity.h"
#include "ShaderFactory.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include <unordered_map>
#include <vector>
#include <memory>

namespace Graphics {

class Renderer final
{
const unsigned int c_ReservedBufferSlot = 13u;

public:
    Renderer();
    ~Renderer();

    void Init(DeviceResources* a_DR);

    // For now, the renderer will handle updating the entities, 
    // In the future, perhaps a Physics Manager or AI Manager would be a good solution?
    void Update(ID3D11DeviceContext* context, float dt, Camera* camera);

    // Binds the fields necessary in the material, then draws every entity in m_EntityMap
    void Draw(ID3D11DeviceContext* context);

private:
    // Loads the necessary models into a collection
    void InitMeshes(DeviceResources* a_DR);

    // Creates the necessary material keys within m_Map, 
    void InitMaterials();

private:
    // Maps a material to a list of entities that utilize it
    std::unordered_map<Material*, std::vector<Game::Entity*>> m_EntityMap;

    // Dictionary of all loaded meshes (Eventually this may be handled by a mesh manager, who takes them in/out of memory
    // Key is a hardcoded :( identifying id
    std::unordered_map<std::string, Game::Mesh*> m_Meshes;

    // Owning pointer to a shaderfactory, which assigns and distributes shaders.
    std::unique_ptr<ShaderFactory> m_pShaderFactory;

    // Two reserved buffers are bound to slot index 13:
    // - CameraBuffer holds the view/projection matrix
    // - MaterialBuffer holds the states of the material parameters, such as specularity, color tint. 
    VSConstantBuffer* m_pCameraBuffer;
    PSConstantBuffer* m_pMaterialBuffer;

public: // Enforce the use of the default constructor
    Renderer(Renderer const&)               = delete;
    Renderer& operator=(Renderer const&)    = delete;
};

}

#endif