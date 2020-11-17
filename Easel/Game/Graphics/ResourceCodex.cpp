#include "ResourceCodex.h"

#include "Factories.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"

#include "hash_util.h"
#include "../../System/PathMacros.h"

namespace Rendering {

ResourceCodex* ResourceCodex::CodexInstance = nullptr;

MeshID ResourceCodex::AddMeshFromFile(const char* fileName, const VertexBufferDescription* vertAttr, ID3D11Device* pDevice)
{
    Mesh mesh;
    MeshID id = MeshFactory::CreateMesh(fileName, vertAttr, pDevice, &mesh);
    auto& hashtable = CodexInstance->mMeshMap;
    
    if (hashtable.find(id) == hashtable.end())
    {
        const Mesh c_Mesh = mesh;
        CodexInstance->mMeshMap.insert(eastl::pair<MeshID, const Mesh>(id, c_Mesh));
    }
    else
    {
        #if defined(DEBUG)
            OutputDebugStringA("ERROR: Tried to insert repeat mesh\n");
        #endif
        assert(false);
    }
    return id;
}

void ResourceCodex::Init(ID3D11Device* device, ID3D11DeviceContext* context)
{
    CodexInstance = new ResourceCodex();

    TextureFactory::LoadAllTextures(device, context, CodexInstance);
    ShaderFactory::LoadAllShaders(device, CodexInstance);
    CodexInstance->BuildAllMaterials();
}

void ResourceCodex::Destroy()
{
    for (auto const& m : CodexInstance->mMeshMap)
    {
        m.second.VertexBuffer->Release();
        m.second.IndexBuffer->Release();
    }

    for (auto const& s : CodexInstance->mVertexShaders)
    {
        const VertexShader& vs = s.second;
        vs.InputLayout->Release();
        free(vs.VertexDesc.SemanticsArr);
        free(vs.VertexDesc.ByteOffsets);
        vs.Shader->Release();
    }

    eastl::hash_map<ShaderID, const PixelShader>::iterator it = CodexInstance->mPixelShaders.begin();

    while (it != CodexInstance->mPixelShaders.end())
    {
        if(it->second.SamplerState) it->second.SamplerState->Release();
        if(it->second.Shader) it->second.Shader->Release();
        ++it;
    }

    for (auto const& t : CodexInstance->mTextureMap)
        for(ID3D11ShaderResourceView* srv : t.second.SRVs)
            if(srv) srv->Release();

    free(CodexInstance->Materials);
    
    delete CodexInstance;
}

const Mesh* ResourceCodex::GetMesh(MeshID UID) const
{
    if(mMeshMap.find(UID) != mMeshMap.end())
        return &mMeshMap.at(UID);
    else
        return nullptr;
}

const Material* ResourceCodex::GetMaterial(uint8_t materialIndex) const
{
    return &Materials[materialIndex];
}

const ResourceBindChord* ResourceCodex::GetTexture(TextureID UID) const
{
    if(mTextureMap.find(UID) != mTextureMap.end())
        return &mTextureMap.at(UID);
    else
        return nullptr;
}

const VertexShader* ResourceCodex::GetVertexShader(ShaderID UID) const
{
    if(mVertexShaders.find(UID) != mVertexShaders.end())
        return &mVertexShaders.at(UID);
    else
        return nullptr;
}

const PixelShader* ResourceCodex::GetPixelShader(ShaderID UID) const
{
    if(mPixelShaders.find(UID) != mPixelShaders.end())
        return &mPixelShaders.at(UID);
    else
        return nullptr;
}

void ResourceCodex::AddVertexShader(ShaderID hash, const wchar_t* path, ID3D11Device* pDevice)
{
    VertexShader shader;
    ShaderFactory::CreateVertexShader(path, &shader, pDevice);
    const VertexShader cShader = shader;
    CodexInstance->mVertexShaders.insert(eastl::pair<ShaderID, const VertexShader>(hash, cShader));
}

void ResourceCodex::AddPixelShader(ShaderID hash, const wchar_t* path, ID3D11Device* pDevice)
{   
    PixelShader shader;
    ShaderFactory::CreatePixelShader(path, &shader, pDevice);
    const PixelShader cShader = shader;
    CodexInstance->mPixelShaders.insert(eastl::pair<ShaderID, const PixelShader>(hash, cShader));
}

void ResourceCodex::InsertTexture(TextureID UID, UINT slot, ID3D11ShaderResourceView* pSRV)
{
    if (mTextureMap.find(UID) != mTextureMap.end())
    {
        if(mTextureMap[UID].SRVs[slot])
            mTextureMap[UID].SRVs[slot]->Release();

        mTextureMap[UID].SRVs[slot] = pSRV;
    }
    else
    {
        ResourceBindChord rbc = {0};
        rbc.SRVs[slot] = pSRV;
        mTextureMap.insert(eastl::pair<TextureID, ResourceBindChord>(UID, rbc));
    }
}

void ResourceCodex::BuildAllMaterials()
{
    // All this code can probably be put into a factory class.
    const uint32_t kLunarId = fnv1a(L"Lunar");       // FNV1A of L"Lunar"
    
    const ShaderID kInstancedPhongVSID = 0xc8a366aa; // FNV1A of L"InstancedPhongVS.cso"
    const ShaderID kPhongPSID = 0x4dc6e249;          // FNV1A of L"PhongPS.cso"
    const ShaderID kPhongPSNormalMapID = 0x928ff72d; // FNV1A of L"PhongPS_NormalMap.cso"

    // Malloc the array
    uint8_t kMaterialCount = 1;
    MaterialCount = kMaterialCount;
    Materials = (Material*)malloc(sizeof(Material) * kMaterialCount);

    Material lunarMaterial;
    lunarMaterial.VS = GetVertexShader(kInstancedPhongVSID);
    lunarMaterial.PS = GetPixelShader(kPhongPSNormalMapID);
    lunarMaterial.Description.colorTint = DirectX::XMFLOAT4(DirectX::Colors::White);
    lunarMaterial.Description.specularExp = 128.0f;
    lunarMaterial.Resources = GetTexture(kLunarId);
    
    Materials[0] = lunarMaterial;
}

}