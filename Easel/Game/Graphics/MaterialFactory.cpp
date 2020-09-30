/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Creates and holds materials/textures 
to be used with game objects 
----------------------------------------------*/
#include "MaterialFactory.h"

#include "Material.h"
#include "ShaderFactory.h"
#include "Texture.h"

#include "ThrowMacros.h"

#include <exception>
#include <filesystem>
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#include <unordered_set>

#if defined(DEBUG)
#include <stdlib.h>
#endif

namespace Graphics {

void MaterialFactory::Init(ID3D11Device* device, ID3D11DeviceContext* context)
{
    mpShaderFactory = std::make_unique<ShaderFactory>();
    mpShaderFactory->Init(device);
    LoadTextures(device, context);
    BuildMaterials();
}

const Material* MaterialFactory::GetMaterial(std::wstring a_UID) const
{
    if(mMaterials.find(a_UID) != mMaterials.end())
        return mMaterials.at(a_UID);

    return nullptr;
}

void MaterialFactory::BuildMaterials()
{
    // Use the shader factory to build some materials
    // This step would likely be streamlined to read shaders, meshes, materials directly from the 
    // development pipeline if this were a AAA game, but for now I just use the ShaderFactory to load some 
    // shaders, then make some sample materials out of them
    VertexShader* phongVS = mpShaderFactory->GetVertexShader(L"PhongVS.cso");
    PixelShader*  phongPS = mpShaderFactory->GetPixelShader(L"PhongPS_NormalMap.cso");
    PixelShader*  phongPS_noNormal = mpShaderFactory->GetPixelShader(L"PhongPS.cso");

    // Skybox shaders
    VertexShader* skyVS = mpShaderFactory->GetVertexShader(L"SkyVS.cso");
    PixelShader*  skyPS = mpShaderFactory->GetPixelShader(L"SkyPS.cso");

    // Material with high specular exponent
    cbMaterialParams highSpec;
    highSpec.colorTint = DirectX::XMFLOAT4(1,1,1,1);
    highSpec.specularExp = 128.0f;

    // For every material name, create a new material that holds all associated resources
    for (const auto& entry : mTextures)
    {
        // Special case: Sky:
        if (entry.first == L"Sky")
        {
            mMaterials[entry.first] = new Material(skyVS, skyPS, nullptr, &entry.second[0], static_cast<uint32_t>(entry.second.size()));
            continue;
        }

        mMaterials[entry.first] = new Material(phongVS, phongPS, &highSpec, &entry.second[0], static_cast<uint32_t>(entry.second.size()));
    }
}

void MaterialFactory::LoadTextures(ID3D11Device* device, ID3D11DeviceContext* context)
{
    namespace fs = std::filesystem;
    std::string texturePath = TEXTUREPATH;

    #if defined(DEBUG)
    if(!fs::exists(texturePath))
        throw std::exception("Textures folder doesn't exist!");
    #endif
    
    // Iterate through folder and initialize materials
    for (const auto& entry : fs::directory_iterator(texturePath))
    {
        std::wstring path = entry.path().c_str();
        std::wstring name = entry.path().filename().c_str();

        ID3D11ShaderResourceView* pSRV;

        // Parse file name to decide how to create this resource
        size_t pos = name.find(L'_');
        const std::wstring TexName = name.substr(0, pos++);
        const std::wstring TexType = name.substr(pos, 1);
        
        // Parse file extension
        pos = name.find(L'.') + 1;
        const std::wstring TexExt  = name.substr(pos);
        
        HRESULT hr = E_FAIL;

        ID3D11Resource* dummy = nullptr;

        // Special Case: DDS Files (Cube maps with no mipmaps)
        if (TexExt == L"dds")
        {
            hr = DirectX::CreateDDSTextureFromFile(
                device,
                path.c_str(),
                &dummy,
                &pSRV);
        } 
        else // For most textures, use WIC with mipmaps
        {
            hr = DirectX::CreateWICTextureFromFile(
                device, context,    // Passing in the context auto generates mipmaps
                path.c_str(),
                &dummy,
                &pSRV);
            
        }
        // Clean up Texture2D
        dummy->Release();
        assert(!FAILED(hr));

        // Classify based on Letter following '_'
        Texture::Type type;
        switch (TexType[0]) // This is the character that precedes the underscore in the naming convention
        {
            case 'N': // This is a normal map
                type = Texture::Type::NORMAL;
                mTextures[TexName].push_back(new Texture(pSRV, type));
                break;
            case 'T': // This is a texture
                type = Texture::Type::DIFFUSE;
                mTextures[TexName].push_back(new Texture(pSRV, type));
                break;
            case 'R': // Roughness map
                type = Texture::Type::ROUGHNESS;
                mTextures[TexName].push_back(new Texture(pSRV, type));
                break;
            case 'C': // Cube map
                type = Texture::Type::CUBE;
                mTextures[TexName].push_back(new Texture(pSRV, type));
                break;
            default:

                #if defined(DEBUG)
                std::wstring debugMsg = L"INFO: Attempted to load a texture with an unrecognized type: ";
                debugMsg.append(name.c_str());
                OutputDebugStringW(debugMsg.append(L"\n").c_str());
                #endif

                pSRV->Release(); // The SRV is still created, so it must be released
                pSRV = nullptr;
                break;
        }

        #if defined(DEBUG)
        if (pSRV)
        {
            size_t byteSize;
            static char texDebugName[64];
            wcstombs_s(&byteSize, texDebugName, name.c_str(), name.size());
            hr = pSRV->SetPrivateData(WKPDID_D3DDebugObjectName, byteSize, texDebugName);
            COM_EXCEPT(hr);
        }
        #endif
    }
}

MaterialFactory::~MaterialFactory()
{
    // Release all allocated materials
    for (const auto& mat : mMaterials)
    {
        delete mat.second;
    }

    // Release all allocated textures
    for (const auto& entry : mTextures)
    {
        for (Texture* tex : entry.second)
        {
            delete tex;
        }
    }

    // Cleanup shader factory
    mpShaderFactory.reset();
}

}