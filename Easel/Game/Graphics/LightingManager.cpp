/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Implementation of LightingManager.h
----------------------------------------------*/
#include "LightingManager.h"

#include "LightStructs.h"

namespace Graphics
{
    LightingManager::LightingManager(ID3D11Device* a_pDevice, DirectX::XMFLOAT3 a_CameraPos)
    {
        InitLights(a_CameraPos);
    }

    void LightingManager::Update(ID3D11DeviceContext* a_pContext, float dt, DirectX::XMFLOAT3 a_CameraPos)
    {
        UpdateLights(dt, a_CameraPos);
    }


    // AAA Case: Bring in lights directly from a "world editor" of some sort, which exports light positions, colors, etc for environment artists
    // Me: Hardcode it!
    void LightingManager::InitLights(DirectX::XMFLOAT3 a_CameraPos)
    {
        using DirectX::XMFLOAT3A;

        SetAmbient(XMFLOAT3A(0.12f, 0.12f, 0.15f));

        mLightData.directionalLight.diffuseColor   = XMFLOAT3A(+1.0f, +0.772f, +0.56f); // sunlight
        mLightData.directionalLight.toLight        = XMFLOAT3A(-1.0f, +0.5f, -1.3f);

        // hold camera position in light data
        mLightData.cameraWorldPos.x = a_CameraPos.x;
        mLightData.cameraWorldPos.y = a_CameraPos.y;
        mLightData.cameraWorldPos.z = a_CameraPos.z;
    }

    void LightingManager::UpdateLights(float dt, DirectX::XMFLOAT3 a_CameraPos)
    {
        // Logic to update the light's direction or something
        //DirectionalLight& light = m_LightData.directionalLight;
        //light.toLight.x = cosf(dt);
        //light.toLight.z = sinf(dt);
        //

        // Overwrite held camera position
        mLightData.cameraWorldPos.x = a_CameraPos.x;
        mLightData.cameraWorldPos.y = a_CameraPos.y;
        mLightData.cameraWorldPos.z = a_CameraPos.z;
    }
}