/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Implementation of Shader wrapper
----------------------------------------------*/
#include "Shader.h"
#include "COMException.h"
#include "../../System/PathMacros.h"

#pragma warning(disable: 6385)

namespace Graphics {

#pragma region Vertex Shader
VertexShader::VertexShader(const wchar_t* path, ID3D11Device* device):
    m_Path(path)
{
    // Zero the memory to be used for constant buffers. 
    SecureZeroMemory((void*)m_pCBuffers, sizeof(VSConstantBuffer*)*14u);

    // Create space for the shader
    Microsoft::WRL::ComPtr<ID3D10Blob> pBlob;
    HRESULT hr = D3DReadFileToBlob(path, pBlob.GetAddressOf());
    if(FAILED(hr)) throw COM_EXCEPT(hr);

    // Use the device to create the shader
    device->CreateVertexShader(pBlob->GetBufferPointer(), 
        pBlob->GetBufferSize(), 
        nullptr, 
        m_pVertexShader.GetAddressOf());
}

VertexShader::~VertexShader()
{
    for (auto cbuffer : m_pCBuffers)
    {
        if(cbuffer)
            delete cbuffer;
    }
}

void VertexShader::Bind(ID3D11DeviceContext* context) noexcept
{
    // Temporary, in the future should hold numCBuffers to avoid having to check all of them.
    // We can assume unused cbuffer slots are null due to the call to ZeroMemory in the constructor.
    for (unsigned int i = 0; i < 14u; i++)
    {
        if(m_pCBuffers[i])
            m_pCBuffers[i]->Bind(context);
    }

    context->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
}

void VertexShader::AddConstantBuffer(VSConstantBuffer* pCBuffer)
{
    UINT slot = pCBuffer->GetSlot();

    // if overriding a constant buffer, must make sure to free that memory
    if (m_pCBuffers[slot] != nullptr)
        delete m_pCBuffers[slot];

    // else, just add the passed cbuffer to that slot
    m_pCBuffers[slot] = pCBuffer;
}

bool VertexShader::SetBufferData(ID3D11DeviceContext* context, UINT slot, UINT size, void* pData)
{
    if (slot > 14u)
        return false;

    m_pCBuffers[slot]->SetData(context, size, pData);
    return true;
}

ID3D11VertexShader* VertexShader::GetShader()
{
    return m_pVertexShader.Get();
}
#pragma endregion

#pragma region Pixel Shader
PixelShader::PixelShader(const wchar_t* path, ID3D11Device* device):
    m_Path(path)
{
    // Zero the memory to be used for constant buffers. 
    SecureZeroMemory((void*)m_pCBuffers, sizeof(PSConstantBuffer*)*14u);

    // Create space for the shader
    Microsoft::WRL::ComPtr<ID3D10Blob> pBlob;
    HRESULT hr = D3DReadFileToBlob(path, pBlob.GetAddressOf());
    if(FAILED(hr)) throw COM_EXCEPT(hr);

    // Use the device to create the shader
    device->CreatePixelShader(pBlob->GetBufferPointer(), 
        pBlob->GetBufferSize(), 
        nullptr, 
        m_pPixelShader.GetAddressOf());
}

PixelShader::~PixelShader()
{
    for (auto cbuffer : m_pCBuffers)
    {
        if(cbuffer)
            delete cbuffer;
    }
}

void PixelShader::Bind(ID3D11DeviceContext* context) noexcept
{
    // Temporary, in the future should hold numCBuffers to avoid having to check all of them.
    for (unsigned int i = 0; i < 14u; i++)
    {
        if(m_pCBuffers[i])
            m_pCBuffers[i]->Bind(context);
    }

    context->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
}

void PixelShader::AddConstantBuffer(PSConstantBuffer* pCBuffer)
{
    UINT slot = pCBuffer->GetSlot();

    // if overriding a constant buffer, must make sure to free that memory
    if (m_pCBuffers[slot] != nullptr)
        delete m_pCBuffers[slot];

    // else, just add the passed cbuffer to that slot
    m_pCBuffers[slot] = pCBuffer;
}

bool PixelShader::SetBufferData(ID3D11DeviceContext* context, UINT slot, UINT size, void* pData)
{
    if (slot > 14u)
        return false;

    m_pCBuffers[slot]->SetData(context, size, pData);
    return true;
}

ID3D11PixelShader* PixelShader::GetShader()
{
    return m_pPixelShader.Get();
}

#pragma endregion
}