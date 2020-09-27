/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Implementation of Renderer class
----------------------------------------------*/
#include "Renderer.h"

#include "Camera.h"
#include "CBufferStructs.h"
#include "COMException.h"
#include "DeviceResources.h"
#include "Material.h"
#include "MaterialFactory.h"
#include "ShaderFactory.h"
#include "SkyRenderer.h"

#include "../Entity.h"
#include "../Mesh.h"
#include "../Transform.h"


namespace Graphics {

Renderer::Renderer()
{
    mpMaterialFactory = std::make_unique<MaterialFactory>();
}

void Renderer::Init(DeviceResources* dr)
{
    // Grab reference to ID3D11Device
    auto device = dr->GetDevice();

    // Create a generic sampler state
    D3D11_SAMPLER_DESC samplerDesc;
    SecureZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.Filter   = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.MaxLOD   = D3D11_FLOAT32_MAX;

    HRESULT hr = device->CreateSamplerState(&samplerDesc, &mpSamplerState);
    if (FAILED(hr)) throw COM_EXCEPT(hr);

    // TODO: In the future, sample textures differently! See note in Renderer.h
    dr->GetContext()->PSSetSamplers(0, 1, &mpSamplerState);

    // Initialize meshes, materials, entities
    InitMeshes(dr);
    mpMaterialFactory->Init(device, dr->GetContext());
    mpSkyRenderer = new SkyRenderer(mMeshes["cube"], mpMaterialFactory->GetMaterial(L"Sky"), device);
    InitEntities();
    
    // For now, assume we're only using trianglelist
    dr->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Renderer::InitMeshes(DeviceResources* dr)
{
    using Game::Mesh;
    auto device = dr->GetDevice();

    // Load some models from memory
    mMeshes["teapot"] = new Mesh("teapot.obj", device);
    mMeshes["cube"]   = new Mesh("cube.obj"  , device);
    mMeshes["sphere"] = new Mesh("sphere.obj", device);
    mMeshes["torus"]  = new Mesh("torus.obj" , device);
}

void Renderer::InitEntities()
{
    using Game::Entity;
    using Game::Transform;

    // Grab material pointer from factory instance
    const Material* mat1 = mpMaterialFactory->GetMaterial(L"Lunar");
    const Material* mat2 = mpMaterialFactory->GetMaterial(L"Earth");
    assert(mat1);
    assert(mat2);

    // Teapot and sphere using material1
    Transform teapotTransform;
    teapotTransform.SetRotation(-DirectX::XM_PIDIV2, 0, 0);
    teapotTransform.SetScale(0.1f, 0.1f, 0.1f);
    teapotTransform.SetPosition(0.0f, +0.2f, 0.0f);

    Entity* entity1 = new Entity(mMeshes["teapot"], teapotTransform);
    Entity* entity2 = new Entity(mMeshes["sphere"]);
    entity2->GetTransform()->Scale(1.5f, 1.5f, 1.5f);

    // Move the sphere downwards
    entity2->GetTransform()->SetPosition(0.0f, -1.0f, 0.0f);

    // Add them all to the hash table
    mEntityMap[mat1].push_back(entity1);
    mEntityMap[mat2].push_back(entity2);
}

void Renderer::Update(ID3D11DeviceContext* context, float dt, const Camera* camera, const cbLighting* lightData)
{
    // Hold camera, lighting data locally for rendering
    mCameraBuffer = camera->AsConstantBuffer();
    memcpy(&mLightingBuffer, lightData, sizeof(cbLighting));
}

void Renderer::Draw(ID3D11DeviceContext* context)
{
    using Game::Entity;
    for (std::pair<const Material* const, std::vector<Entity*>> element : mEntityMap)
    {
        // Set material data, then bind 
        // "Binding" a material means binding its internal VS/PS
        const Material* material = element.first;

        // Set data for lighting, camera buffers
        material->GetPixelShader()->SetBufferData(context, (UINT)ReservedRegisters::RR_PS_LIGHTS, sizeof(cbLighting), &mLightingBuffer);
        material->GetVertexShader()->SetBufferData(context, (UINT)ReservedRegisters::RR_VS_CAMERA, sizeof(cbCamera), &mCameraBuffer);

        // Bind() sets the contents of material params constant buffer automatically before PSSetConstantBuffers
        material->Bind(context);

        for (Entity* entity : element.second)
        {
            // Set Per Entity world matrix in the associated material's vertex shader.
            DirectX::XMFLOAT4X4 world = entity->GetTransform()->GetWorldMatrix();
            cbPerEntity perEntityCB = {};
            perEntityCB.world = world;

            // @todo: Should the material own the vertexshader? by extension, should the materialFactory own the shaderFactory?
            material->GetVertexShader()->SetBufferData(context, (UINT)ReservedRegisters::RR_VS_WORLDMATRIX, sizeof(cbPerEntity), &perEntityCB);
            // draw entity
            entity->Draw(context);
        }
    }

    mpSkyRenderer->GetMaterial()->GetVertexShader()->SetBufferData(context, (UINT)ReservedRegisters::RR_VS_CAMERA, sizeof(cbCamera), &mCameraBuffer);
    mpSkyRenderer->Render(context);
}

Renderer::~Renderer()
{
    using Game::Entity;

    // Cleanup entities
    for (std::pair<const Material* const, std::vector<Entity*>> element : mEntityMap)
    {
        // Free every entity
        for (Entity* entity : element.second)
        {
            delete entity;
        }
    }

    // Cleanup meshes
    for (std::pair<std::string, const Game::Mesh*> element : mMeshes)
    {
        delete element.second;
    }

    mpSamplerState->Release();
    
    mpMaterialFactory.reset();
    delete mpSkyRenderer;
}
}