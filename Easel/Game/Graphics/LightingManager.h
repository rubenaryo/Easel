/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2019/12
Description : Interface for management of lights in the scene
----------------------------------------------*/
#ifndef LIGHTINGMANAGER_H
#define LIGHTINGMANAGER_H

#include "D3D11App.h"
#include "LightStructs.h"
#include "CBufferStructs.h"

namespace Graphics {
using Microsoft::WRL::ComPtr;

class LightingManager
{
public:
    LightingManager(ID3D11Device* a_pDevice);
    LightingManager() = delete;
    ~LightingManager();

    void Update(ID3D11DeviceContext* a_pContext);
    
    // Public Setter for the scene to be able to change the ambient color in the light buffer
    inline void SetAmbient(XMFLOAT3 ambientColor)
    {
        m_pLightBuffer->ambientColor = ambientColor;
        m_NeedsRebind = true;
    }

private:
    // Initializes the data for each directional light
    void InitLights();

    // Updates the data in each directional light
    void UpdateLights();

    // Defines the constant buffer which holds light information
    void CreateLightBuffer(ID3D11Device* a_pDevice);

    // Binds the light buffer to the pipeline
    void BindLightBuffer(ID3D11DeviceContext* a_pContext);

    // Helper Getters/Setters
    inline bool NeedsRebind() const
    { 
        return m_NeedsRebind;
    }

    inline DirectionalLight& GetLight(unsigned int index)
    {
        return m_pLightBuffer->lights[index];
    }

private:
    // Structure
    LightBuffer*                m_pLightBuffer;

    // Indicates whether we need to rebind
    bool                        m_NeedsRebind;

    // Constant buffer to be sent to the pixel shader
    ComPtr<ID3D11Buffer>        m_cbLights;
};

}

#endif