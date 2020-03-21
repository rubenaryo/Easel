/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Material class for shader information
----------------------------------------------*/
#include "Material.h"

namespace Graphics {

Material::Material(VertexShader* a_pVS, PixelShader* a_pPS, cbMaterialParams* a_pParams)
{
    // Set material parameters
    m_Params = *a_pParams;

    // Set Shader fields
    m_pVertexShader = a_pVS;
    m_pPixelShader = a_pPS;

    // not textured
    m_DiffuseTexture = nullptr;
}

Material::Material(VertexShader* a_pVS, PixelShader* a_pPS, cbMaterialParams* a_pParams, ID3D11ShaderResourceView* a_pSRV)
{
    // Set material parameters
    m_Params = *a_pParams;

    // Set Shader fields
    m_pVertexShader = a_pVS;
    m_pPixelShader = a_pPS;

    // textured
    m_DiffuseTexture = a_pSRV;
}

inline void Material::Bind(ID3D11DeviceContext* context) noexcept
{
    // Bind this materials VS,PS to the pipeline
    m_pVertexShader->Bind(context);
    m_pPixelShader->Bind(context);

    if(m_DiffuseTexture) // If this is a textured material, bind that texture
        context->PSSetShaderResources(0, 1, &m_DiffuseTexture);
}
}

