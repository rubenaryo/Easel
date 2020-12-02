/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Implementation of Renderer class
----------------------------------------------*/
#include "EntityRenderer.h"

#include "Camera.h"
#include "CBufferStructs.h"
#include "ConstantBuffer.h"
#include "DeviceResources.h"
#include "DrawContext.h"
#include "hash_util.h"
#include "Material.h"
#include "Mesh.h"
#include "ResourceCodex.h"
#include "Shader.h"
#include "SkyRenderer.h"
#include "ThrowMacros.h"

#include <random>
#include <time.h>

namespace Rendering {

EntityRenderer::EntityRenderer()
{}

void EntityRenderer::Init(DeviceResources const& dr)
{
    // Grab reference to d3d11 device and context
    auto device = dr.GetDevice();
    auto context = dr.GetContext();

    // Initialize meshes, materials, entities
    InitMeshes(dr);
    InitEntities();
    InitDrawContexts(device);
    
    // For now, assume we're only using trianglelist
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ConstantBufferUpdateManager::Populate(sizeof(cbMaterialParams), (UINT)PS_REGISTERS::MATERIAL, EASEL_SHADER_STAGE::ESS_PS, device, &MaterialParamsCB);
    ConstantBufferUpdateManager::Bind(&MaterialParamsCB, context);

    ConstantBufferUpdateManager::Populate(sizeof(cbPerEntity), (UINT)VS_REGISTERS::WORLD, EASEL_SHADER_STAGE::ESS_VS, device, &EntityCB);
    ConstantBufferUpdateManager::Bind(&EntityCB, context);
    
    EntityBatchRenderer.Init(device);
}

//TODO: Lots of hardcoded hashes here huh
void EntityRenderer::InitMeshes(DeviceResources const& dr)
{
    auto device = dr.GetDevice();

    ResourceCodex const& sg_Codex = ResourceCodex::GetSingleton();

    const ShaderID kInstancedPhongVSID = 0xc8a366aa; // FNV1A of L"InstancedPhongVS.cso"
    const ShaderID kPhongPSID = 0x4dc6e249;          // FNV1A of L"PhongPS.cso"
    const ShaderID kPhongPSNormalMapID = 0x928ff72d; // FNV1A of L"PhongPS_NormalMap.cso"

    const VertexShader* instancedPhongVS = sg_Codex.GetVertexShader(kInstancedPhongVSID);
    const PixelShader*  PhongPS = sg_Codex.GetPixelShader(kPhongPSID);

    const VertexBufferDescription* phongVertDesc = &instancedPhongVS->VertexDesc;
    const MeshID sphereID = ResourceCodex::AddMeshFromFile("sphere.obj", phongVertDesc, device);
    const MeshID cubeID = ResourceCodex::AddMeshFromFile("cube.obj", phongVertDesc, device);
    
    //dr.GetContext()->PSSetSamplers(0, 1, &PhongPS->SamplerState);
}

void EntityRenderer::InitEntities()
{
    const UINT kNumEntities1 = 100;
    EntityGroups[0].WorldMatrices  = (DirectX::XMFLOAT4X4*)malloc(sizeof(DirectX::XMFLOAT4X4) * kNumEntities1);
    EntityGroups[0].Transforms     = (Transform*)malloc(sizeof(Transform) * kNumEntities1);
    EntityGroups[0].Count          = kNumEntities1;

    for(UINT i = 0; i != kNumEntities1; ++i)
    { 
        Transform& transform = EntityGroups[0].Transforms[i];
        transform.ResetFields();
        transform.SetTranslation(i * 1.5f, 0.0f, 0.0f);
    }

    const UINT kNumEntities2 = 40;
    EntityGroups[1].WorldMatrices  = (DirectX::XMFLOAT4X4*)malloc(sizeof(DirectX::XMFLOAT4X4) * kNumEntities2);
    EntityGroups[1].Transforms     = (Transform*)malloc(sizeof(Transform) * kNumEntities2);
    EntityGroups[1].Count          = kNumEntities2;

    for(UINT i = 0; i != kNumEntities2; ++i)
    { 
        Transform& transform = EntityGroups[1].Transforms[i];
        transform.ResetFields();
        transform.SetTranslation(0.0f, i * 1.5f, 0.0f);
    }
}

void EntityRenderer::InitDrawContexts(ID3D11Device* device)
{
    const UINT kInstancingPassCount = 2;
    const MeshID kSphereID = 0x4fd8281f;
    
    ResourceCodex const& sg_Codex = ResourceCodex::GetSingleton();

    InstancingPassCount = kInstancingPassCount;
    InstancingPasses = (InstancedDrawContext*)malloc(sizeof(InstancedDrawContext) * kInstancingPassCount);

    InstancedDrawContext& lunarDraw = InstancingPasses[0];
    lunarDraw.Entities       = &EntityGroups[0];
    lunarDraw.InstancedMesh  = sg_Codex.GetMesh(kSphereID);
    lunarDraw.EntityMaterial = sg_Codex.GetMaterial(0);
    
    InstancedDrawContext& earthDraw = InstancingPasses[1];
    earthDraw.Entities       = &EntityGroups[1];
    earthDraw.InstancedMesh  = sg_Codex.GetMesh(kSphereID);
    earthDraw.EntityMaterial = sg_Codex.GetMaterial(1);

    // Create the dynamic vertex buffer
    D3D11_BUFFER_DESC dynamicDesc = {0};
    dynamicDesc.Usage = D3D11_USAGE_DYNAMIC;
    dynamicDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    dynamicDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    dynamicDesc.MiscFlags = 0;
    dynamicDesc.StructureByteStride = 0;
    dynamicDesc.ByteWidth = sizeof(DirectX::XMFLOAT4X4) * lunarDraw.Entities->Count;
    COM_EXCEPT(device->CreateBuffer(&dynamicDesc, nullptr, &lunarDraw.DynamicBuffer));
    
    // Second context
    dynamicDesc.ByteWidth = sizeof(DirectX::XMFLOAT4X4) * earthDraw.Entities->Count;
    COM_EXCEPT(device->CreateBuffer(&dynamicDesc, nullptr, &earthDraw.DynamicBuffer));
}

void EntityRenderer::Update(DeviceResources* dr, const ConstantBufferBindPacket* camPacket, const ConstantBufferBindPacket* lightPacket, float dt)
{
    using namespace DirectX;

    static const float rotSpeed = 1.25f;
    static const XMVECTOR rot1 = DirectX::XMQuaternionRotationRollPitchYaw(rotSpeed, -rotSpeed, rotSpeed);
    static const XMVECTOR rot2 = -rot1;

    for (UINT i = 0; i != EntityGroups[0].Count; ++i)
    {
        Transform& transform = EntityGroups[0].Transforms[i];

        transform.Rotate(rot1 * dt);
        EntityGroups[0].WorldMatrices[i] = transform.Recompute();
    }

    for (UINT i = 0; i != EntityGroups[1].Count; ++i)
    {
        Transform& transform = EntityGroups[1].Transforms[i];

        transform.Rotate(rot2 * dt);
        EntityGroups[1].WorldMatrices[i] = transform.Recompute();
    }

    static DeferredJob::DeferredFunc drawWrapper = [](DeferredJob::DeferredArgs* args, DeviceResources* dr, ID3D11DeviceContext* def_context, EntityRenderer* entityRenderer, ID3D11CommandList** out_cmdList)
    {
        dr->SetRTV(def_context);
        def_context->RSSetViewports(1, &dr->GetScreenViewport());
        ConstantBufferUpdateManager::Bind(args->CameraPacket, def_context);
        ConstantBufferUpdateManager::Bind(args->LightPacket, def_context);
        entityRenderer->InstancedDraw(args->pass, def_context, out_cmdList);
    };

    for (UINT i = 0; i != InstancingPassCount; ++i)
    {
        DeferredJob::DeferredArgs args;
        args.pass = &InstancingPasses[i];
        args.CameraPacket = camPacket;
        args.LightPacket = lightPacket;

        EntityBatchRenderer.SubmitJob(drawWrapper, args);
    }
}

void EntityRenderer::Draw(DeviceResources* dr)
{
    EntityBatchRenderer.Render(dr, this);
    //this->InstancedDraw(&InstancingPasses[1], context);
}

void EntityRenderer::InstancedDraw(InstancedDrawContext* pDrawContext, ID3D11DeviceContext* context, ID3D11CommandList** out_cmdList)
{
    D3D11_MAPPED_SUBRESOURCE mappedBuffer;

    // Dereference the ptr up front
    const InstancedDrawContext pass = *pDrawContext;
    
    // Rewrite the dynamic vertex buffer with new matrix data
    COM_EXCEPT(context->Map(pass.DynamicBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer));
    memcpy(mappedBuffer.pData, pass.Entities->WorldMatrices, sizeof(DirectX::XMFLOAT4X4) * pass.Entities->Count);
    context->Unmap(pass.DynamicBuffer, 0);

    const Mesh mesh = *pass.InstancedMesh;

    ID3D11Buffer* vertexBuffers[2];
    vertexBuffers[0] = mesh.VertexBuffer;     // Vertices
    vertexBuffers[1] = pass.DynamicBuffer;    // Instanced World Matrices

    static const UINT strides[2] = 
    {
        mesh.Stride,
        sizeof(DirectX::XMFLOAT4X4)
    };

    static const UINT offsets[2] = 
    {
        0, 
        0
    };

    // Enqueue a bunch of commands into the deferred context
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetVertexBuffers(0, 2, &vertexBuffers[0], &strides[0], &offsets[0]);
    context->IASetIndexBuffer(mesh.IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Setup VS,PS
    const Material material = *pass.EntityMaterial;
    const VertexShader* VS = material.VS;
    const PixelShader*  PS = material.PS;

    context->IASetInputLayout(VS->InputLayout);
    context->VSSetShader(VS->Shader, nullptr, 0);
    context->PSSetShader(PS->Shader, nullptr, 0);
    context->PSSetSamplers(0, 1, &PS->SamplerState);

    // Update Material Param Data:
    ConstantBufferUpdateManager::Bind(&MaterialParamsCB, context);
    ConstantBufferUpdateManager::MapUnmap(&MaterialParamsCB, (void*)&material.Description, context);

    // Bind Textures expected by the shader
    context->PSSetShaderResources(0, (UINT)TextureSlots::COUNT, material.Resources->SRVs);

    // Draw call
    context->DrawIndexedInstanced(mesh.IndexCount, pass.Entities->Count, 0, 0, 0);

    // Populate command list
    context->FinishCommandList(FALSE, out_cmdList);
}

void EntityRenderer::Shutdown()
{
    for (UINT i = 0; i != InstancingPassCount; ++i)
    {
        InstancingPasses[i].DynamicBuffer->Release();
    }
    free(InstancingPasses);

    for (UINT i = 0; i != ENTITY_GROUP_COUNT; ++i)
    {
        free(EntityGroups[i].WorldMatrices);
        free(EntityGroups[i].Transforms);
    }

    ConstantBufferUpdateManager::Shutdown(&MaterialParamsCB);
    ConstantBufferUpdateManager::Shutdown(&EntityCB);

    EntityBatchRenderer.Shutdown();
}

}