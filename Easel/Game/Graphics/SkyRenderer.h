/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/9
Description : Handles the rendering of the sky backdrop
----------------------------------------------*/
#ifndef SKYRENDERER_H
#define SKYRENDERER_H

#include "DXCore.h"

namespace Rendering
{
struct VertexShader;
struct PixelShader;
struct Mesh;
struct ResourceBindChord;
}

namespace Rendering {

struct SkyRenderer
{
public:
    void Init(ID3D11Device* device);
    void Draw(ID3D11DeviceContext* context);
    void Cleanup();

private:
    // Drawing params
    ID3D11RasterizerState*      RasterState;
    ID3D11DepthStencilState*    DepthState;
    const VertexShader*         SkyVS;
    const PixelShader*          SkyPS;
    const Mesh*                 CubeMesh;
    const ResourceBindChord*    SkyTexture;
};

}
#endif