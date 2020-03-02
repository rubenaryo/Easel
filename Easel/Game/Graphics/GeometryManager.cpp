/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Implementation of Geometry Manager
----------------------------------------------*/

#include "GeometryManager.h"
#include "CBufferStructs.h"

#include "COMException.h"
#include "ThrowMacros.h"
#include "../../System/PathMacros.h"

namespace Graphics {

GeometryManager::GeometryManager():
    m_pInputLayout(0),
    m_pBasicMaterial(0)
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

void GeometryManager::UpdateEntities(float dt)
{
    m_Entities[0]->GetTransform()->Rotate(0, dt, 0);
}

void GeometryManager::BuildMeshes(DeviceResources* a_DR)
{
    using namespace DirectX;
    // sample colors
    XMFLOAT4 white = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
    XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
    XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

    // Construct pyramid programatically
    Vertex verts[5];
    const float width = 1.0f;
    const float yPos = -0.5f;
    const float height = 1.0f;

    for (int i = 0; i < 4; i++)
    {
        float angle = (i * XM_PIDIV2) + XM_PIDIV4;
        float xPos = width / 2.0f * cosf(angle);
        float zPos = width / 2.0f * sinf(angle);

        verts[i].position = XMFLOAT3(xPos, yPos, zPos);
    }

    verts[4].position = XMFLOAT3(0.0f, height + yPos, 0.0f);

    //verts[0].color = blue;
    //verts[1].color = green;
    //verts[2].color = blue;
    //verts[3].color = green;
    //verts[4].color = red;


    int numVerts = ARRAYSIZE(verts);

    // Make index list
    UINT indices[] = {
        2, 4, 3,    // front
        3, 4, 0,    // right
        0, 4, 1,    // back
        1, 4, 2,    // left
        1, 2, 3,    // bottom1
        3, 0, 1     // bottom2
    };

    int numIndices = ARRAYSIZE(indices);

    // Make a mesh based on this geometry information
    m_Meshes.push_back(new Game::Mesh(MODELPATH("teapot.obj"), a_DR->GetD3DDevice()));

    // Make an entity based on the mesh and the material
    m_Entities.push_back(new Game::Entity(m_Meshes[0], m_pBasicMaterial));

    // For teapot
    m_Entities[0]->GetTransform()->SetRotation(-XM_PIDIV2, 0, 0);
    m_Entities[0]->GetTransform()->SetScale(0.1f, 0.1f, 0.1f);
    m_Entities[0]->GetTransform()->SetPosition(0.0f, -0.6f, 0.0f);
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

// Compiles Shaders and creates the basic material
void GeometryManager::CompileShaders(DeviceResources* DR)
{
    ID3D11Device* device = DR->GetD3DDevice();
    ID3D11DeviceContext* context = DR->GetD3DDeviceContext();

    Microsoft::WRL::ComPtr<ID3D10Blob> pBlob = 0;
    HRESULT hr;
    
    // Create Pixel Shader
    hr = D3DReadFileToBlob(L"..\\_Binary\\PixelShader.cso", &pBlob);
    if (FAILED(hr)) throw COM_EXCEPT(hr);
    hr = device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), 0, m_pPixelShader.GetAddressOf());
    if (FAILED(hr)) throw COM_EXCEPT(hr);

    // Create Vertex Shader
    hr = D3DReadFileToBlob(L"..\\_Binary\\VertexShader.cso", &pBlob);
    if (FAILED(hr)) throw COM_EXCEPT(hr);
    hr = device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), 0, m_pVertexShader.GetAddressOf());
    if (FAILED(hr)) throw COM_EXCEPT(hr);

    // Describe Input Layout
    const D3D11_INPUT_ELEMENT_DESC IED[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    
    // Create Input Layout and hold as a member field
    hr = device->CreateInputLayout(
        IED,
        (UINT)std::size(IED),
        pBlob->GetBufferPointer(),
        pBlob->GetBufferSize(),
        &m_pInputLayout);

    if (FAILED(hr)) throw COM_EXCEPT(hr);

    // Bind input layout to Input Assembler Stage
    context->IASetInputLayout(m_pInputLayout.Get());

    // Set Primitive Topology of the input assembler stage
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create material based on compiled vertex/pixel shaders
    DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(0.75f, 0.75f, 0.75f, 1.0f);
    float spec = 64.0f;

    m_pBasicMaterial = new Material(m_pVertexShader.Get(), m_pPixelShader.Get(), color, spec);

}

}