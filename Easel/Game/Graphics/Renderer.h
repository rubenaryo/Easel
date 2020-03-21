/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
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
#include "MaterialFactory.h"
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
    void InitEntities();

private:
    // Maps a material to a list of entities that utilize it
    std::unordered_map<Material*, std::vector<Game::Entity*>> m_EntityMap;

    // Dictionary of all loaded meshes (Eventually this may be handled by a mesh manager, who takes them in/out of memory
    // Key is a hardcoded :( identifying id
    std::unordered_map<std::string, Game::Mesh*> m_Meshes;

    // Sample all textures the same way,
    // TODO: DON'T Sample all textures the same way
    // A possible solution to this is to set sampler options as material params, then
    // generate a sampler state on the fly whenever is needed 
    // This begs the question: Perhaps drawing should be organized in separate piles, a table for
    // textured objects, and another for non-textured. Do real games even use non-textured objects?
    // This requires more thought.
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSamplerState;

    // Owning pointer to a material factory instance, which creates and distributes materials and textures
    std::unique_ptr<MaterialFactory> m_pMaterialFactory;

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