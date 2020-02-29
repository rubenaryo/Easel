/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Material class for shader information
----------------------------------------------*/
#ifndef MATERIAL_H
#define MATERIAL_H

#include "D3D11App.h"

namespace Graphics {

class Material
{
public:
    Material(ID3D11VertexShader* a_pVS, ID3D11PixelShader* a_pPS, DirectX::XMFLOAT4 a_ColorTint, float a_Specularity);
    ~Material() = default;

    // Field Getters
    inline ID3D11VertexShader*  GetVertexShader() { return m_pVertexShader; }
    inline ID3D11PixelShader*   GetPixelShader()  { return m_pPixelShader;  }
    inline DirectX::XMFLOAT4    GetColorTint()    { return m_ColorTint;     }
    inline float                GetSpecularity()  { return m_Specularity;   }

private:
    ID3D11PixelShader*  m_pPixelShader;
    ID3D11VertexShader* m_pVertexShader;
    DirectX::XMFLOAT4   m_ColorTint;
    float               m_Specularity;
};

}

#endif