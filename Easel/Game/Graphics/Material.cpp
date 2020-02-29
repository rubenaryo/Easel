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

}

