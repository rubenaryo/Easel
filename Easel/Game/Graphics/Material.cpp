/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Material class for shader information
----------------------------------------------*/
#include "Material.h"

namespace Graphics {

Material::Material(ID3D11VertexShader* a_pVS, ID3D11PixelShader* a_pPS, DirectX::XMFLOAT4 a_ColorTint, float a_Specularity)
{
    m_pVertexShader = a_pVS;
    m_pPixelShader  = a_pPS;
    m_ColorTint     = a_ColorTint;
    m_Specularity   = a_Specularity;
}

Material::Material(VertexShader* a_pVS, PixelShader* a_pPS, cbMaterialParams* a_pParams)
{
    // Set material parameters
    m_Params = *a_pParams;

    // Set Shader fields
    m_pVS = a_pVS;
    m_pPS = a_pPS;
}

inline void Material::Bind(ID3D11DeviceContext* context) noexcept
{
    // Bind this materials VS,PS to the pipeline
    context->VSSetShader(m_pVS->GetShader(), 0, 0);
    context->PSSetShader(m_pPS->GetShader(), 0, 0);
}



}

