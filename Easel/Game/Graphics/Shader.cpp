/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Implementation of Shader wrapper
----------------------------------------------*/
#include "Shader.h"
#include "COMException.h"
#include "../../System/PathMacros.h"

namespace Graphics {

#pragma region Vertex Shader
VertexShader::VertexShader(const wchar_t* path, ID3D11Device* device):
    m_Path(path)
{
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

void VertexShader::Bind(ID3D11DeviceContext* context) noexcept
{
    context->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
}

UINT VertexShader::AddConstantBuffer(VSConstantBuffer* pCBuffer)
{
    m_pCBuffers.push_back(pCBuffer);
    return m_pCBuffers.size() - 1u;
}

bool VertexShader::SetBufferData(ID3D11DeviceContext* context, UINT slot, UINT size, void* pData)
{
    if (slot > m_pCBuffers.size())
        return false;

    m_pCBuffers.at(slot)->SetData(context, size, pData);
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

void PixelShader::Bind(ID3D11DeviceContext* context) noexcept
{
    context->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
}

UINT PixelShader::AddConstantBuffer(PSConstantBuffer* pCBuffer)
{
    m_pCBuffers.push_back(pCBuffer);
    return m_pCBuffers.size() - 1u;
}

bool PixelShader::SetBufferData(ID3D11DeviceContext* context, UINT slot, UINT size, void* pData)
{
    if (slot > m_pCBuffers.size())
        return false;

    m_pCBuffers.at(slot)->SetData(context, size, pData);
    return true;
}
ID3D11PixelShader* PixelShader::GetShader()
{
    return m_pPixelShader.Get();
}
#pragma endregion
}