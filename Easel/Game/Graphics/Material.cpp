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

Material::Material(ID3D11DeviceContext* context, VertexShader* a_pVS, PixelShader* a_pPS, DirectX::XMFLOAT4 a_ColorTint, float a_Specularity)
{
    cbMaterialParams mp = {};
    mp.m_ColorTint = a_ColorTint;
    mp.m_Specularity = a_Specularity;

    //m_pMaterialParams = new VSConstantBuffer()
    m_pMaterialParams->SetData(context, sizeof(cbMaterialParams), (void*) &mp);
}

inline void Material::Bind(ID3D11DeviceContext* context) noexcept
{
    // Bind material params cbuffer to the pixel shader
    m_pMaterialParams->Bind(context);

    // Bind this materials VS,PS to the pipeline
    context->VSSetShader(m_pVS->GetShader(), 0, 0);
    context->PSSetShader(m_pPS->GetShader(), 0, 0);
}



}

