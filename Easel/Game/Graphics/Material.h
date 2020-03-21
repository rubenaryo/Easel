/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Material class for shader information
----------------------------------------------*/
#ifndef MATERIAL_H
#define MATERIAL_H

#include "DXCore.h"
#include "Shader.h"
#include "Texture.h"
#include "ConstantBuffer.h"
#include "IBindable.h"
#include <vector>

namespace Graphics {

class Material : IBindable
{
public:
    Material(VertexShader* a_pVS, PixelShader* a_pPS, cbMaterialParams* a_pParams);
    Material(VertexShader* a_pVS, PixelShader* a_pPS, cbMaterialParams* a_pParams, Texture*const* a_Resources, size_t numResources);
    ~Material();

    // Getter for material parameters
    inline VertexShader*        GetVertexShader() const { return m_pVertexShader; }
    inline PixelShader*         GetPixelShader()  const { return m_pPixelShader;  }
    cbMaterialParams const&     GetMaterialInfo() const { return m_Params;        }

    // Bind override from IBindable
    inline void Bind(ID3D11DeviceContext* context) noexcept override;

private:
    VertexShader*       m_pVertexShader;
    PixelShader*        m_pPixelShader;
    cbMaterialParams    m_Params;
    Texture**           m_Resources;
    size_t              m_ResourceCount;
};

}

#endif