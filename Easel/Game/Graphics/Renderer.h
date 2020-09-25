/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Manager-level class for intelligently binding and drawing objects
----------------------------------------------*/
#ifndef RENDERER_H
#define RENDERER_H

#include "CBufferStructs.h"
#include "DXCore.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace Graphics
{
    class DeviceResources;
    class Camera;
    class Material;
    class MaterialFactory;
    class SkyRenderer;
}

namespace Game
{
    class Entity;
    class Mesh;
}

namespace Graphics {

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void Init(DeviceResources* dr);

    // For now, the renderer will handle updating the entities, 
    // In the future, perhaps a Physics Manager or AI Manager would be a good solution?
    void Update(ID3D11DeviceContext* context, float dt, const Camera* camera, const cbLighting* lightData);

    // Binds the fields necessary in the material, then draws every entity in m_EntityMap
    void Draw(ID3D11DeviceContext* context);

private:
    // Loads the necessary models into a collection
    void InitMeshes(DeviceResources* dr);

    // Creates the necessary material keys within m_Map, 
    void InitEntities();

private:
    // Maps a material to a list of entities that utilize it
    std::unordered_map<const Material*, std::vector<Game::Entity*>> mEntityMap;

    // Dictionary of all loaded meshes (Eventually this may be handled by a mesh manager, who takes them in/out of memory
    // Key is a hardcoded :( identifying id
    std::unordered_map<std::string, const Game::Mesh*> mMeshes;

    // Sample all textures the same way,
    // TODO: DON'T Sample all textures the same way
    // A possible solution to this is to set sampler options as material params, then
    // generate a sampler state on the fly whenever is needed 
    // This begs the question: Perhaps drawing should be organized in separate piles, a table for
    // textured objects, and another for non-textured. Do real games even use non-textured objects?
    // This requires more thought.
    ID3D11SamplerState* mpSamplerState;

    // Owning pointer to a material factory instance, which creates and distributes materials and textures
    std::unique_ptr<MaterialFactory> mpMaterialFactory;

    const SkyRenderer* mpSkyRenderer;

    // Cached constant buffer structs for camera and lighting
    cbCamera    mCameraBuffer;
    cbLighting  mLightingBuffer;

public: // Enforce use of the default constructor
    Renderer(Renderer const&)               = delete;
    Renderer& operator=(Renderer const&)    = delete;
};
}
#endif