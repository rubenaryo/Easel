#ifndef CONSTANTBUFFER_H
#define CONSTANTBUFFER_H

#include "DXCore.h"
#include "RenderingParams.h"

#include "ThrowMacros.h"

namespace Rendering {

struct ConstantBufferBindPacket
{
    ID3D11Buffer* Buffer;
    UINT          ByteSize;
    UINT          BindSlot;
    UINT          ShaderStage; // EASEL_SHADER_STAGE type
    BOOL          Stale;
};

typedef void (*BindFunction)(ID3D11DeviceContext* context, UINT slot, ID3D11Buffer*const* cbuffer);
static const BindFunction kBindFunctions[(UINT)EASEL_SHADER_STAGE::ESS_COUNT] = 
{
    [](ID3D11DeviceContext* context, UINT slot, ID3D11Buffer*const* cbuffer) -> void { context->VSSetConstantBuffers(slot, 1, cbuffer); },
    [](ID3D11DeviceContext* context, UINT slot, ID3D11Buffer*const* cbuffer) -> void { context->PSSetConstantBuffers(slot, 1, cbuffer); }
};

struct ConstantBufferUpdateManager
{
    static void Populate(UINT byteSize, UINT slot, EASEL_SHADER_STAGE shaderStage, ID3D11Device* device, ConstantBufferBindPacket* out_packet)
    {
        // Buffer description
        D3D11_BUFFER_DESC dynamicDesc = {0};
        dynamicDesc.Usage = D3D11_USAGE_DYNAMIC;
        dynamicDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        dynamicDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        dynamicDesc.MiscFlags = 0;
        dynamicDesc.StructureByteStride = 0;
        dynamicDesc.ByteWidth = byteSize;
        
        // Populate fields
        ConstantBufferBindPacket cbp;
        COM_EXCEPT(device->CreateBuffer(&dynamicDesc, nullptr, &cbp.Buffer));
        cbp.ByteSize    = byteSize;
        cbp.BindSlot    = slot;
        cbp.Stale       = true;
        cbp.ShaderStage = (UINT)shaderStage;

        *out_packet     = cbp;
    }

    static void MapUnmap(ConstantBufferBindPacket* packet, void* newData, ID3D11DeviceContext* context)
    {
        D3D11_MAPPED_SUBRESOURCE mappedBuffer = {0};

        COM_EXCEPT(context->Map(packet->Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer));
        memcpy(mappedBuffer.pData, newData, packet->ByteSize);
        context->Unmap(packet->Buffer, 0);

        packet->Stale = false;
    }

    static void Bind(ConstantBufferBindPacket* packet, ID3D11DeviceContext* context)
    {
        kBindFunctions[packet->ShaderStage](context, packet->BindSlot, &packet->Buffer);
    }

    static void Cleanup(ConstantBufferBindPacket* packet)
    {
        packet->Buffer->Release();
    }
};




}

#endif