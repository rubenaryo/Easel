/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Implementation of Renderer class
----------------------------------------------*/
#include "Renderer.h"

namespace Graphics {

Renderer::Renderer()
{
    m_pShaderFactory = std::make_unique<ShaderFactory>();
}

void Renderer::Init(ID3D11Device* device)
{
    // Create necessary shaders
    m_pShaderFactory->Init(device);

    // Initialize meshes and materials
    InitMeshes();
    InitMaterials();
}

void Renderer::InitMeshes()
{
    // Load some models from memory
}

void Renderer::InitMaterials()
{
    // Use the shader factory to build some materials
    VertexShader* vs1 = m_pShaderFactory->GetVertexShader(L"VertexShader.hlsl");
    PixelShader* ps1 = m_pShaderFactory->GetPixelShader(L"PixelShader.hlsl");
    //Material* material1 = new Material()
}

void Renderer::Update(ID3D11DeviceContext* context, float dt, Camera* camera)
{
    VertexShader* vs = m_pShaderFactory->GetVertexShader(L"VertexShader.hlsl");
    
    // Give b0 buffer updated camera matrices every frame
    cbPerFrame perFrame = {};
    perFrame.view       = camera->GetView();
    perFrame.projection = camera->GetProjection();
    bool b = vs->SetBufferData(context, 0u, sizeof(cbPerFrame), (void*) &perFrame);

    Draw(context);
}

void Renderer::Draw(ID3D11DeviceContext* context)
{
    using Game::Entity;
    for (std::pair<Material* const, std::vector<Entity*>> element : m_EntityMap)
    {
        // Bind material
        Material* material = element.first;
        material->Bind(context);

        for (Entity* entity : element.second)
        {
            // set entity world matrix in cbuffer b1
            DirectX::XMFLOAT4X4 world = entity->GetTransform()->GetWorldMatrix();
            cbPerEntity perEntityCB = {};
            perEntityCB.world = world;

            //material->GetVertexShader()->SetBufferData(context, 1u, sizeof(cbPerEntity), &perEntityCB);

            // draw entity
            // entity->Draw(context);
        }
    }
}

Renderer::~Renderer()
{
    using Game::Entity;

    // Cleanup entities/materials
    for (std::pair<Material* const, std::vector<Entity*>> element : m_EntityMap)
    {
        // Free every entity
        for (Entity* entity : element.second)
        {
            delete entity;
        }

        // Free the associated material
        delete element.first;
    }

    // Cleanup meshes
    for (std::pair<std::string, Game::Mesh*> element : m_Meshes)
    {
        delete element.second;
    }

    // Cleanup shader factory
    m_pShaderFactory.reset();
}
}