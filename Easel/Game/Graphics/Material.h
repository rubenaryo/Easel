/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Material class for shader information
----------------------------------------------*/
#ifndef MATERIAL_H
#define MATERIAL_H

#include "DXCore.h"
#include "ConstantBuffer.h"
#include "CBufferStructs.h"
#include "IBindable.h"

#include <vector>

namespace Graphics
{
class VertexShader;
class PixelShader;
class Texture;
}

namespace Graphics {

class Material : IBindable
{
public:
    Material(VertexShader* pVS, PixelShader* pPS, cbMaterialParams* pParams);
    Material(VertexShader* pVS, PixelShader* pPS, cbMaterialParams* pParams, Texture*const* ppResources, uint32_t numResources);
    ~Material();

    // Access methods for shaders, material params
    VertexShader*       GetVertexShader() const { return mpVertexShader; }
    PixelShader*        GetPixelShader()  const { return mpPixelShader;  }
    cbMaterialParams&   GetMaterialInfo()       { return mParams;        }

    // Bind override from IBindable
    inline void Bind(ID3D11DeviceContext* context) const override;

private:
    VertexShader*       mpVertexShader;
    PixelShader*        mpPixelShader;
    cbMaterialParams    mParams;
    Texture**           mResources;
    uint32_t            mResourceCount;
};

}

#endif