/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Implementation of Renderer class
----------------------------------------------*/
#include "Renderer.h"
#include "COMException.h"

namespace Graphics {

Renderer::Renderer()
{
    m_pMaterialFactory = std::make_unique<MaterialFactory>();
}

void Renderer::Init(DeviceResources* a_DR)
{
    // Grab reference to ID3D11Device
    auto device = a_DR->GetDevice();

    // Create a general sampler state
    D3D11_SAMPLER_DESC samplerDesc;
    SecureZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.Filter   = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.MaxLOD   = D3D11_FLOAT32_MAX;

    HRESULT hr = device->CreateSamplerState(&samplerDesc, m_pSamplerState.GetAddressOf());
    if (FAILED(hr)) throw COM_EXCEPT(hr);

    // TODO: In the future, sample textures differently! See Renderer.h
    a_DR->GetContext()->PSSetSamplers(0, 1, m_pSamplerState.GetAddressOf());

    // Create reserved buffers
    m_pCameraBuffer     = new VSConstantBuffer(device, sizeof(cbCamera), c_ReservedBufferSlot, nullptr);
    m_pMaterialBuffer   = new PSConstantBuffer(device, sizeof(cbMaterialParams), c_ReservedBufferSlot, nullptr);

    // Initialize meshes, materials, entities
    InitMeshes(a_DR);
    m_pMaterialFactory->Init(device, a_DR->GetContext());
    InitEntities();
    
    // For now, assume we're only using trianglelist
    a_DR->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Renderer::InitMeshes(DeviceResources* a_DR)
{
    using Game::Mesh;
    auto device = a_DR->GetDevice();

    // Load some models from memory
    m_Meshes["teapot"] = new Mesh("teapot.obj", device);
    m_Meshes["cube"]   = new Mesh("cube.obj", device);
    m_Meshes["sphere"] = new Mesh("sphere.obj", device);
    m_Meshes["torus"]  = new Mesh("torus.obj" , device);
}

void Renderer::InitEntities()
{
    using Game::Entity;
    using Game::Transform;

    // Grab material pointer from factory instance
    Material* mat1 = m_pMaterialFactory->GetMaterial(L"Lunar");
    assert(mat1);

    // Teapot and sphere using material1
    Transform teapotTransform;
    teapotTransform.SetRotation(-DirectX::XM_PIDIV2, 0, 0);
    teapotTransform.SetScale(0.1f, 0.1f, 0.1f);
    teapotTransform.SetPosition(0.0f, +0.2f, 0.0f);

    //Entity* entity1 = new Entity(m_Meshes["teapot"], teapotTransform);
    Entity* entity2 = new Entity(m_Meshes["sphere"]);
    entity2->GetTransform()->Scale(2,2,2);

    // Move the sphere downwards
    //entity2->GetTransform()->SetPosition(0.0f, -1.0f, 0.0f);

    // Add them all to the hash table
    //m_EntityMap[mat1].push_back(entity1);
    m_EntityMap[mat1].push_back(entity2);
}

void Renderer::Update(ID3D11DeviceContext* context, float dt, Camera* camera)
{
    // Give camera buffer updated matrices
    cbCamera perFrame = {};
    perFrame.view       = camera->GetView();
    perFrame.projection = camera->GetProjection();
    m_pCameraBuffer->SetData(context, sizeof(cbCamera), (void*)&perFrame);

    // Bind camera buffer to the pipeline
    m_pCameraBuffer->Bind(context);
}

void Renderer::Draw(ID3D11DeviceContext* context)
{
    using Game::Entity;
    for (std::pair<Material* const, std::vector<Entity*>> element : m_EntityMap)
    {
        // Set material data, then bind 
        // "Binding" a material means binding its internal VS/PS
        Material* material = element.first;
        m_pMaterialBuffer->SetData(context, sizeof(cbMaterialParams), (void*) &(material->GetMaterialInfo()));
        material->Bind(context);
        m_pMaterialBuffer->Bind(context);

        for (Entity* entity : element.second)
        {
            // set entity world matrix data in the reserved buffer
            DirectX::XMFLOAT4X4 world = entity->GetTransform()->GetWorldMatrix();
            cbPerEntity perEntityCB = {};
            perEntityCB.world = world;

            material->GetVertexShader()->SetBufferData(context, 0u, sizeof(cbPerEntity), &perEntityCB);

            // draw entity
            entity->Draw(context);
        }
    }
}

Renderer::~Renderer()
{
    using Game::Entity;

    // Cleanup entities
    for (std::pair<Material* const, std::vector<Entity*>> element : m_EntityMap)
    {
        // Free every entity
        for (Entity* entity : element.second)
        {
            delete entity;
        }

    }

    // Cleanup meshes
    for (std::pair<std::string, Game::Mesh*> element : m_Meshes)
    {
        delete element.second;
    }

    // Cleanup Camera/Material buffers
    delete m_pCameraBuffer;
    delete m_pMaterialBuffer;

    // Cleanup material factory
    m_pMaterialFactory.reset();
}
}