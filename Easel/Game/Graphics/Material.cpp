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
    m_pPixelShader  = a_pPS;

    // Not textured
    m_Resources = nullptr;
}

Material::Material(VertexShader* a_pVS, PixelShader* a_pPS, cbMaterialParams* a_pParams, Texture*const* a_Resources, size_t numResources)
{
    // Set material parameters
    m_Params = *a_pParams;

    // Set Shader fields
    m_pVertexShader = a_pVS;
    m_pPixelShader  = a_pPS;

    // Copy over resource pointers
    m_Resources     = new Texture*[numResources];
    memcpy(m_Resources, a_Resources, sizeof(Texture*) * numResources);
    m_ResourceCount = numResources;
}

inline void Material::Bind(ID3D11DeviceContext* context) noexcept
{
    // Bind this materials VS,PS to the pipeline
    m_pVertexShader->Bind(context);
    m_pPixelShader->Bind(context);

    // Bind all the associated textures
    for (int i = 0; i < m_ResourceCount; ++i)
    {
        m_Resources[i]->Bind(context);
    }
}

Material::~Material()
{
    // Delete the array of pointers, but not the textures themselves
    delete[] m_Resources;
}
}

