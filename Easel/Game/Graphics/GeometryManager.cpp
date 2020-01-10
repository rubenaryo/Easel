/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Implementation of Geometry Manager
----------------------------------------------*/

#include "GeometryManager.h"

namespace Graphics {

GeometryManager::GeometryManager():
    m_pVertexBuffer(0),
    m_pIndexBuffer(0),
    m_NumVertices(0),
    m_pVertexShader(0),
    m_pPixelShader(0)
{}

GeometryManager::GeometryManager(const GeometryManager& other):
    m_pVertexBuffer(other.m_pVertexBuffer),
    m_pIndexBuffer(other.m_pIndexBuffer),
    m_NumVertices(other.m_NumVertices),
    m_pVertexShader(other.m_pVertexShader),
    m_pPixelShader(other.m_pPixelShader)
{}

GeometryManager& GeometryManager::operator=(const GeometryManager& other)
{
    m_pPixelShader = other.m_pPixelShader;
    m_pVertexShader = other.m_pVertexShader;
    m_pVertexBuffer = other.m_pVertexBuffer;
    m_pIndexBuffer = other.m_pIndexBuffer;
    m_NumVertices = other.m_NumVertices;

    return (GeometryManager&) other;
}

void GeometryManager::Initialize(DeviceResources* DR)
{
    BuildVertexBuffers(DR);
    BuildIndexBuffers(DR);
    CompileShaders(DR);
}

void GeometryManager::BuildVertexBuffers(DeviceResources* DR)
{
    // Build Vertex Array
    const Vertex vertices[] = {
        {{0.0f, 0.0f, 0.0f},   {1.f, 1.f, 1.0f}},    // center
        {{0.0f, 0.5f, 0.0f},   {0.f, 1.f, 1.0f}},    
        {{0.5f, -0.5f, 0.0f},  {0.5f, 0.5f, 0.0f}},
        {{-0.5f, -0.5f, 0.0f}, {0.3f, 0.5f, 1.f}}
    };

    // Hold the number of vertices as a member field
    UINT numVerts = (UINT)std::size(vertices);
    m_NumVertices = numVerts;
    
    // Create Descriptors
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0u;
    bd.MiscFlags = 0u;
    bd.ByteWidth = sizeof(vertices);
    bd.StructureByteStride = sizeof(Vertex);
    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = vertices;
    
    // Pipe data into member field
    ThrowIfFailed(DR->GetD3DDevice()->CreateBuffer(&bd, &sd, &m_pVertexBuffer));

}

void GeometryManager::BuildIndexBuffers(DeviceResources* a_DR)
{
    // Make index list
    const UINT indices[] = {
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

    // Create Descriptors
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0u;
    bd.MiscFlags = 0u;
    bd.ByteWidth = sizeof(indices);
    bd.StructureByteStride = sizeof(UINT);
    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = indices;

    // Pipe Data into member field
    ThrowIfFailed(a_DR->GetD3DDevice()->CreateBuffer(&bd, &sd, &m_pIndexBuffer));
}

// Programmatically Compiles and then Binds Shaders to the Render pipeline.
void GeometryManager::CompileShaders(DeviceResources* DR)
{
    using Microsoft::WRL::ComPtr;

    ID3D11Device* device = DR->GetD3DDevice();
    ID3D11DeviceContext* context = DR->GetD3DDeviceContext();

    ComPtr<ID3D10Blob> pBlob = 0;
    HRESULT hr;
    
    // Create Pixel Shader
    hr = D3DReadFileToBlob(L"..\\_Binary\\PixelShader.cso", &pBlob);
    ThrowIfFailed(hr);
    hr = device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), 0, &m_pPixelShader);
    ThrowIfFailed(hr);

    // Bind Pixel Shader
    context->PSSetShader(m_pPixelShader.Get(), NULL, 0U);

    // Create Vertex Shader
    hr = D3DReadFileToBlob(L"..\\_Binary\\VertexShader.cso", &pBlob);
    ThrowIfFailed(hr);
    hr = device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), 0, &m_pVertexShader);
    ThrowIfFailed(hr);

    // Bind Vertex Shader
    context->VSSetShader(m_pVertexShader.Get(), NULL, 0U);

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
}

}