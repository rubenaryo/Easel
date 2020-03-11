/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Material class for shader information
----------------------------------------------*/
#ifndef MATERIAL_H
#define MATERIAL_H

#include "DXCore.h"
#include "Shader.h"
#include "ConstantBuffer.h"
#include "IBindable.h"
#include <vector>

namespace Graphics {

class Material : IBindable
{
public:
    Material(VertexShader* a_pVS, PixelShader* a_pPS, cbMaterialParams* a_pParams);
    ~Material() = default;

    // Getter for material parameters
    inline VertexShader*        GetVertexShader() { return m_pVertexShader;           }
    inline PixelShader*         GetPixelShader()  { return m_pPixelShader;           }
    cbMaterialParams const&     GetMaterialInfo() { return m_Params;        }

    // Bind override from IBindable
    inline void Bind(ID3D11DeviceContext* context) noexcept override;

private:
    VertexShader*       m_pVertexShader;
    PixelShader*        m_pPixelShader;
    cbMaterialParams    m_Params;
};

}

#endif