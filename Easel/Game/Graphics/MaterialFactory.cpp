#include "MaterialFactory.h"
#include <filesystem>
#include <WICTextureLoader.h>
#include <exception>
#include <unordered_set>
#include "ThrowMacros.h"
#include "COMException.h"
#include "../../System/PathMacros.h"

namespace Graphics {

void MaterialFactory::Init(ID3D11Device* device, ID3D11DeviceContext* context)
{
    m_pShaderFactory = std::make_unique<ShaderFactory>();
    m_pShaderFactory->Init(device);
    LoadTextures(device, context);
    BuildMaterials();
}

Material* MaterialFactory::GetMaterial(std::wstring a_UID) const
{
    if(m_Materials.find(a_UID) != m_Materials.end())
        return m_Materials.at(a_UID);

    return nullptr;
}

void MaterialFactory::BuildMaterials()
{
    // Use the shader factory to build some materials
    // This step would likely be streamlined to read shaders, meshes, materials directly from the 
    // development pipeline if this were a AAA game, but for now I just use the ShaderFactory to load some 
    // shaders, then make some sample materials out of them
    VertexShader* phongVS = m_pShaderFactory->GetVertexShader(L"PhongVS.cso");
    PixelShader* phongPS = m_pShaderFactory->GetPixelShader(L"PhongPS.cso");

    // Material with high specular exponent
    cbMaterialParams highSpec;
    highSpec.m_ColorTint = DirectX::XMFLOAT4(1,1,1,1);
    highSpec.m_Specularity = 128.0f;

    for (const auto& entry : m_DiffuseTextures)
    {
        m_Materials[entry.first] = new Material(phongVS, phongPS, &highSpec, entry.second);

        // Does this material have any associated maps? load them
        
    }
}

void MaterialFactory::LoadTextures(ID3D11Device* device, ID3D11DeviceContext* context)
{
    namespace fs = std::filesystem;
    std::string texturePath = TEXTUREPATH;
    if(!fs::exists(texturePath))
        throw std::exception("Textures folder doesn't exist!");

    // Iterate through folder and initialize materials
    for (const auto& entry : fs::directory_iterator(texturePath))
    {
        std::wstring path = entry.path().c_str();
        std::wstring name = entry.path().filename().c_str();

        ID3D11ShaderResourceView* pSRV;

        // Note: These have to be released in the destructor.
        HRESULT hr = DirectX::CreateWICTextureFromFile(
            device, context,
            path.c_str(),
            nullptr,
            &pSRV
        );

        assert(!FAILED(hr));

        size_t pos = name.find(L'_');
        const std::wstring TexName = name.substr(0, pos);
        const std::wstring TexType = name.substr(pos+1, 1);

        // Classify based on Letter following '_'
        switch (TexType[0])
        {
            case 'N': // This is a normal map
                m_NormalMaps[TexName] = pSRV;
                break;
            case 'T': // This is a texture
                m_DiffuseTextures[TexName] = pSRV;
                break;
            default:
                OutputDebugStringW(L"INFO: Attempted to load a texture with an unrecognized type");
                pSRV->Release(); // The SRV is still created, so it must be released
        }
    }
}

MaterialFactory::~MaterialFactory()
{
    for (const auto mat : m_Materials)
    {
        delete mat.second;
    }

    // Release ShaderResourceViews
    for (const auto srv : m_DiffuseTextures)
    {
        srv.second->Release();
    }
    for (const auto srv : m_NormalMaps)
    {
        srv.second->Release();
    }

    // Cleanup shader factory
    m_pShaderFactory.reset();
}

}