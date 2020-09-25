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
    ~Mesh();

    // Sets the buffers and calls DrawIndexed
    void Draw(ID3D11DeviceContext* a_pContext) const ;

    // Accessors
    inline ID3D11Buffer* GetVertexBuffer() const { return mpVertexBuffer; }
    inline ID3D11Buffer* GetIndexBuffer()  const { return mpIndexBuffer;  }
    inline int           GetIndexCount()   const { return mIndexCount;    }

protected:
    // Model Loading Helper Functions
    void LoadSceneInfo(const aiScene* pScene, ID3D11Device* pDevice);

    // Internal buffers
    ID3D11Buffer* mpVertexBuffer;
    ID3D11Buffer* mpIndexBuffer;

    // How many indices are in the index buffer
    int mIndexCount;

    // Creates the internally held vertex/index buffers
    void CreateBuffers(Graphics::Vertex* vertexArray, unsigned int numVertices, unsigned int* indexArray, unsigned int numIndices, ID3D11Device* pDevice);

public:
    Mesh()                          = delete;
    Mesh(Mesh const&)               = delete;
    Mesh& operator=(Mesh const&)    = delete;
};

}
#endif