/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Mesh functionality
----------------------------------------------*/
#include "Mesh.h"
#include <vector>

using namespace Graphics;
namespace Game {

Mesh::Mesh(Vertex* a_VertexArray, int a_NumVertices, unsigned int* a_IndexArray, int a_NumIndices, ID3D11Device* a_pDevice)
{
    CreateBuffers(a_VertexArray, a_NumVertices, a_IndexArray, a_NumIndices, a_pDevice);
}

Mesh::Mesh(const std::string& pFile)
{
    Assimp::Importer Importer;

    // Load assimpScene with proper flags
    const aiScene* scene = Importer.ReadFile(
        pFile,
        aiProcess_Triangulate           |
        aiProcess_JoinIdenticalVertices |   // Remove unnecessary duplicate information
        aiProcess_MakeLeftHanded        |   // Must make left handed for DirectX
        aiProcess_GenNormals            |   // Ensure normals are generated
        aiProcess_CalcTangentSpace          // Needed for normal mapping
    );

    if (scene)
    {
        LoadModelInfo(scene);
    }
    else
    {
        char buf[64];
        sprintf_s(buf, "Error parsing '%s': '%s'\n", pFile.c_str(), Importer.GetErrorString());
        throw std::exception(buf);
    }
}

// Loads imports the model data passed in by an Assimp scene into 
// Easel's Mesh interface, which can be worked with
void Mesh::LoadModelInfo(const aiScene* a_pScene)
{
    // aiScenes may be composed of multiple submeshes,
    // we want to coagulate this into a single vertex/index buffer
    for (unsigned int i = 0; i < a_pScene->mNumMeshes; ++i)
    {
        //TODO: import code
    }
}

void Mesh::CreateBuffers(Vertex* a_VertexArray, int a_NumVertices, unsigned int* a_IndexArray, int a_NumIndices, ID3D11Device* a_pDevice)
{
    // Create the vertex buffer
    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * a_NumVertices; // Number of vertices
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA initialVertexData;
    initialVertexData.pSysMem = a_VertexArray;
    a_pDevice->CreateBuffer(&vbd, &initialVertexData, m_pVertexBuffer.GetAddressOf());
    
    // Create the index buffer
    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(unsigned int) * a_NumIndices; // Number of indices
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA initialIndexData;
    initialIndexData.pSysMem = a_IndexArray;
    a_pDevice->CreateBuffer(&ibd, &initialIndexData, m_pIndexBuffer.GetAddressOf());
    
    // Save the index count
    this->m_IndexCount = a_NumIndices;
}

void Mesh::Draw(ID3D11DeviceContext* a_pContext)
{
    // Bind buffers to IA
    UINT stride = sizeof(Vertex);
    UINT offset = 0u;
    
    a_pContext->IASetVertexBuffers(0u, 1u, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
    a_pContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
    
    // Actually draw the mesh
    a_pContext->DrawIndexed(this->m_IndexCount, 0u, 0);

}

}