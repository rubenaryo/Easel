/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Implementation of Renderer class
----------------------------------------------*/
#include "Renderer.h"

#include "Camera.h"
#include "CBufferStructs.h"
#include "DeviceResources.h"
#include "hash_util.h"
#include "Material.h"
#include "Mesh.h"
#include "ResourceCodex.h"
#include "Shader.h"
#include "SkyRenderer.h"
#include "ThrowMacros.h"

#if defined(DEBUG)
#include <typeinfo>
#endif

#include <random>
#include <time.h>

namespace Rendering {

Renderer::Renderer()
{
}

void Renderer::Init(DeviceResources* dr)
{
    // Grab reference to ID3D11Device
    auto device = dr->GetDevice();
    auto context = dr->GetContext();

    ResourceCodex::Init(device, context);

    // Initialize meshes, materials, entities
    InitMeshes(dr);
    InitDrawContexts(device);
    
    // For now, assume we're only using trianglelist
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // The camera buffer is never unbound again, so just set it now
    context->VSSetConstantBuffers(mCameraBuffer.BindSlot, 1, &mCameraBuffer.ConstantBuffer);

    // Same with the lighting
    context->PSSetConstantBuffers(mLightingBuffer.BindSlot, 1, &mLightingBuffer.ConstantBuffer);

    #if defined(DEBUG)
        #define TTYPE VertexShader
        char buf[64];
        SecureZeroMemory(buf, 64);
        sprintf_s(buf, "Size of %s: %zu\n", typeid(TTYPE).name(), sizeof(TTYPE));
        OutputDebugStringA(buf);
        #undef TTYPE
    #endif
}

void Renderer::InitMeshes(DeviceResources* dr)
{
    auto device = dr->GetDevice();

    ResourceCodex* sg_Codex = ResourceCodex::GetSingleton();

    const ShaderID vsId = ResourceCodex::AddVertexShader(L"InstancedPhongVS.cso", device);
    const ShaderID psId = ResourceCodex::AddPixelShader(L"PhongPS.cso", device);

    const ShaderID kInstancedPhongVSID = 0xc8a366aa; // FNV1A of L"InstancedPhongVS.cso"
    const ShaderID kPhongPSID = 0x4dc6e249;          // FNV1A of L"PhongPS.cso"

    const VertexShader* instancedPhongVS = sg_Codex->GetVertexShader(kInstancedPhongVSID);
    const PixelShader*  PhongPS = sg_Codex->GetPixelShader(psId);

    const VertexShader* skyVS = sg_Codex->GetVertexShader(ResourceCodex::AddVertexShader(L"SkyVS.cso", device));
    const PixelShader*  skyPS = sg_Codex->GetPixelShader(ResourceCodex::AddPixelShader(L"SkyPS.cso", device));

    const VertexBufferDescription* phongVertDesc = &instancedPhongVS->VertexDesc;
    const MeshID sphereID = ResourceCodex::AddMeshFromFile("sphere.obj", phongVertDesc, device);
    
    dr->GetContext()->PSSetSamplers(0, 1, &PhongPS->SamplerState);

    sg_Codex->BuildAllMaterials();
    
    mSkyRenderer.Init(skyVS, skyPS, ResourceCodex::AddMeshFromFile("cube.obj", phongVertDesc, device), 0x2fb626d6, device);
}

void Renderer::InitDrawContexts(ID3D11Device* device)
{
    // Description for dynamic vertex buffer
    D3D11_BUFFER_DESC dynamicDesc = {0};
    dynamicDesc.Usage = D3D11_USAGE_DYNAMIC;
    dynamicDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    dynamicDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    dynamicDesc.MiscFlags = 0;
    dynamicDesc.StructureByteStride = 0;
    dynamicDesc.ByteWidth = -1; // This must be changed per new buffer
    
    ResourceCodex* sg_Codex = ResourceCodex::GetSingleton();
    
    const UINT kNumEntities = 100;
    EntityCount = kNumEntities;

    Entities = (TEntity*)malloc(sizeof(TEntity) * kNumEntities);
    for(UINT i = 0; i != kNumEntities; ++i)
    { 
        Game::Transform tfm;
        tfm.SetTranslation(i * 5.f, 0.0f, 0.0f);

        TEntity test;
        test.MaterialIndex = 0;
        test.mMeshID = 0x4fd8281f;
        test.mTransform = tfm;

        Entities[i] = test;
    }

    instancingPasses = (InstancedDrawContext*)malloc(sizeof(InstancedDrawContext) * 1);

    InstancedDrawContext& lunarDraw = instancingPasses[0];
    lunarDraw.InstanceCount   = kNumEntities;
    lunarDraw.WorldMatrices   = (DirectX::XMFLOAT4X4*)malloc(sizeof(DirectX::XMFLOAT4X4) * lunarDraw.InstanceCount);
    lunarDraw.InstancedMeshID = Entities[0].mMeshID;
    lunarDraw.MaterialIndex   = 0;

    // Create the dynamic buffer
    dynamicDesc.ByteWidth = sizeof(DirectX::XMFLOAT4X4) * lunarDraw.InstanceCount;
    HRESULT hr = device->CreateBuffer(&dynamicDesc, nullptr, &lunarDraw.DynamicBuffer);
    COM_EXCEPT(hr);

    // Reuse the desc to make the camera cbuffer;
    dynamicDesc.ByteWidth = sizeof(cbCamera);
    dynamicDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hr = device->CreateBuffer(&dynamicDesc, nullptr, &mCameraBuffer.ConstantBuffer);
    COM_EXCEPT(hr);
    mCameraBuffer.BindSlot = 10;

    // Make the lighting buffer now
    dynamicDesc.ByteWidth = sizeof(cbLighting);
    hr = device->CreateBuffer(&dynamicDesc, nullptr, &mLightingBuffer.ConstantBuffer);
    COM_EXCEPT(hr);
    mLightingBuffer.BindSlot = 10;
}

void Renderer::Update(ID3D11DeviceContext* context, float dt, const Camera* camera, const cbLighting* lightData)
{
    using namespace DirectX;
    using Game::Transform;
    
    // Hold camera, lighting data locally for rendering
    cbCamera camData = camera->AsConstantBuffer();
    
    // Prepare the camera buffer
    D3D11_MAPPED_SUBRESOURCE mappedBuffer = {0};
    HRESULT hr = context->Map(mCameraBuffer.ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
    memcpy(mappedBuffer.pData, &camData, sizeof(camData));
    context->Unmap(mCameraBuffer.ConstantBuffer, 0);

    // Prepare Lighting buffer
    hr = context->Map(mLightingBuffer.ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
    memcpy(mappedBuffer.pData, lightData, sizeof(cbLighting));
    context->Unmap(mLightingBuffer.ConstantBuffer, 0);

    const float rotSpeed = 1.25f;

    static const XMVECTOR rot1 = DirectX::XMQuaternionRotationRollPitchYaw(rotSpeed, -rotSpeed, rotSpeed);
    static const XMVECTOR rot2 = -rot1;

    InstancedDrawContext& lunarDraw = instancingPasses[0];

    for (UINT i = 0; i != EntityCount; ++i)
    {
        Transform* tfm = &Entities[i].mTransform;
        tfm->Rotate(rot1*dt);
        lunarDraw.WorldMatrices[i] = tfm->Recompute();
    }

    // Rewrite the dynamic vertex buffer
    hr = context->Map(lunarDraw.DynamicBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
    memcpy(mappedBuffer.pData, lunarDraw.WorldMatrices, sizeof(DirectX::XMFLOAT4X4) * instancingPasses[0].InstanceCount);
    context->Unmap(lunarDraw.DynamicBuffer, 0);
}

void Renderer::Draw(ID3D11DeviceContext* context)
{
    this->InstancedDraw(instancingPasses, 1, context);
    this->RenderSky(context);
}

void Renderer::InstancedDraw(InstancedDrawContext* drawContexts, UINT drawCallCount, ID3D11DeviceContext* context)
{
    ResourceCodex* sg_Codex = ResourceCodex::GetSingleton();

    InstancedDrawContext* drawCtx = drawContexts; 
    InstancedDrawContext* const drawCtxItEnd = drawContexts + drawCallCount;
    for (; drawCtx != drawCtxItEnd; ++drawCtx)
    {
        UINT instanceCount = drawCtx->InstanceCount;
        const Mesh* mesh = sg_Codex->GetMesh(drawCtx->InstancedMeshID);

        ID3D11Buffer* vertBuffers[2];
        vertBuffers[0] = mesh->VertexBuffer; // Vertices
        vertBuffers[1] = drawCtx->DynamicBuffer;  // Instanced World Matrices

        UINT strides[2] = 
        {
            mesh->Stride,
            sizeof(DirectX::XMFLOAT4X4)
        };

        UINT offsets[2] = 
        {
            0, 
            0
        };

        context->IASetVertexBuffers(0, 2, &vertBuffers[0], &strides[0], &offsets[0]);
        context->IASetIndexBuffer(mesh->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

        // Setup VS,PS
        const TMaterial* mat = sg_Codex->GetMaterial(drawCtx->MaterialIndex);
        const VertexShader VS = *mat->VS;
        const PixelShader  PS = *mat->PS;

        context->IASetInputLayout(VS.InputLayout);
        context->VSSetShader(VS.Shader, nullptr, 0);
        context->PSSetShader(PS.Shader, nullptr, 0);

        // Binding Textures
        context->PSSetShaderResources(0, (UINT)TextureSlots::COUNT, mat->Resources->SRVs);

        context->DrawIndexedInstanced(mesh->IndexCount, drawCtx->InstanceCount, 0, 0, 0);
    }
}

void Renderer::RenderSky(ID3D11DeviceContext* context)
{
    ResourceCodex* sg_Codex = ResourceCodex::GetSingleton();
    const Mesh* mesh = sg_Codex->GetMesh(mSkyRenderer.CubeMeshID);

    // Sst backface culling
    context->OMSetDepthStencilState(mSkyRenderer.DepthState, 0);
    context->RSSetState(mSkyRenderer.RasterState);

    UINT offsets = 0;

    // Bind the Cube Mesh
    context->IASetVertexBuffers(0, 1, &mesh->VertexBuffer, &mesh->Stride, &offsets);
    context->IASetIndexBuffer(mesh->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set Vertex Shader and Input
    context->IASetInputLayout(mSkyRenderer.SkyVS->InputLayout);
    context->VSSetShader(mSkyRenderer.SkyVS->Shader, 0, 0);

    // Set Pixel Shader and Bind Textures
    const ResourceBindChord* chord = sg_Codex->GetTexture(mSkyRenderer.SkyTextureID);
    ID3D11ShaderResourceView* const* srvs = chord->SRVs;
    context->PSSetShaderResources(0, (UINT)TextureSlots::COUNT, srvs);
    context->PSSetShader(mSkyRenderer.SkyPS->Shader, 0, 0);

    context->DrawIndexed(mesh->IndexCount, 0, 0);

    // Reset states back to default
    context->OMSetDepthStencilState(0,0);
    context->RSSetState(0);
}

Renderer::~Renderer()
{
    for (uint8_t i = 0; i != 1; ++i)
    {
        free(instancingPasses->WorldMatrices);
        instancingPasses->DynamicBuffer->Release();
    }
    free(instancingPasses);

    free(Entities);

    mCameraBuffer.ConstantBuffer->Release();
    mLightingBuffer.ConstantBuffer->Release();

    mSkyRenderer.RasterState->Release();
    mSkyRenderer.DepthState->Release();
    
    ResourceCodex::Destroy();
}
}