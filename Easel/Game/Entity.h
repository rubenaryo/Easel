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
    Entity(Mesh* a_pMesh, Graphics::Material* a_pMaterial);
    Entity() = delete;
    ~Entity() = default;

    // Set constant buffer information and draw the underlying mesh
    void Draw(ID3D11DeviceContext* a_pContext, ID3D11Buffer* a_pVSCBuffer);

    // Accessors
    inline Mesh*      GetMesh() const;
    inline Transform* GetTransform();
private:
    // Entity information
    Mesh*     m_pMesh;
    Graphics::Material* m_pMaterial;
    Transform m_Transform;
};

}
#endif