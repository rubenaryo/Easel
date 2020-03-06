/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Entity class functionality
----------------------------------------------*/
#include "Entity.h"
#include "Graphics/CBufferStructs.h"

namespace Game {

Entity::Entity(Mesh* a_pMesh, Graphics::Material* a_pMaterial) :
    m_pMesh(a_pMesh),
    m_pMaterial(a_pMaterial)
{}

void Entity::Draw(ID3D11DeviceContext* a_pContext, ID3D11Buffer* a_pVSCBuffer)
{
    // Fill out data to be passed to the shader
    Graphics::cbPerEntity vsData;
    vsData.world = this->GetTransform()->GetWorldMatrix();

    // Copy this data to the constant buffer we intend to use
    D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
    a_pContext->Map(a_pVSCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);

    // Straight memcpy() into the resource
    memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));

    // Unmap so the GPU can once again use the buffer
    a_pContext->Unmap(a_pVSCBuffer, 0);

    // Right before drawing, set the shaders
    a_pContext->VSSetShader(m_pMaterial->GetVertexShader(), 0, 0);
    a_pContext->PSSetShader(m_pMaterial->GetPixelShader(), 0, 0);

    // Draw the mesh
    m_pMesh->Draw(a_pContext);
}

inline Mesh* Entity::GetMesh() const
{
    return m_pMesh;
}

inline Transform* Entity::GetTransform()
{
    return &m_Transform;
}

}