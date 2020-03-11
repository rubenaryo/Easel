/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Entity is an instance of a mesh with a unique transform and material
----------------------------------------------*/
#ifndef ENTITY_H
#define ENTITY_H

#include "Graphics/DXCore.h"
#include "Mesh.h"
#include "Graphics/Material.h"
#include "Transform.h"

namespace Game
{
class Entity
{
public:
    Entity(Mesh* a_pMesh);
    Entity(Mesh* a_pMesh, Transform a_Transform);
    Entity() = delete;
    ~Entity() = default;

    // Call Mesh's draw, passing in the world matrix is done by the Renderer class
    void Draw(ID3D11DeviceContext* a_pContext);

    // Accessors
    inline Mesh*      GetMesh() const;
    Transform* GetTransform();
private:
    // Entity information
    Mesh*     m_pMesh;
    Transform m_Transform;
};

}
#endif