/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Material class for shader information
----------------------------------------------*/
#include "Material.h"

namespace Graphics {

Material::Material(ID3D11VertexShader* a_pVS, ID3D11PixelShader* a_pPS)
{
    m_pVertexShader = a_pVS;
    m_pPixelShader = a_pPS;
}

ID3D11VertexShader* Material::GetVertexShader()
{
    return m_pVertexShader;
}

ID3D11PixelShader* Material::GetPixelShader()
{
    return m_pPixelShader;
}

}

