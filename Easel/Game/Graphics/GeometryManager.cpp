/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Implementation of Geometry Manager
----------------------------------------------*/

#include "GeometryManager.h"
#include "CBufferStructs.h"

namespace Graphics {

GeometryManager::GeometryManager():
    m_pInputLayout(0)
{}

GeometryManager::~GeometryManager()
{
    // Cleanup all heap-allocated meshes
    for (auto& mesh : m_Meshes)
    {
        delete mesh;
    }

    // Cleanup all heap-allocated entities
    for (auto& entity : m_Entities)
    {
        delete entity;
    }

    // Cleanup base material
    delete m_pBasicMaterial;
}

void GeometryManager::Init(DeviceResources* DR)
{
    CompileShaders(DR);
    BuildMeshes(DR);
    BuildConstantBuffer(DR);
}

void GeometryManager::DrawEntities(ID3D11DeviceContext* a_pContext, Camera* a_pCamera)
{
    // Bind Constant Buffer
    a_pContext->VSSetConstantBuffers(
        0,  // slot 0
        1,  // 1 buffer
        m_pVSConstantBuffer.GetAddressOf()
    );

    // draw all entities in the scene
    for (Game::Entity* e : m_Entities)
    {
        e->Draw(a_pContext, m_pVSConstantBuffer.Get(), a_pCamera);
    }
}

void GeometryManager::BuildMeshes(DeviceResources* a_DR)
{
    // Build Vertex Array
    Vertex vertices[] = {
        {{0.0f, 0.0f, 0.0f},   {1.f, 1.f, 1.0f}},    // center
        {{0.0f, 0.5f, 0.0f},   {0.f, 1.f, 1.0f}},
        {{0.5f, -0.5f, 0.0f},  {0.5f, 0.5f, 0.0f}},
        {{-0.5f, -0.5f, 0.0f}, {0.3f, 0.5f, 1.f}}
    };

    int numVerts = std::size(vertices);

    // Make index list
    UINT indices[] = {
        0u,
        1u,
        2u,
        0u,
        2u,
        3u,
        0u,
        3u,
        1u
    };

    int numIndices = std::size(indices);

    // Make a mesh based on this geometry information
    m_Meshes.push_back(new Game::Mesh(vertices, numVerts, indices, numIndices, a_DR->GetD3DDevice()));

    // Make an entity based on the mesh and the material
    m_Entities.push_back(new Game::Entity(m_Meshes[0], m_pBasicMaterial));
}

void GeometryManager::BuildConstantBuffer(DeviceResources* a_DR)
{
    // Get the size of the constant buffer struct as a multiple of 16
    unsigned int size = sizeof(VSBasicData);
    size = (size + 15) / 16 * 16;

    // Describe the constant buffer and create it
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.ByteWidth = size;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.MiscFlags = 0;
    cbDesc.StructureByteStride = 0;

    // Store the cbuffer as a member field
    a_DR->GetD3DDevice()->CreateBuffer(&cbDesc, 0, m_pVSConstantBuffer.GetAddressOf());
}

// Programmatically Compiles and then Binds Shaders to the Render pipeline.
void GeometryManager::CompileShaders(DeviceResources* DR)
{
    using Microsoft::WRL::ComPtr;

    ID3D11Device* device = DR->GetD3DDevice();
    ID3D11DeviceContext* context = DR->GetD3DDeviceContext();

    ComPtr<ID3D10Blob> pBlob = 0;
    HRESULT hr;

    ComPtr<ID3D11VertexShader> pVS;
    ComPtr<ID3D11PixelShader> pPS;
    
    // Create Pixel Shader
    hr = D3DReadFileToBlob(L"..\\_Binary\\PixelShader.cso", &pBlob);
    ThrowIfFailed(hr);
    hr = device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), 0, &pPS);
    ThrowIfFailed(hr);

    // Create Vertex Shader
    hr = D3DReadFileToBlob(L"..\\_Binary\\VertexShader.cso", &pBlob);
    ThrowIfFailed(hr);
    hr = device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), 0, &pVS);
    ThrowIfFailed(hr);

    // Describe Input Layout
    const D3D11_INPUT_ELEMENT_DESC IED[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12u, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    
    // Create Input Layout and hold as a member field
    hr = device->CreateInputLayout(
        IED,
        (UINT)std::size(IED),
        pBlob->GetBufferPointer(),
        pBlob->GetBufferSize(),
        &m_pInputLayout);

    ThrowIfFailed(hr);

    // Bind input layout to Input Assembler Stage
    context->IASetInputLayout(m_pInputLayout.Get());

    // Set Primitive Topology of the input assembler stage
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create material based on compiled vertex/pixel shaders
    m_pBasicMaterial = new Material(pVS, pPS);
}

}