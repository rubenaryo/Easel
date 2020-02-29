/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Interface for management of geometry 
buffers, input layouts, shader compilation
----------------------------------------------*/
#ifndef GEOMETRYMANAGER_H
#define GEOMETRYMANAGER_H

#include "D3D11App.h"
#include "DeviceResources.h"
#include "Vertex.h"
#include <vector>
#include "../Mesh.h"
#include "../Entity.h"
#include "Camera.h"

namespace Graphics {
using Microsoft::WRL::ComPtr;
class GeometryManager final {
public:
    GeometryManager();
    ~GeometryManager();

    void Init(DeviceResources* a_DR);

    void DrawEntities(ID3D11DeviceContext* a_pContext, Camera* a_pCamera);
    void UpdateEntities(float dt);

private:
    // Creates vertex/index arrays and populates m_Meshes/m_Entities
    void BuildMeshes(DeviceResources* a_DR);

    // Builds the constant buffer needed for the WVP matrix in the vertex shader
    void BuildConstantBuffer(DeviceResources* a_DR);

    // Compiles shader files, 
    void CompileShaders(DeviceResources* a_DR);

private:
    ComPtr<ID3D11InputLayout>   m_pInputLayout;
    ComPtr<ID3D11Buffer>        m_pVSConstantBuffer;

    // Owning pointers to vertex/pixel shaders
    ComPtr<ID3D11VertexShader>  m_pVertexShader;
    ComPtr<ID3D11PixelShader>   m_pPixelShader;


    std::vector<Game::Mesh*>   m_Meshes;
    std::vector<Game::Entity*> m_Entities;

    Material* m_pBasicMaterial;

};

}

#endif