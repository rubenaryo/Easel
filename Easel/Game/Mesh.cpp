/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Mesh functionality
----------------------------------------------*/
#include "Mesh.h"

namespace Game {
using namespace Graphics;

Mesh::Mesh(Vertex* a_VertexArray, unsigned int a_NumVertices, unsigned int* a_IndexArray, unsigned int a_NumIndices, ID3D11Device* a_pDevice)
{
    CreateBuffers(a_VertexArray, a_NumVertices, a_IndexArray, a_NumIndices, a_pDevice);
}

Mesh::Mesh(const std::string& a_pFile, ID3D11Device* a_pDevice)
{
    Assimp::Importer Importer;

    // Load assimpScene with proper flags
    const aiScene* pScene = Importer.ReadFile(
        a_pFile,
        aiProcess_Triangulate           |
        aiProcess_JoinIdenticalVertices |   // Remove unnecessary duplicate information
        aiProcess_GenNormals            |   // Ensure normals are generated
        aiProcess_CalcTangentSpace          // Needed for normal mapping
    );

    if (pScene)
    {
        LoadSceneInfo(pScene, a_pDevice);
    }
    else
    {
        char buf[64];
        sprintf_s(buf, "Error parsing '%s': '%s'\n", a_pFile.c_str(), Importer.GetErrorString());
        throw std::exception(buf);
    }
}

// Loads imports the model data passed in by an Assimp scene into 
// Easel's Mesh interface, which can be worked with
void Mesh::LoadSceneInfo(const aiScene* a_pScene, ID3D11Device* a_pDevice)
{
    // temporary vectors to hold vertex/index information
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // aiScenes may be composed of multiple submeshes,
    // we want to coagulate this into a single vertex/index buffer
    for (unsigned int i = 0; i < a_pScene->mNumMeshes; ++i)
    {
        const aiMesh* pMesh = a_pScene->mMeshes[i];
        const aiVector3D cZero(0.0f, 0.0f, 0.0f);

        // Process Vertices for this mesh
        for (unsigned int j = 0; j < pMesh->mNumVertices; ++j)
        {
            // Every vertex has a position
            const aiVector3D position = pMesh->mVertices[j];

            // Check for normals
            const aiVector3D normal   = pMesh->HasNormals() ? pMesh->mNormals[j] : cZero;
            
            // Assume every vertex holds only one set of uv coords
            const aiVector3D texCoord = pMesh->HasTextureCoords(0) ? pMesh->mTextureCoords[0][j] : cZero;
        
            // Create one of our vertices with this information
            Vertex v;
            v.position = DirectX::XMFLOAT3(position.x, position.y, position.z);
            v.normal = DirectX::XMFLOAT3(normal.x, normal.y, normal.z);
            v.uv = DirectX::XMFLOAT2(texCoord.x, texCoord.y);

            vertices.push_back(v);
        }

        // Process Indices next
        for (unsigned int j = 0; j < pMesh->mNumFaces; ++j)
        {
            const aiFace& face = pMesh->mFaces[j];
            assert(face.mNumIndices == 3); // Sanity check
            
            // All the indices of this face are valid, add to list
            indices.push_back(face.mIndices[0]);
            indices.push_back(face.mIndices[1]);
            indices.push_back(face.mIndices[2]);
        }

    }

    // Now, we have the vertices and indices for every mesh in the scene, so we make the buffers.
    // These temporary variables are to avoid compiler warnings
    unsigned int numVerts = static_cast<unsigned int>(vertices.size());
    unsigned int numIndices = static_cast<unsigned int>(indices.size());
    
    CreateBuffers(&vertices[0], numVerts, &indices[0], numIndices, a_pDevice);
}

void Mesh::CreateBuffers(Vertex* a_VertexArray, unsigned int a_NumVertices, unsigned int* a_IndexArray, unsigned int a_NumIndices, ID3D11Device* a_pDevice)
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