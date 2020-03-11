/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Entity class functionality
----------------------------------------------*/
#include "Entity.h"
#include "Graphics/CBufferStructs.h"

namespace Game {

Entity::Entity(Mesh* a_pMesh) :
    m_pMesh(a_pMesh)
    // This constructor triggers the default constructor for the transform class
{}

Entity::Entity(Mesh* a_pMesh, Transform a_Transform) :
    m_pMesh(a_pMesh),
    m_Transform(a_Transform)
{}


void Entity::Draw(ID3D11DeviceContext* a_pContext)
{
    // Draw the mesh
    // Note: This method used to be far more convoluted, now it is in a good place to eventually implement instanced rendering
    // TODO: Instanced Rendering
    m_pMesh->Draw(a_pContext);
}

inline Mesh* Entity::GetMesh() const
{
    return m_pMesh;
}

Transform* Entity::GetTransform()
{
    return &m_Transform;
}

}