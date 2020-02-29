/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Implementation of LightingManager.h
----------------------------------------------*/
#include "LightingManager.h"

namespace Graphics
{
    LightingManager::LightingManager(ID3D11Device* a_pDevice)
    {
        InitLights();
        CreateLightBuffer(a_pDevice);
    }

    LightingManager::~LightingManager()
    {
        delete m_pLightBuffer;
    }

    void LightingManager::Update(ID3D11DeviceContext* a_pContext, float dt)
    {
        UpdateLights(dt);
        BindLightBuffer(a_pContext);
    }

    void LightingManager::InitLights()
    {
        using DirectX::XMFLOAT3A;
        m_pLightBuffer = new LightBuffer();

        SetAmbient(XMFLOAT3A(0.12f, 0.12f, 0.15f));

        m_pLightBuffer->directionalLight.diffuseColor   = XMFLOAT3A(+1.0f, +0.772f, +0.56f); // sunlight
        m_pLightBuffer->directionalLight.toLight        = XMFLOAT3A(-1.0f, +0.7f, +0.3f);

        m_NeedsRebind = true;
    }

    void LightingManager::UpdateLights(float dt)
    {
        // Logic to update the light's direction or something
        //DirectionalLight& light = m_pLightBuffer->directionalLight;
        //light.toLight.x = cosf(dt);
        //light.toLight.z = sinf(dt);
        //
        //m_NeedsRebind = true;
    }

    void LightingManager::CreateLightBuffer(ID3D11Device* a_pDevice)
    {
        // Get the size of the constant buffer struct as a multiple of 16
        const unsigned int size = sizeof(LightBuffer);
        static_assert(size % 16 == 0, "Constant Buffer size must be alignable on a 16-byte boundary");

        // Describe the constant buffer and create it
        D3D11_BUFFER_DESC cbDesc = {};
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.ByteWidth = size;
        cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cbDesc.Usage = D3D11_USAGE_DYNAMIC;
        cbDesc.MiscFlags = 0;
        cbDesc.StructureByteStride = 0;

        // Store the cbuffer as a member field
        a_pDevice->CreateBuffer(&cbDesc, 0, m_cbLights.GetAddressOf());
    }

    void LightingManager::BindLightBuffer(ID3D11DeviceContext* a_pContext)
    {
        // Only bind if any data has been changed
        if (!NeedsRebind())
            return;

        // Copy this data to the constant buffer we intend to use
        D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
        a_pContext->Map(m_cbLights.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);

        // Straight memcpy() into the resource
        memcpy(mappedBuffer.pData, m_pLightBuffer, sizeof(LightBuffer));

        // Unmap so the GPU can once again use the buffer
        a_pContext->Unmap(m_cbLights.Get(), 0);

        // Bind the buffer to the pipeline
        a_pContext->PSSetConstantBuffers
        (
            0,
            1,
            m_cbLights.GetAddressOf()
        );

        // Just bound, no need to rebind until next data change in UpdateLights
        m_NeedsRebind = false;
    }

}