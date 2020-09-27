/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Implementation of Shader wrapper
----------------------------------------------*/
#include "Shader.h"

#include "Texture.h"
#include "../../System/PathMacros.h"

#if defined(DEBUG)
#include "DXCore.h"
#include "COMException.h"
#include "ThrowMacros.h"
#endif

#include <vector>
#pragma warning(disable: 6385)

namespace Graphics {

#pragma region Vertex Shader
VertexShader::VertexShader(const wchar_t* path, ID3D11Device* device) :
    Shader(path, device)
{}

VertexShader::~VertexShader()
{
    mpInputLayout->Release();
    mpVertexShader->Release();
}

// Use the device to create the ID3D11VertexShader
void VertexShader::CreateDXShader(ID3D10Blob* pBlob, ID3D11Device* device)
{
    device->CreateVertexShader(pBlob->GetBufferPointer(), 
        pBlob->GetBufferSize(), 
        nullptr, 
        &mpVertexShader);

    #if defined(DEBUG)
	static const char debugShaderName[] = "VS_Shader";
    HRESULT hr = mpVertexShader->SetPrivateData(WKPDID_D3DDebugObjectName, ARRAYSIZE(debugShaderName) - 1, debugShaderName);
    if (FAILED(hr)) throw COM_EXCEPT(hr);
    #endif
}

void VertexShader::BuildReflectionFields(ID3D11ShaderReflection* pReflection, ID3D10Blob* pBlob, ID3D11Device* device)
{
    BuildInputLayout(pReflection, pBlob, device);
}

// Read the reflection to create an input layout dynamically
void VertexShader::BuildInputLayout(ID3D11ShaderReflection* pReflection, ID3D10Blob* pBlob, ID3D11Device* device)
{
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
    HRESULT hr = device->CreateInputLayout(&inputElements[0], inputElements.size(), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &mpInputLayout);
    if (FAILED(hr)) throw COM_EXCEPT(hr);

	#if defined(DEBUG)
	static const char debugNameIL[] = "VS_InputLayout";
	hr = mpInputLayout->SetPrivateData(WKPDID_D3DDebugObjectName, ARRAYSIZE(debugNameIL) - 1, debugNameIL);
	if (FAILED(hr)) throw COM_EXCEPT(hr);
	#endif
}

// Bind all the underlying constant buffers
void VertexShader::BindConstantBuffers(ID3D11DeviceContext* context) const
{
    context->VSSetConstantBuffers(0, 13, mConstantBuffers);
}

void VertexShader::BindShader(ID3D11DeviceContext* context) const
{
    // Set Input Layout, then bind the shader
    context->IASetInputLayout(mpInputLayout);
    context->VSSetShader(mpVertexShader, nullptr, 0);
}

#pragma endregion

#pragma region Pixel Shader
PixelShader::PixelShader(const wchar_t* path, ID3D11Device* device) :
    Shader(path, device)
{}

PixelShader::~PixelShader()
{
    mpPixelShader->Release();
}

void PixelShader::CreateDXShader(ID3D10Blob* pBlob, ID3D11Device* device)
{
    device->CreatePixelShader(pBlob->GetBufferPointer(), 
        pBlob->GetBufferSize(), 
        nullptr, 
        &mpPixelShader);

	#if defined(DEBUG)
	static const char debugShaderName[] = "PS_Shader";
	HRESULT hr = mpPixelShader->SetPrivateData(WKPDID_D3DDebugObjectName, ARRAYSIZE(debugShaderName) - 1, debugShaderName);
	if (FAILED(hr)) throw COM_EXCEPT(hr);
	#endif
}

void PixelShader::BuildReflectionFields(ID3D11ShaderReflection* pReflection, ID3D10Blob* pBlob, ID3D11Device* device)
{
    // In the future, building samplers and SRVs may be needed
}


// PS Specific bind function
void PixelShader::BindConstantBuffers(ID3D11DeviceContext* context) const
{
    context->PSSetConstantBuffers(0, 13, mConstantBuffers);
}

void PixelShader::BindShader(ID3D11DeviceContext* context) const
{
    context->PSSetShader(mpPixelShader, nullptr, 0);
}

void PixelShader::SetTexture(ID3D11DeviceContext* context, Texture* texture)
{
    UINT slot = static_cast<UINT>(texture->GetType());
    ID3D11ShaderResourceView* pSRV = texture->GetSRV();
    context->PSSetShaderResources(slot, 1, &pSRV );
}

#pragma endregion

}