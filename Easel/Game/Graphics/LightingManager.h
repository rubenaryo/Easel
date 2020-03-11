/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Interface for management of lights in the scene
----------------------------------------------*/
#ifndef LIGHTINGMANAGER_H
#define LIGHTINGMANAGER_H

#include "DXCore.h"
#include "LightStructs.h"
#include "CBufferStructs.h"
#include "ConstantBuffer.h"

#include <wrl/client.h>

namespace Graphics {

class LightingManager
{
const unsigned int c_PSLighingSlot = 12u;

public:
    LightingManager(ID3D11Device* a_pDevice);
    LightingManager() = delete;
    ~LightingManager();

    void Update(ID3D11DeviceContext* a_pContext, float dt);
    
    // Public Setter for the scene to be able to change the ambient color in the light buffer
    inline void SetAmbient(DirectX::XMFLOAT3A ambientColor)
    {
        m_LightData.ambientColor = ambientColor;
        m_NeedsRebind = true;
    }

private:
    // Initializes the data for each directional light
    void InitLights();

    // Updates the data in each directional light
    void UpdateLights(float dt);

    // Defines the constant buffer which holds light information
    void CreateLightBuffer(ID3D11Device* a_pDevice);

    // Binds the light buffer to the pipeline
    void BindLightBuffer(ID3D11DeviceContext* a_pContext);

    // Helper Getters/Setters
    inline bool NeedsRebind() const
    { 
        return m_NeedsRebind;
    }

private:
    // Structure
    cbLighting m_LightData;

    // Constant Buffer
    PSConstantBuffer* m_pLightingBuffer;

    // Indicates whether we need to rebind
    bool m_NeedsRebind;

    
};

}

#endif