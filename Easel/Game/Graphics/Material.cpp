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
    mParams = *a_pParams;

    // Set Shader fields
    mpVertexShader = a_pVS;
    mpPixelShader  = a_pPS;

    // Not textured
    mResources = nullptr;
}

Material::Material(VertexShader* a_pVS, PixelShader* a_pPS, cbMaterialParams* a_pParams, Texture*const* a_Resources, uint32_t numResources)
{
    // Set material parameters
    mParams = *a_pParams;

    // Set Shader fields
    mpVertexShader = a_pVS;
    mpPixelShader  = a_pPS;

    // Copy over resource pointers
    mResources     = new Texture*[numResources];
    memcpy(mResources, a_Resources, sizeof(Texture*) * numResources);
    mResourceCount = numResources;
}

inline void Material::Bind(ID3D11DeviceContext* context) const
{
    // Set material parameters
    mpPixelShader->SetBufferData(
        context, 
        (UINT)ReservedRegisters::RR_PS_MATERIAL, 
        sizeof(mParams),
        reinterpret_cast<const void*>(&mParams));

    // Bind all the associated textures
    for (uint32_t t = 0; t < mResourceCount; ++t)
    {
        mpPixelShader->SetTexture(context, mResources[t]);
    }

    // Bind this material's VS,PS to the pipeline
    mpVertexShader->Bind(context);
    mpPixelShader->Bind(context);
}

Material::~Material()
{
    // Delete the array of pointers, but not the textures themselves
    delete[] mResources;
}
}

