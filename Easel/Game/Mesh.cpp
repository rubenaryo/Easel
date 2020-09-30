/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Mesh functionality
----------------------------------------------*/
#include "Mesh.h"
#include "Graphics/Vertex.h"

#include <vector>
#include "../System/PathMacros.h"

#include "Graphics/ThrowMacros.h"

namespace Game {

Mesh::Mesh(Graphics::Vertex* vertexArray, unsigned int numVertices, unsigned int* indexArray, unsigned int numIndices, ID3D11Device* pDevice)
{
    CreateBuffers(vertexArray, numVertices, indexArray, numIndices, pDevice);
}

Mesh::Mesh(const std::string& fileName, ID3D11Device* pDevice)
{
    Assimp::Importer Importer;

    // Load assimpScene with proper flags
    const aiScene* pScene = Importer.ReadFile(
        System::GetModelPathFromFile(fileName),
        aiProcess_Triangulate           |
        aiProcess_JoinIdenticalVertices |   // Remove unnecessary duplicate information
        aiProcess_GenNormals            |   // Ensure normals are generated
        aiProcess_CalcTangentSpace          // Needed for normal mapping
    );

    if (pScene)
    {
        LoadSceneInfo(pScene, pDevice);
    }
    #if defined(DEBUG)
    else
    {
        char buf[256];
        sprintf_s(buf, "Error parsing '%s': '%s'\n", fileName.c_str(), Importer.GetErrorString());
        throw std::exception(buf);
        return;
    }
    #endif

    #if defined(DEBUG)
    static const char vbDebug[] = "_VertexBuffer";
    static const char ibDebug[] = "_IndexBuffer";
    std::string debugName = fileName + vbDebug;
    HRESULT hr = mpVertexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, debugName.size(), debugName.c_str());
    COM_EXCEPT(hr);
    debugName = fileName + ibDebug;
    hr = mpIndexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, debugName.size(), debugName.c_str());
    COM_EXCEPT(hr);
    #endif
}

// Loads imports the model data passed in by an Assimp scene into 
// Easel's Mesh interface, which can be worked with
void Mesh::LoadSceneInfo(const aiScene* pScene, ID3D11Device* pDevice)
{
    // temporary vectors to hold vertex/index information
    std::vector<Graphics::Vertex> vertices;
    std::vector<unsigned int> indices;

    // aiScenes may be composed of multiple submeshes,
    // we want to coagulate this into a single vertex/index buffer
    for (unsigned int i = 0; i < pScene->mNumMeshes; ++i)
    {
        const aiMesh* pMesh = pScene->mMeshes[i];
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

            // Check for tangents and bitangents
            bool tangentsGenerated = pMesh->HasTangentsAndBitangents();
            const aiVector3D tangent   = tangentsGenerated ? pMesh->mTangents[j]   : cZero;
            const aiVector3D bitangent = tangentsGenerated ? pMesh->mBitangents[j] : cZero;

            // Create one of our vertices with this information
            Graphics::Vertex v;
            v.position  = DirectX::XMFLOAT3(position.x, position.y, position.z);
            v.normal    = DirectX::XMFLOAT3(normal.x, normal.y, normal.z);
            v.uv        = DirectX::XMFLOAT2(texCoord.x, texCoord.y);
            v.tangent   = DirectX::XMFLOAT3(tangent.x, tangent.y, tangent.z);
            v.binormal  = DirectX::XMFLOAT3(bitangent.x, bitangent.y, bitangent.z);

            vertices.push_back(v);
        }

        // Process Indices next
        for (unsigned int j = 0; j < pMesh->mNumFaces; ++j)
        {
            const aiFace& face = pMesh->mFaces[j];

            #if defined(DEBUG)
            assert(face.mNumIndices == 3); // Sanity check
            #endif
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
    
    CreateBuffers(&vertices[0], numVerts, &indices[0], numIndices, pDevice);
}

void Mesh::CreateBuffers(Graphics::Vertex* vertexArray, unsigned int numVertices, unsigned int* indexArray, unsigned int numIndices, ID3D11Device* pDevice)
{
    // Create the vertex buffer
    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Graphics::Vertex) * numVertices; // Number of vertices
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA initialVertexData;
    initialVertexData.pSysMem = vertexArray;
    HRESULT hr = pDevice->CreateBuffer(&vbd, &initialVertexData, &mpVertexBuffer);
    
    COM_EXCEPT(hr);

    // Create the index buffer
    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(unsigned int) * numIndices; // Number of indices
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA initialIndexData;
    initialIndexData.pSysMem = indexArray;
    hr = pDevice->CreateBuffer(&ibd, &initialIndexData, &mpIndexBuffer);

    COM_EXCEPT(hr);

    // Save the index count
    this->mIndexCount = numIndices;
}

void Mesh::Draw(ID3D11DeviceContext* pContext) const
{
    // Bind buffers to IA
    UINT stride = sizeof(Graphics::Vertex);
    UINT offset = 0;
    
    pContext->IASetVertexBuffers(0, 1, &mpVertexBuffer, &stride, &offset);
    pContext->IASetIndexBuffer(mpIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    
    // Actually draw the mesh
    pContext->DrawIndexed(this->mIndexCount, 0, 0);

}

Mesh::~Mesh()
{
    mpVertexBuffer->Release();
    mpIndexBuffer->Release();
}

}