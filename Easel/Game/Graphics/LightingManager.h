/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Interface for management of lights in the scene
----------------------------------------------*/
#ifndef LIGHTINGMANAGER_H
#define LIGHTINGMANAGER_H

#include "DXCore.h"
#include "CBufferStructs.h"

#include <wrl/client.h>

namespace Graphics {

class LightingManager
{
public:
    LightingManager(ID3D11Device* a_pDevice, DirectX::XMFLOAT3 a_CameraPos);
    LightingManager()  = delete;
    ~LightingManager() = default;

    void Update(ID3D11DeviceContext* a_pContext, float dt, DirectX::XMFLOAT3 a_CameraPos);
    
    // Public Setter for the scene to be able to change the ambient color in the light buffer
    inline void SetAmbient(DirectX::XMFLOAT3A ambientColor)
    {
        mLightData.ambientColor = ambientColor;
    }

    inline cbLighting const& GetLightData() const
    {
        return mLightData;
    }

private:
    // Initializes the data for each directional light
    void InitLights(DirectX::XMFLOAT3 a_CameraPos);

    // Updates the data in each directional light
    void UpdateLights(float dt, DirectX::XMFLOAT3 a_CameraPos);

private:
    // Constant buffer struct
    cbLighting mLightData;
};

}

#endif