/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Material class for shader information
----------------------------------------------*/
#ifndef MATERIAL_H
#define MATERIAL_H

#include "DXCore.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "IBindable.h"
#include <vector>

namespace Graphics {

class Material : IBindable
{
public:
    Material(ID3D11VertexShader* a_pVS, ID3D11PixelShader* a_pPS, DirectX::XMFLOAT4 a_ColorTint, float a_Specularity);
    Material(VertexShader* a_pVS, PixelShader* a_pPS, cbMaterialParams* a_pParams);
    ~Material() = default;

    // Field Getters
    inline ID3D11VertexShader*  GetVertexShader() { return m_pVertexShader; }
    inline ID3D11PixelShader*   GetPixelShader()  { return m_pPixelShader;  }
    inline DirectX::XMFLOAT4    GetColorTint()    { return m_ColorTint;     }
    inline float                GetSpecularity()  { return m_Specularity;   }

    // Getter for material parameters
    cbMaterialParams const&     GetMaterialInfo() { return m_Params;        }

    // Bind override from IBindable
    inline void Bind(ID3D11DeviceContext* context) noexcept override;

private:
    ID3D11PixelShader*  m_pPixelShader;
    ID3D11VertexShader* m_pVertexShader;
    DirectX::XMFLOAT4   m_ColorTint;
    float               m_Specularity;

    VertexShader*       m_pVS;
    PixelShader*        m_pPS;
    cbMaterialParams    m_Params;
};

}

#endif