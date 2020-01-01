/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Implementation of Geometry Manager
----------------------------------------------*/

#include "GeometryManager.h"

namespace Graphics {

GeometryManager::GeometryManager():
    m_pVertexBuffer(0),
    m_pVertexShader(0),
    m_pPixelShader(0)
{}

GeometryManager::GeometryManager(const GeometryManager& other):
    m_pVertexBuffer(other.m_pVertexBuffer),
    m_pVertexShader(other.m_pVertexShader),
    m_pPixelShader(other.m_pPixelShader)
{}

GeometryManager& GeometryManager::operator=(const GeometryManager& other)
{
    m_pPixelShader = other.m_pPixelShader;
    m_pVertexShader = other.m_pVertexShader;
    m_pVertexBuffer = other.m_pVertexBuffer;

    return (GeometryManager&) other;
}

void GeometryManager::Initialize(DeviceResources* DR)
{
    BuildVertexBuffers(DR);
    CompileShaders(DR);
    
}

void GeometryManager::BuildVertexBuffers(DeviceResources* DR)
{
    // Build Vertex Array
    const Vertex vertices[] = {
        {{0.0f, 0.5f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}},
        {{-0.5f, -0.5f, 0.0f}}
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
    hr = D3DReadFileToBlob(L"..\\_Binary\\VertexShader.cso", pBlob.GetAddressOf());
    ThrowIfFailed(hr);
    hr = device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), 0, &m_pVertexShader);
    ThrowIfFailed(hr);

    // Bind Vertex Shader
    context->VSSetShader(m_pVertexShader.Get(), NULL, 0U);

    // Describe Input Layout
    const D3D11_INPUT_ELEMENT_DESC IED[] =
    {
        {"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0,0, D3D11_INPUT_PER_VERTEX_DATA, 0},
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