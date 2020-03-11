/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Implementation of Shader wrapper
----------------------------------------------*/
#include "Shader.h"
#include <vector>
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
    if (FAILED(hr)) throw COM_EXCEPT(hr);

    // Use the device to create the shader
    device->CreateVertexShader(pBlob->GetBufferPointer(), 
        pBlob->GetBufferSize(), 
        nullptr, 
        m_pVertexShader.GetAddressOf());

    // Use the compiled blob to programmatically build an input layout using shader reflection
    ID3D11ShaderReflection* pReflection = nullptr;
    hr = D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**) &pReflection);
    if (FAILED(hr)) throw COM_EXCEPT(hr);

    // Get a shader description
    D3D11_SHADER_DESC shaderDesc;
    pReflection->GetDesc(&shaderDesc);

    // Read each input parameter into a std::vector
    std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
    for (unsigned int i = 0; i < shaderDesc.InputParameters; ++i)
    {
        // Grab a description of the parameter at this index
        D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
        pReflection->GetInputParameterDesc(i, &paramDesc);

        // Fill out input element
        D3D11_INPUT_ELEMENT_DESC inputParam = {};
        inputParam.SemanticName  = paramDesc.SemanticName;
        inputParam.SemanticIndex = paramDesc.SemanticIndex;
        inputParam.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        inputParam.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

        // determine DXGI format ... Thanks MSDN!
        if ( paramDesc.Mask == 1 )
        {
            if      ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 )    inputParam.Format = DXGI_FORMAT_R32_UINT;
            else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 )    inputParam.Format = DXGI_FORMAT_R32_SINT;
            else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 )   inputParam.Format = DXGI_FORMAT_R32_FLOAT;
        }
        else if ( paramDesc.Mask <= 3 )
        {
            if      ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 )    inputParam.Format = DXGI_FORMAT_R32G32_UINT;
            else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 )    inputParam.Format = DXGI_FORMAT_R32G32_SINT;
            else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 )   inputParam.Format = DXGI_FORMAT_R32G32_FLOAT;
        }
        else if ( paramDesc.Mask <= 7 )
        {
            if      ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 )    inputParam.Format = DXGI_FORMAT_R32G32B32_UINT;
            else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 )    inputParam.Format = DXGI_FORMAT_R32G32B32_SINT;
            else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 )   inputParam.Format = DXGI_FORMAT_R32G32B32_FLOAT;
        }
        else if ( paramDesc.Mask <= 15 )
        {
            if      ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32 )    inputParam.Format = DXGI_FORMAT_R32G32B32A32_UINT;
            else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32 )    inputParam.Format = DXGI_FORMAT_R32G32B32A32_SINT;
            else if ( paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 )   inputParam.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        }

        inputElements.push_back(inputParam);
    }

    // Finally, try to create the input layout, storing it as a member if successful
    hr = device->CreateInputLayout(&inputElements[0], inputElements.size(), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), m_pInputLayout.GetAddressOf());
    if (FAILED(hr)) throw COM_EXCEPT(hr);
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
    // Make sure the right input layout is being used:
    context->IASetInputLayout(m_pInputLayout.Get());

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