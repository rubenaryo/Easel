/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Interface for management of lights in the scene
----------------------------------------------*/
#ifndef LIGHTINGMANAGER_H
#define LIGHTINGMANAGER_H

#include "DXCore.h"
#include "CBufferStructs.h"
#include "ConstantBuffer.h"

namespace Rendering {

class LightingManager
{
public:
    LightingManager(ID3D11Device* device, ID3D11DeviceContext* context, DirectX::XMFLOAT3A cameraPos);
    LightingManager()  = delete;
    ~LightingManager();

    void Update(ID3D11DeviceContext* context, float dt, DirectX::XMFLOAT3A cameraPos);
    
    // Public Setter for the scene to be able to change the ambient color in the light buffer
    inline void SetAmbient(DirectX::XMFLOAT3A ambientColor)
    {
        mLightData.ambientColor = ambientColor;
    }

    const ConstantBufferBindPacket* GetBindPacket() const { return &mBindPacket; }

private:
    // Initializes the data for each directional light
    void InitLights(DirectX::XMFLOAT3A cameraPos);

    // Updates the data in each directional light
    void UpdateLights(float dt, DirectX::XMFLOAT3A cameraPos, ID3D11DeviceContext* context);

private:
    ConstantBufferBindPacket mBindPacket;

    // Constant buffer struct
    cbLighting mLightData;
};

}

#endif