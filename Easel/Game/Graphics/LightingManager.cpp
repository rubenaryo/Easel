/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Implementation of LightingManager.h
----------------------------------------------*/
#include "LightingManager.h"

#include "LightStructs.h"

namespace Graphics
{
    LightingManager::LightingManager(ID3D11Device* device, DirectX::XMFLOAT3 cameraPos)
    {
        InitLights(cameraPos);
    }

    void LightingManager::Update(ID3D11DeviceContext* context, float dt, DirectX::XMFLOAT3 cameraPos)
    {
        UpdateLights(dt, cameraPos);
    }


    // AAA Case: Bring in lights directly from a "world editor" of some sort, which exports light positions, colors, etc for environment artists
    // Me: Hardcode it!
    void LightingManager::InitLights(DirectX::XMFLOAT3 cameraPos)
    {
        using DirectX::XMFLOAT3A;

        SetAmbient(XMFLOAT3A(0.12f, 0.12f, 0.15f));

        mLightData.directionalLight.diffuseColor   = XMFLOAT3A(+1.0f, +0.772f, +0.56f); // sunlight
        mLightData.directionalLight.toLight        = XMFLOAT3A(-1.0f, +0.5f, -1.3f);

        // hold camera position in light data
        mLightData.cameraWorldPos.x = cameraPos.x;
        mLightData.cameraWorldPos.y = cameraPos.y;
        mLightData.cameraWorldPos.z = cameraPos.z;
    }

    void LightingManager::UpdateLights(float dt, DirectX::XMFLOAT3 cameraPos)
    {
        // Logic to update the light's direction or something
        //DirectionalLight& light = m_LightData.directionalLight;
        //light.toLight.x = cosf(dt);
        //light.toLight.z = sinf(dt);
        //

        // Overwrite held camera position
        mLightData.cameraWorldPos.x = cameraPos.x;
        mLightData.cameraWorldPos.y = cameraPos.y;
        mLightData.cameraWorldPos.z = cameraPos.z;
    }
}