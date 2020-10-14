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
struct Transform;
}

namespace Game
{
class Entity
{
public:
    explicit Entity(const Mesh* pMesh);
    explicit Entity(const Mesh* pMesh, Transform transform);
    ~Entity() = default;

    // Call Mesh's draw, passing in the world matrix is done by the Renderer class
    void Draw(ID3D11DeviceContext* pContext) const;

    Transform mTransform;

private:
    // Entity information
    const Mesh* mpMesh;

public:
    Entity() = delete;
};

}
#endif