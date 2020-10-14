/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Entity class functionality
----------------------------------------------*/
#include "Entity.h"
#include "Mesh.h"

namespace Game {

Entity::Entity(const Mesh* pMesh) :
    mpMesh(pMesh)
{}

Entity::Entity(const Mesh* pMesh, Transform transform) :
    mpMesh(pMesh),
    mTransform(transform)
{}


void Entity::Draw(ID3D11DeviceContext* context) const
{
    // Draw the mesh
    // TODO: Instanced Rendering
    mpMesh->Draw(context);
}

}