/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Entity class functionality
----------------------------------------------*/
#include "Entity.h"
#include "Mesh.h"

namespace Game {

Entity::Entity(Mesh* a_pMesh) :
    mpMesh(a_pMesh)
{}

Entity::Entity(Mesh* a_pMesh, Transform a_Transform) :
    mpMesh(a_pMesh),
    mTransform(a_Transform)
{}


void Entity::Draw(ID3D11DeviceContext* a_pContext)
{
    // Draw the mesh
    // Note: This method used to be far more convoluted, now it is in a good place to eventually implement instanced rendering
    // TODO: Instanced Rendering
    mpMesh->Draw(a_pContext);
}

inline Mesh* Entity::GetMesh() const
{
    return mpMesh;
}

Transform* Entity::GetTransform()
{
    return &mTransform;
}

}