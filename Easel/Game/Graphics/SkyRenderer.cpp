/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/9
Description : Handles the rendering of the sky backdrop
----------------------------------------------*/
#include "SkyRenderer.h"

#include "Material.h"
#include "../Mesh.h"

namespace Graphics {

SkyRenderer::SkyRenderer(const Game::Mesh* pMesh, const Material* pMaterial, ID3D11Device* device):
    mpCubeMesh(pMesh),
    mpSkyMaterial(pMaterial)
{
    // Back-facing rasterizer state
    D3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_FRONT;
    rastDesc.DepthClipEnable = true;
    device->CreateRasterizerState(&rastDesc, &mpRSState);

    // Depth stencil with "less equal"
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    device->CreateDepthStencilState(&dsDesc, &mpDSState);

	#if defined(DEBUG)
	const char RSdebugName[] = "Sky_RS";
    const char DSdebugName[] = "Sky_DS";
	HRESULT hr = mpRSState->SetPrivateData(WKPDID_D3DDebugObjectName, ARRAYSIZE(RSdebugName) - 1, RSdebugName);
    hr = mpDSState->SetPrivateData(WKPDID_D3DDebugObjectName, ARRAYSIZE(DSdebugName) - 1, DSdebugName);
	if (FAILED(hr)) throw COM_EXCEPT(hr);
	#endif
}

// Bind material, d3d states, and then draw the sky mesh.
void SkyRenderer::Render(ID3D11DeviceContext* context) const
{
    mpSkyMaterial->Bind(context);
    
    context->OMSetDepthStencilState(mpDSState, 0);
    context->RSSetState(mpRSState);

    mpCubeMesh->Draw(context);

    // Reset states back to default
    context->OMSetDepthStencilState(0,0);
    context->RSSetState(0);
}

SkyRenderer::~SkyRenderer()
{
    mpRSState->Release();
    mpDSState->Release();
}

}