/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Implementation of LightingManager.h
----------------------------------------------*/
#include "LightingManager.h"

#include "LightStructs.h"

namespace Rendering
{
    LightingManager::LightingManager(ID3D11Device* device, ID3D11DeviceContext* context, DirectX::XMFLOAT3A cameraPos)
    {
        InitLights(cameraPos);

        ConstantBufferUpdateManager::Populate(sizeof(cbLighting), 10, EASEL_SHADER_STAGE::ESS_PS, device, &mBindPacket);
        ConstantBufferUpdateManager::Bind(&mBindPacket, context);
    }

    LightingManager::~LightingManager()
    {
        ConstantBufferUpdateManager::Cleanup(&mBindPacket);
    }

    void LightingManager::Update(ID3D11DeviceContext* context, float dt, DirectX::XMFLOAT3A cameraPos)
    {
        UpdateLights(dt, cameraPos, context);
    }

    // AAA Case: Bring in lights directly from a "world editor" of some sort, which exports light positions, colors, etc for environment artists
    // Me: Hardcode it!
    void LightingManager::InitLights(DirectX::XMFLOAT3A cameraPos)
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

    void LightingManager::UpdateLights(float dt, DirectX::XMFLOAT3A cameraPos, ID3D11DeviceContext* context)
    {
        // Logic to update the light's direction or something
        const float lightSpeed = 2.0f;
        DirectionalLight& light = mLightData.directionalLight;
        light.toLight.x = cosf(lightSpeed * dt);
        light.toLight.z = sinf(lightSpeed * dt);

        // Overwrite held camera position
        mLightData.cameraWorldPos = cameraPos;

        // Overwrite constant buffer
        ConstantBufferUpdateManager::MapUnmap(&mBindPacket, &mLightData, context);
    }
}