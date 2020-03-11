/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Wrapper for Constant Buffer functionality
----------------------------------------------*/
#ifndef CONSTANTBUFFER_H
#define CONSTANTBUFFER_H

#include "DXCore.h"
#include "IBindable.h"
#include "ThrowMacros.h"

#include <wrl/client.h> // ComPtr

namespace Graphics {

class ConstantBuffer : public IBindable
{
public:
    // Base Class Constructor 
    ConstantBuffer(ID3D11Device* device, UINT size, UINT slot = 0u, void* pData = nullptr) :
        m_Slot(slot)
    {
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

        // Create Subresource data and a pointer to point to it.
        D3D11_SUBRESOURCE_DATA subData = {};
        subData.pSysMem = pData;
        D3D11_SUBRESOURCE_DATA* pSubData = &subData;

        // Make sure null is passed in if there is no initial data
        if(pData == nullptr) pSubData = nullptr;

        // call on device to create the buffer
        HRESULT hr = device->CreateBuffer(&cbDesc, pSubData, m_pBuffer.GetAddressOf());
    }

    virtual ~ConstantBuffer() = default;

    inline void SetData(ID3D11DeviceContext* context, UINT size, void* pData)
    {
        D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
        context->Map(m_pBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
        memcpy(mappedBuffer.pData, pData, size);
        context->Unmap(m_pBuffer.Get(), 0);
    }

    // Must be overloaded by VSConstantBuffer, PSConstantBuffer, etc
    virtual inline void Bind(ID3D11DeviceContext* context) = 0;

    // Accessor for slot number
    inline UINT GetSlot() { return m_Slot; }

protected:
    // Owning pointer holds the actual d3d resource
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_pBuffer;

    // Register slot for this cbuffer
    UINT m_Slot;

public: // Delete all default operators and constructors
    ConstantBuffer()                                    = delete;
    ConstantBuffer(const ConstantBuffer&)               = delete;
    ConstantBuffer& operator=(const ConstantBuffer&)    = delete;
};

class VSConstantBuffer : public ConstantBuffer
{
public:
    // CBuffer Constructor only requires a device and a size to create
    // Slot is b0 by default, and data is not passed in by default.
    VSConstantBuffer(ID3D11Device* device, UINT size, UINT slot = 0u, void* pData = nullptr) :
        ConstantBuffer(device, size, slot, pData)
    {}

    // Binds the cbuffer to the vertex shader, this function is the only real reason for this class' existence
    inline void Bind(ID3D11DeviceContext* context) noexcept override
    {
        context->VSSetConstantBuffers(m_Slot, 1u, m_pBuffer.GetAddressOf());
    }

public: // Delete all default operators and constructors
    VSConstantBuffer()                                      = delete;
    VSConstantBuffer(const VSConstantBuffer&)               = delete;
    VSConstantBuffer& operator=(const VSConstantBuffer&)    = delete;
};


class PSConstantBuffer : public ConstantBuffer
{
public:
    // CBuffer Constructor only requires a device and a size to create
    // Slot is b0 by default, and data is not passed in by default.
    PSConstantBuffer(ID3D11Device* device, UINT size, UINT slot = 0u, void* pData = nullptr) :
        ConstantBuffer(device, size, slot, pData)
    {}

    // Binds the cbuffer to the pixel shader, this function is the only real reason for this class' existence
    inline void Bind(ID3D11DeviceContext* context) noexcept override
    {
        context->PSSetConstantBuffers(m_Slot, 1u, m_pBuffer.GetAddressOf());
    }

public: // Delete all default operators and constructors
    PSConstantBuffer()                                      = delete;
    PSConstantBuffer(const PSConstantBuffer&)               = delete;
    PSConstantBuffer& operator=(const PSConstantBuffer&)    = delete;
};


}

#endif