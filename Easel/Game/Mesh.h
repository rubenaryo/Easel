/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Mesh holds the internal vertex/index buffers for a particular object
----------------------------------------------*/
#ifndef MESH_H
#define MESH_H

#include "Graphics/DXCore.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <wrl/client.h>

namespace Graphics
{
struct Vertex;
}

namespace Game {

class Mesh
{
public:
    Mesh(Graphics::Vertex* vertexArray, unsigned int numVertices, unsigned int* indexArray, unsigned int numIndices, ID3D11Device* pDevice);
    //@architecture: Require input layout description. That way mesh only stores the data it needs
    Mesh(const std::string& pFile, ID3D11Device* a_pDevice);
    Mesh() = delete;
    ~Mesh() = default;

    // Sets the buffers and calls DrawIndexed
    void Draw(ID3D11DeviceContext* a_pContext);

    // Accessors
    inline ID3D11Buffer* GetVertexBuffer() const { return mpVertexBuffer.Get(); }
    inline ID3D11Buffer* GetIndexBuffer()  const { return mpIndexBuffer.Get();  }
    inline int           GetIndexCount()   const { return mIndexCount;          }

protected:
    // Model Loading Helper Functions
    void LoadSceneInfo(const aiScene* pScene, ID3D11Device* pDevice);

    // Internal buffers
    Microsoft::WRL::ComPtr<ID3D11Buffer> mpVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> mpIndexBuffer;

    // How many indices are in the index buffer
    int mIndexCount;

    // Creates the internally held vertex/index buffers
    void CreateBuffers(Graphics::Vertex* vertexArray, unsigned int numVertices, unsigned int* indexArray, unsigned int numIndices, ID3D11Device* pDevice);
};

}
#endif