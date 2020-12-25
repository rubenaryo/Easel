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

    #define NUM_ENTITIES 75

    // Initialize meshes, materials, entities
    InitMeshes(dr);

    for (unsigned i = 0; i != ENTITY_GROUP_COUNT; ++i)
    {
        std::uniform_real_distribution<float> dist(-10.f * (i + 1), 10.f * (i + 1));
        InitEntityGroup(&EntityGroups[i], NUM_ENTITIES / 4, dist);
    }

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
    const MeshID cubeID = ResourceCodex::AddMeshFromFile("cube.obj", phongVertDesc, device); //0x4a986f37
    const MeshID cylinderID = ResourceCodex::AddMeshFromFile("cylinder.obj", phongVertDesc, device); //0xdfc46858
    const MeshID torusID = ResourceCodex::AddMeshFromFile("torus.obj", phongVertDesc, device); //0x31bf4b69
}

void EntityRenderer::InitEntityGroup(EntityGroup* grp, const UINT numEntities, std::uniform_real_distribution<float> dist)
{
    std::default_random_engine gen;
    gen.seed(gen.default_seed);
    
    grp->WorldMatrices  = (DirectX::XMFLOAT4X4*)malloc(sizeof(DirectX::XMFLOAT4X4) * numEntities);
    grp->Transforms     = (Transform*)malloc(sizeof(Transform) * numEntities);
    grp->Count          = numEntities;

    for(UINT i = 0; i != numEntities; ++i)
    { 
        float d = dist(gen);
        float r = dist(gen);
        float r2 = dist(gen);
        float r3 = dist(gen);

        Transform& transform = grp->Transforms[i];
        transform.ResetFields();
        transform.SetTranslation(r, r2, r3);
        transform.SetRotation(r, r2, r3);
    }
}

void EntityRenderer::InitDrawContexts(ID3D11Device* device)
{
    const UINT kInstancingPassCount = ENTITY_GROUP_COUNT;
    const MeshID kSphereID   = 0x4fd8281f;
    const MeshID kCubeID     = 0x4a986f37;
    const MeshID kCylinderID = 0xdfc46858;
    const MeshID kTorusID    = 0x31bf4b69;
    
    ResourceCodex const& sg_Codex = ResourceCodex::GetSingleton();

    InstancingPassCount = kInstancingPassCount;
    InstancingPasses = (InstancedDrawContext*)malloc(sizeof(InstancedDrawContext) * kInstancingPassCount);

    InstancedDrawContext& sphereGrp = InstancingPasses[0];
    sphereGrp.Entities       = &EntityGroups[0];
    sphereGrp.InstancedMesh  = sg_Codex.GetMesh(kSphereID);
    sphereGrp.EntityMaterial = sg_Codex.GetMaterial(0);
    
    InstancedDrawContext& cubeGrp = InstancingPasses[1];
    cubeGrp.Entities       = &EntityGroups[1];
    cubeGrp.InstancedMesh  = sg_Codex.GetMesh(kCubeID);
    cubeGrp.EntityMaterial = sg_Codex.GetMaterial(1);

    InstancedDrawContext& cylinderGrp = InstancingPasses[2];
    cylinderGrp.Entities       = &EntityGroups[2];
    cylinderGrp.InstancedMesh  = sg_Codex.GetMesh(kCylinderID);
    cylinderGrp.EntityMaterial = sg_Codex.GetMaterial(1);

    InstancedDrawContext& torusGrp = InstancingPasses[3];
    torusGrp.Entities       = &EntityGroups[3];
    torusGrp.InstancedMesh  = sg_Codex.GetMesh(kTorusID);
    torusGrp.EntityMaterial = sg_Codex.GetMaterial(0);

    // Create the dynamic vertex buffer
    D3D11_BUFFER_DESC dynamicDesc = {0};
    dynamicDesc.Usage = D3D11_USAGE_DYNAMIC;
    dynamicDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    dynamicDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    dynamicDesc.MiscFlags = 0;
    dynamicDesc.StructureByteStride = 0;
    dynamicDesc.ByteWidth = sizeof(DirectX::XMFLOAT4X4) * sphereGrp.Entities->Count;
    //COM_EXCEPT(device->CreateBuffer(&dynamicDesc, nullptr, &sphereGrp.DynamicBuffer));
    HRESULT hr = device->CreateBuffer(&dynamicDesc, nullptr, &sphereGrp.DynamicBuffer);
    assert(!FAILED(hr));

    // Build the other instancing groups
    dynamicDesc.ByteWidth = sizeof(DirectX::XMFLOAT4X4) * cubeGrp.Entities->Count;
    //COM_EXCEPT(device->CreateBuffer(&dynamicDesc, nullptr, &cubeGrp.DynamicBuffer));
    hr = device->CreateBuffer(&dynamicDesc, nullptr, &cubeGrp.DynamicBuffer);
    assert(!FAILED(hr));

    dynamicDesc.ByteWidth = sizeof(DirectX::XMFLOAT4X4) * cylinderGrp.Entities->Count;
    //COM_EXCEPT(device->CreateBuffer(&dynamicDesc, nullptr, &cylinderGrp.DynamicBuffer));
    hr =device->CreateBuffer(&dynamicDesc, nullptr, &cylinderGrp.DynamicBuffer);
    assert(!FAILED(hr));

    dynamicDesc.ByteWidth = sizeof(DirectX::XMFLOAT4X4) * torusGrp.Entities->Count;
    //COM_EXCEPT(device->CreateBuffer(&dynamicDesc, nullptr, &torusGrp.DynamicBuffer));
    hr = device->CreateBuffer(&dynamicDesc, nullptr, &torusGrp.DynamicBuffer);
    assert(!FAILED(hr));
}

void EntityRenderer::Update(DeviceResources* dr, const ConstantBufferBindPacket* camPacket, const ConstantBufferBindPacket* lightPacket, float dt)
{
    using namespace DirectX;

    static const float rotSpeed = 1.25f;
    static const XMVECTOR rot1 = DirectX::XMQuaternionRotationRollPitchYaw(rotSpeed, -rotSpeed, rotSpeed);
    static const XMVECTOR rot2 = -rot1;

    static std::thread updaters[ENTITY_GROUP_COUNT];

    for (UINT i = 0; i != ENTITY_GROUP_COUNT; ++i)
    {
        
        static auto updateFunc = [](EntityGroup* grp, XMVECTOR quat, float dt)
        {
            for (UINT j = 0; j != grp->Count; ++j)
            {
                Transform& transform = grp->Transforms[j];

                transform.Rotate(quat * dt);
                grp->WorldMatrices[j] = transform.Recompute();
            }
        };

        updaters[i] = std::thread(updateFunc, &EntityGroups[i], rot1, dt);
    }

    for (UINT i = 0; i != ENTITY_GROUP_COUNT; ++i)
        updaters[i].join();

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
    //EntityBatchRenderer.Render(dr, this);
    //this->InstancedDraw(&InstancingPasses[1], context);
}

void EntityRenderer::InstancedDraw(InstancedDrawContext* pDrawContext, ID3D11DeviceContext* context, ID3D11CommandList** out_cmdList)
{
    D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};

    // Dereference the ptr up front
    const InstancedDrawContext pass = *pDrawContext;
    
    // Rewrite the dynamic vertex buffer with new matrix data
    //COM_EXCEPT(context->Map(pass.DynamicBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer));
    HRESULT hr = context->Map(pass.DynamicBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
    assert(!FAILED(hr));
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