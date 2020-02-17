/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Material class for shader information
----------------------------------------------*/
#include "Material.h"

namespace Graphics {

Material::Material(ComPtr<ID3D11VertexShader> a_pVS, ComPtr<ID3D11PixelShader> a_pPS)
{
    m_pVertexShader = a_pVS;
    m_pPixelShader = a_pPS;
}

ID3D11VertexShader* Material::GetVertexShader()
{
    return m_pVertexShader.Get();
}

ID3D11PixelShader* Material::GetPixelShader()
{
    return m_pPixelShader.Get();
}

}

