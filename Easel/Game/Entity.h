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
    Entity(Mesh* pMesh);
    Entity(Mesh* pMesh, Transform transform);
    Entity() = delete;
    ~Entity() = default;

    // Call Mesh's draw, passing in the world matrix is done by the Renderer class
    void Draw(ID3D11DeviceContext* pContext);

    // Accessors
    inline Mesh*    GetMesh() const;
    Transform*      GetTransform();
private:
    // Entity information
    Mesh*     mpMesh;
    Transform mTransform;
};

}
#endif