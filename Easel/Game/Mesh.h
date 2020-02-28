/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Mesh holds the internal vertex/index buffers for a particular object
----------------------------------------------*/
#ifndef MESH_H
#define MESH_H

#include "Graphics/Vertex.h"
#include "Graphics/D3D11App.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace Game {

class Mesh
{
public:
    Mesh(Graphics::Vertex* a_VertexArray, int a_NumVertices, unsigned int* a_IndexArray, int a_NumIndices, ID3D11Device* a_pDevice);
    Mesh(const std::string& pFile);
    Mesh() = delete;
    ~Mesh() = default;

    // Sets the buffers and calls DrawIndexed
    void Draw(ID3D11DeviceContext* a_pContext);

    // Accessors
    inline ID3D11Buffer* GetVertexBuffer() const { return m_pVertexBuffer.Get(); }
    inline ID3D11Buffer* GetIndexBuffer()  const { return m_pIndexBuffer.Get();  }
    inline int           GetIndexCount()   const { return m_IndexCount;          }

protected:
    // Model Loading Helper Functions
    void LoadModelInfo(const aiScene* a_pScene);

    // Internal buffers
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;

    // How many indices are in the index buffer
    int m_IndexCount;

    // Creates the internally held vertex/index buffers
    void CreateBuffers(Graphics::Vertex* a_VertexArray, int a_NumVertices, unsigned int* a_IndexArray, int a_NumIndices, ID3D11Device* a_pDevice);
};

}
#endif