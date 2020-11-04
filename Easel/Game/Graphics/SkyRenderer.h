/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/9
Description : Handles the rendering of the sky backdrop
----------------------------------------------*/
#ifndef SKYRENDERER_H
#define SKYRENDERER_H

#include "DXCore.h"
#include "ResourceCodex.h"

namespace Rendering
{
struct VertexShader;
struct PixelShader;
}

namespace Rendering {

struct TSkyRenderer
{
    ID3D11RasterizerState*   RasterState;
    ID3D11DepthStencilState* DepthState;
    const VertexShader*     SkyVS;
    const PixelShader*      SkyPS;
    MeshID                   CubeMeshID;
    TextureID                SkyTextureID;

    void Init(const VertexShader* vs, const PixelShader* ps, MeshID cubeMeshID, TextureID skyTextureID, ID3D11Device* device);
};

}
#endif