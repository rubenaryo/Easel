/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/9
Description : Handles the rendering of the sky backdrop
----------------------------------------------*/
#include "SkyRenderer.h"

#include "Material.h"
#include "ResourceCodex.h"
#include "ThrowMacros.h"

#include "../Mesh.h"

namespace Rendering {

void TSkyRenderer::Init(const VertexShader* vs, const PixelShader* ps, MeshID cubeMeshID, TextureID skyTextureID, ID3D11Device* device)
{
    this->SkyVS = vs;
    this->SkyPS = ps;
    this->CubeMeshID = cubeMeshID;
    this->SkyTextureID = skyTextureID;

    // Back-facing rasterizer state
    D3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_FRONT;
    rastDesc.DepthClipEnable = true;
    device->CreateRasterizerState(&rastDesc, &RasterState);

    // Depth stencil with "less equal"
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    device->CreateDepthStencilState(&dsDesc, &DepthState);

    #if defined(DEBUG)
    const char RSdebugName[] = "Sky_RS";
    const char DSdebugName[] = "Sky_DS";
    HRESULT hr = RasterState->SetPrivateData(WKPDID_D3DDebugObjectName, ARRAYSIZE(RSdebugName) - 1, RSdebugName);
    hr = DepthState->SetPrivateData(WKPDID_D3DDebugObjectName, ARRAYSIZE(DSdebugName) - 1, DSdebugName);
    COM_EXCEPT(hr);
    #endif
}

}