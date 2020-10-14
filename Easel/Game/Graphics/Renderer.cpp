/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Implementation of Renderer class
----------------------------------------------*/
#include "Renderer.h"

#include "Camera.h"
#include "CBufferStructs.h"
#include "DeviceResources.h"
#include "Material.h"
#include "MaterialFactory.h"
#include "Shader.h"
#include "ShaderFactory.h"
#include "SkyRenderer.h"
#include "ThrowMacros.h"

#include "../Entity.h"
#include "../Mesh.h"
#include "../Transform.h"

#include <random>
#include <time.h>

namespace Graphics {

Renderer::Renderer()
{
    mpMaterialFactory = new MaterialFactory();
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
    COM_EXCEPT(hr);

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
    mMeshes["cylinder"] = new Mesh("cylinder.obj" , device);
}

void Renderer::InitEntities()
{
    using Game::Entity;

    // Grab material pointer from factory instance
    const Material* mat1 = mpMaterialFactory->GetMaterial(L"Lunar");
    const Material* mat2 = mpMaterialFactory->GetMaterial(L"Earth");
    assert(mat1);
    assert(mat2);

    #define NUMENTITIES 75000
    Game::Transform xform;
    xform.Scale(0.5, 0.5, 0.5);
    
    // Entities are split about evenly per material, but not exactly
    mEntityMap[mat1].reserve(NUMENTITIES/2 + 50);
    mEntityMap[mat2].reserve(NUMENTITIES/2 + 50);
    
    std::default_random_engine gen;
    std::uniform_real_distribution dist(-40.f, 40.f);
    gen.seed(gen.default_seed);

    // Profiling test
    for (int i = 0; i < NUMENTITIES; ++i)
    {
        float d = dist(gen);
        float r = dist(gen);
        float r2 = dist(gen);
        float r3 = dist(gen);

        xform.SetTranslation(r, r2, r3);
        xform.SetRotation(r, r2, r3);

        // put entity into one of two buckets
        const Material* mat = d > 0 ? mat1 : mat2;
        const Game::Mesh* mesh;

        if (d < 0)
        {
            if (d > -5)
                mesh = mMeshes["sphere"];
            else
                mesh = mMeshes["cube"];
        }
        else
        {
            if (d > 5)
                mesh = mMeshes["torus"];
            else
                mesh = mMeshes["cylinder"];
        }

        mEntityMap[mat].push_back(Entity(mesh, xform));
    }
    #undef NUMENTITIES
}

void Renderer::Update(ID3D11DeviceContext* context, float dt, const Camera* camera, const cbLighting* lightData)
{
    using namespace DirectX;
    using Game::Transform;
    
    // Hold camera, lighting data locally for rendering
    mCameraBuffer = camera->AsConstantBuffer();
    memcpy(&mLightingBuffer, lightData, sizeof(cbLighting));

    const float rotSpeed = 1.25f;

    static const XMVECTOR rot1 = DirectX::XMQuaternionRotationRollPitchYaw(rotSpeed, -rotSpeed, rotSpeed);
    static const XMVECTOR rot2 = -rot1;

    std::vector<Game::Entity>* mat1List = &mEntityMap[mpMaterialFactory->GetMaterial(L"Lunar")];
    std::vector<Game::Entity>::iterator it = mat1List->begin();
    for (;it != mat1List->end(); ++it)
    {
        it->mTransform.Rotate(rot1*dt);
    }

    std::vector<Game::Entity>* mat2List = &mEntityMap[mpMaterialFactory->GetMaterial(L"Earth")];
    it = mat2List->begin();
    for (;it != mat2List->end(); ++it)
    {
        it->mTransform.Rotate(rot2*dt);
    }
}

void Renderer::Draw(ID3D11DeviceContext* context)
{
    using Game::Entity;
    using Game::Transform;

    // For every material<->entityList association
    for (std::pair<const Material* const, std::vector<Entity>>& element : mEntityMap)
    {
        // Set material data, then bind 
        // "Binding" a material means binding its internal VS/PS
        const Material* material = element.first;

        // Set data for lighting, camera buffers
        material->GetPixelShader()->SetBufferData(context, (UINT)ReservedRegisters::RR_PS_LIGHTS, sizeof(cbLighting), &mLightingBuffer);
        material->GetVertexShader()->SetBufferData(context, (UINT)ReservedRegisters::RR_VS_CAMERA, sizeof(cbCamera), &mCameraBuffer);

        material->SetMaterialParams(context);
        material->Bind(context);
        
        // For every entity using this material
        std::vector<Entity>::iterator it = element.second.begin();
        for(;it != element.second.end(); ++it)
        {
            // Set Per Entity world matrix in the associated material's vertex shader.
            DirectX::XMFLOAT4X4 world = it->mTransform.Recompute();
            material->GetVertexShader()->SetBufferData(context, (UINT)ReservedRegisters::RR_VS_WORLDMATRIX, sizeof(world), &world);
            it->Draw(context);
        }
    }

    // TODO: Should not have to rebind camera VP matrix twice. This will be fixed once constant buffers are overhauled.
    mpSkyRenderer->GetMaterial()->GetVertexShader()->SetBufferData(context, (UINT)ReservedRegisters::RR_VS_CAMERA, sizeof(cbCamera), &mCameraBuffer);
    mpSkyRenderer->Render(context); // Render binds the material internally
}

Renderer::~Renderer()
{
    // Cleanup meshes
    for (std::pair<std::string, const Game::Mesh*> element : mMeshes)
    {
        delete element.second;
    }

    mpSamplerState->Release();
    
    delete mpMaterialFactory;
    delete mpSkyRenderer;
}
}