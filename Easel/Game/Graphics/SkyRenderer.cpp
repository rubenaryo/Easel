/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/9
Description : Handles the rendering of the sky backdrop
----------------------------------------------*/
#include "SkyRenderer.h"

#include "ResourceCodex.h"
#include "ThrowMacros.h"

#include "Mesh.h"
#include "Shader.h"
#include "Material.h"

namespace Rendering {

void SkyRenderer::Init(ID3D11Device* device)
{
    // Mesh, texture, and shaders
    ResourceCodex const& sg_Codex = ResourceCodex::GetSingleton();
    const ShaderID kSkyVSID = 0xeb5accd4;         // fnv1a L"SkyVS.cso"
    const ShaderID kSkyPSID = 0x6ec235e6;         // fnv1a L"SkyPS.cso"
    const TextureID kSkyTextureID = 0x2fb626d6;   // fnv1a L"Sky"
    const TextureID kSpaceTextureID = 0xc1c43225; // fnv1a L"Space"
    const MeshID kSkyMeshID = 0x4a986f37;

    // Query the resource codex to get the bindables directly.
    SkyVS = sg_Codex.GetVertexShader(kSkyVSID);
    SkyPS = sg_Codex.GetPixelShader(kSkyPSID);
    SkyTexture = sg_Codex.GetTexture(kSkyTextureID);
    CubeMesh = sg_Codex.GetMesh(kSkyMeshID);

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

void SkyRenderer::Draw(ID3D11DeviceContext* context)
{
    // Sst backface culling
    context->OMSetDepthStencilState(DepthState, 0);
    context->RSSetState(RasterState);

    UINT offsets = 0;

    // Bind the Cube Mesh
    const Mesh mesh = *CubeMesh;
    context->IASetVertexBuffers(0, 1, &mesh.VertexBuffer, &mesh.Stride, &offsets);
    context->IASetIndexBuffer(mesh.IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Set Vertex Shader and Input
    context->IASetInputLayout(SkyVS->InputLayout);
    context->VSSetShader(SkyVS->Shader, 0, 0);

    // Set Pixel Shader and Bind Textures
    context->PSSetShaderResources(0, (UINT)TextureSlots::COUNT, SkyTexture->SRVs);
    context->PSSetShader(SkyPS->Shader, 0, 0);
    context->PSSetSamplers(0, 1, &SkyPS->SamplerState);

    // Submit Draw Call
    context->DrawIndexed(mesh.IndexCount, 0, 0);

    // Reset states back to default
    context->OMSetDepthStencilState(0, 0);
    context->RSSetState(0);
}

void SkyRenderer::Shutdown()
{
    RasterState->Release();
    DepthState->Release();
}

}