/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Implementation of LightingManager.h
----------------------------------------------*/
#include "LightingManager.h"

namespace Graphics
{
    LightingManager::LightingManager(ID3D11Device* a_pDevice, DirectX::XMFLOAT3 a_CameraPos)
    {
        InitLights(a_CameraPos);
        CreateLightBuffer(a_pDevice);
    }

    LightingManager::~LightingManager()
    {
        delete m_pLightingBuffer;
    }

    void LightingManager::Update(ID3D11DeviceContext* a_pContext, float dt, DirectX::XMFLOAT3 a_CameraPos)
    {
        UpdateLights(dt, a_CameraPos);
        BindLightBuffer(a_pContext);
    }


    // AAA Case: Bring in lights directly from a "world editor" of some sort, which exports light positions, colors, etc for environment artists
    // Me: Hardcode it!
    void LightingManager::InitLights(DirectX::XMFLOAT3 a_CameraPos)
    {
        using DirectX::XMFLOAT3A;

        SetAmbient(XMFLOAT3A(0.12f, 0.12f, 0.15f));

        m_LightData.directionalLight.diffuseColor   = XMFLOAT3A(+1.0f, +0.772f, +0.56f); // sunlight
        m_LightData.directionalLight.toLight        = XMFLOAT3A(-1.0f, +0.7f, +0.3f);

        // hold camera position in light data
        m_LightData.cameraWorldPos.x = a_CameraPos.x;
        m_LightData.cameraWorldPos.y = a_CameraPos.y;
        m_LightData.cameraWorldPos.z = a_CameraPos.z;

        m_NeedsRebind = true;
    }

    void LightingManager::UpdateLights(float dt, DirectX::XMFLOAT3 a_CameraPos)
    {
        // Logic to update the light's direction or something
        DirectionalLight& light = m_LightData.directionalLight;
        light.toLight.x = cosf(dt);
        light.toLight.z = sinf(dt);

        // Overwrite held camera position
        m_LightData.cameraWorldPos.x = a_CameraPos.x;
        m_LightData.cameraWorldPos.y = a_CameraPos.y;
        m_LightData.cameraWorldPos.z = a_CameraPos.z;
        
        m_NeedsRebind = true;
    }

    void LightingManager::CreateLightBuffer(ID3D11Device* a_pDevice)
    {
        m_pLightingBuffer = new PSConstantBuffer(a_pDevice, sizeof(cbLighting), c_PSLighingSlot);
    }

    void LightingManager::BindLightBuffer(ID3D11DeviceContext* a_pContext)
    {
        // Only bind if any data has been changed
        if (!NeedsRebind())
            return;

        // Set data through constant buffer interface
        m_pLightingBuffer->SetData(a_pContext, sizeof(cbLighting), (void*) &m_LightData);

        // Bind Light buffer
        m_pLightingBuffer->Bind(a_pContext);

        // Just bound, no need to rebind until next data change in UpdateLights
        m_NeedsRebind = false;
    }

}