/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Entity is an instance of a mesh with a unique transform and material
----------------------------------------------*/
#ifndef ENTITY_H
#define ENTITY_H

#include "Graphics/DXCore.h"
#include "Transform.h"

namespace Game
{
class Mesh;
class Transform;
}

namespace Game
{
class Entity
{
public:
    Entity(const Mesh* pMesh);
    Entity(const Mesh* pMesh, Transform transform);
    ~Entity() = default;

    // Call Mesh's draw, passing in the world matrix is done by the Renderer class
    void Draw(ID3D11DeviceContext* pContext);

    // Accessors
    inline const Mesh* GetMesh() const { return mpMesh; }
    Transform*         GetTransform();
private:
    // Entity information
    const Mesh* mpMesh;
    Transform   mTransform;

public:
    Entity()                            = delete;
    Entity(Entity const&)               = delete;
    Entity& operator=(Entity const&)    = delete;
};

}
#endif