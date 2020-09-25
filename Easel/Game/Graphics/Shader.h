/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Wrapper for Vertex/Pixel/other shader code
----------------------------------------------*/
#ifndef SHADER_H
#define SHADER_H

#include "DXCore.h"
#include "ConstantBuffer.h"

#include <string>
#include <unordered_map>

#include "COMException.h"
#include "ThrowMacros.h"

namespace Graphics
{
class Texture;

}

namespace Graphics {

// A Shader Interface using CRTP
template<class ShaderType>
class Shader
{
public:
    Shader(const wchar_t* path, ID3D11Device* device)
    {
        HRESULT hr = E_FAIL;

        // Store file in Blob
        ID3D10Blob* pBlob;
        hr = D3DReadFileToBlob(path, &pBlob);
        if (FAILED(hr)) throw COM_EXCEPT(hr);

        // Create Shader using device
        this->Impl().CreateDXShader(pBlob, device);

        // Use Blob to create a Reflection
        ID3D11ShaderReflection* pReflection = nullptr;
        hr = D3DReflect(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&pReflection));
        if (FAILED(hr)) throw COM_EXCEPT(hr);

        // Initialize Constant Buffer Array
        BuildConstantBuffers(pReflection, device);

        // Use Reflection to create shader-specific fields (input layout, shader resources, so on)
        this->Impl().BuildReflectionFields(pReflection, pBlob, device);

        // Cleanup COM Objects
        pBlob->Release();
        pReflection->Release();
    }

    virtual ~Shader()
    {
        // Cleanup any of the ID3D11Buffers that exist
        for (UINT i = 0; i < (UINT) ReservedRegisters::RR_MAX_INDEX + 1; i++)
        {
            if(mConstantBuffers[i])
                mConstantBuffers[i]->Release();
        }
    }

    // Implementation-specific bind function
    void Bind(ID3D11DeviceContext* context) const
    {
        this->Impl().BindConstantBuffers(context);
        this->Impl().BindShader(context);
    }

    // Sets the data for a constant buffer by index
    bool SetBufferData(ID3D11DeviceContext* context, UINT slot, UINT dataSize, const void* pData)
    {
        if(slot > (UINT)ReservedRegisters::RR_MAX_INDEX) return false;

        ID3D11Buffer* buffer = mConstantBuffers[slot];
        
        if(!buffer)
            return false;

        // --Map/Memcpy/Unmap--
        D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
        HRESULT hr = context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
        memcpy(mappedBuffer.pData, pData, dataSize);
        context->Unmap(buffer, 0);

        if(FAILED(hr)) return false;
        return true;
    }


private:
    // Read the reflection to initialize constant buffers with proper name/sizes
    void BuildConstantBuffers(ID3D11ShaderReflection* pReflection, ID3D11Device* device)
    {
        // Get a description of the shader
        D3D11_SHADER_DESC shaderDesc;
        pReflection->GetDesc(&shaderDesc);

        // Ensure all array data is 0
        ZeroMemory(mConstantBuffers, sizeof(ID3D11Buffer*) * (UINT) ReservedRegisters::RR_MAX_INDEX + 1);

        // Create and initialize every cbuffer in the array
        for (int b = 0; b < shaderDesc.ConstantBuffers; ++b)
        {
            // Get buffer and description
            ID3D11ShaderReflectionConstantBuffer* cb = pReflection->GetConstantBufferByIndex(b);
            D3D11_SHADER_BUFFER_DESC bufferDesc;
            cb->GetDesc(&bufferDesc);

            // Binding details
            D3D11_SHADER_INPUT_BIND_DESC bindDesc;
            pReflection->GetResourceBindingDescByName(bufferDesc.Name, &bindDesc);
            
            // Initialization arguments
            UINT size = bufferDesc.Size;
            UINT slot = bindDesc.BindPoint;

            // Ensure cbuffer is allocated on a 16-byte boundary
            assert(size % 16u == 0);

            // Create cbuffer description from necessary size
            D3D11_BUFFER_DESC cbDesc = {};
            cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            cbDesc.ByteWidth = size;
            cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            cbDesc.Usage = D3D11_USAGE_DYNAMIC;
            cbDesc.MiscFlags = 0;
            cbDesc.StructureByteStride = 0;

            // Create the cbuffer
            ID3D11Buffer* rawBuffer;
            HRESULT hr = device->CreateBuffer(&cbDesc, nullptr, &rawBuffer);
            if (FAILED(hr)) throw COM_EXCEPT(hr);

            // Store in contiguous list
            mConstantBuffers[slot] = rawBuffer;
        }
    }

    // CRTP Specific Helpers
    inline ShaderType&        Impl()       { return static_cast<ShaderType&>(*this);        }
    inline ShaderType  const& Impl() const { return static_cast<ShaderType const&>(*this);  }
    
protected:
    // At load-time: fill this with constant buffer info from Reflection
    ID3D11Buffer*    mConstantBuffers[(UINT)ReservedRegisters::RR_MAX_INDEX + 1];

public: // Enforce use of path constructor
    Shader()                            = delete;
    Shader(const Shader&)               = delete;
    Shader& operator=(const Shader&)    = delete;
};

class VertexShader : public Shader<VertexShader>
{
friend class Shader<VertexShader>;

public:
    VertexShader(const wchar_t* path, ID3D11Device* device);
    ~VertexShader();

private:
    // Initialization helpers for shader and input layout
    void CreateDXShader(ID3D10Blob* pBlob, ID3D11Device* device);
    void BuildReflectionFields(ID3D11ShaderReflection* pReflection, ID3D10Blob* pBlob, ID3D11Device* device);
    void BuildInputLayout(ID3D11ShaderReflection* pReflection, ID3D10Blob* pBlob, ID3D11Device* device);

    // Bind helpers for cbuffers, input layout, shader
    void BindConstantBuffers(ID3D11DeviceContext* context) const;
    void BindShader(ID3D11DeviceContext* context) const;

    ID3D11VertexShader* mpVertexShader;
    ID3D11InputLayout* mpInputLayout;
};

class PixelShader : public Shader<PixelShader>
{
friend class Shader<PixelShader>;

public:
    PixelShader(const wchar_t* path, ID3D11Device* device);
    ~PixelShader();
    
    // PS-Specific Bind function, Texture knows how to set itself
    void SetTexture(ID3D11DeviceContext* context, Texture* texture);

private:
    // Use device to create a pixel shader
    void CreateDXShader(ID3D10Blob* pBlob, ID3D11Device* device);

    // Builds the sampler, textures, or other resources (todo)
    void BuildReflectionFields(ID3D11ShaderReflection* pReflection, ID3D10Blob* pBlob, ID3D11Device* device);

    // Bind helpers for Pixel shader
    void BindConstantBuffers(ID3D11DeviceContext* context) const;
    void BindShader(ID3D11DeviceContext* context) const;

    ID3D11PixelShader* mpPixelShader;
};

}

#endif