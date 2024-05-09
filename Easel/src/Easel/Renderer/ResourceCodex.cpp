/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/10
Description : Master Resource Distributor
----------------------------------------------*/
#include "ResourceCodex.h"

#include <Easel/Core/PathMacros.h>

#include "Factories.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"

#include "hash_util.h"

namespace Renderer {


MeshID ResourceCodex::AddMeshFromFile(const char* fileName, const VertexBufferDescription* vertAttr, ID3D11Device* pDevice)
{
    ResourceCodex& codexInstance = GetSingleton();

    Mesh mesh;
    MeshID id = MeshFactory::CreateMesh(fileName, vertAttr, pDevice, &mesh);
    auto& hashtable = codexInstance.mMeshMap;
    
    if (hashtable.find(id) == hashtable.end())
    {
        const Mesh c_Mesh = mesh;
        codexInstance.mMeshMap.insert(std::pair<MeshID, const Mesh>(id, c_Mesh));
    }
    else
    {
        #if defined(ESL_DEBUG)
            OutputDebugStringA("ERROR: Tried to insert repeat mesh\n");
        #endif
        assert(false);
    }
    return id;
}

void ResourceCodex::Init(ID3D11Device* device, ID3D11DeviceContext* context)
{
    ResourceCodex& codexInstance = GetSingleton();
    
    TextureFactory::LoadAllTextures(device, context, codexInstance);
    ShaderFactory::LoadAllShaders(device, codexInstance);
    MaterialFactory::CreateAllMaterials(device, codexInstance);
}

void ResourceCodex::Destroy()
{
    ResourceCodex& codexInstance = GetSingleton();

    for (auto const& m : codexInstance.mMeshMap)
    {
        m.second.VertexBuffer->Release();
        m.second.IndexBuffer->Release();
    }

    for (auto const& m : codexInstance.mMaterials)
    {
        if (m.RasterStateOverride)
            m.RasterStateOverride->Release();

        if (m.DepthStencilStateOverride)
            m.DepthStencilStateOverride->Release();
    }

    for (auto const& s : codexInstance.mVertexShaders)
    {
        const VertexShader& vs = s.second;
        vs.InputLayout->Release();
        free(vs.VertexDesc.SemanticsArr);
        free(vs.VertexDesc.ByteOffsets);
        vs.Shader->Release();
    }

    std::unordered_map<ShaderID, const PixelShader>::iterator it = codexInstance.mPixelShaders.begin();

    while (it != codexInstance.mPixelShaders.end())
    {
        if(it->second.SamplerState) it->second.SamplerState->Release();
        if(it->second.Shader) it->second.Shader->Release();
        ++it;
    }

    for (auto const& t : codexInstance.mTextureMap)
        for(ID3D11ShaderResourceView* srv : t.second.SRVs)
            if(srv) srv->Release();
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
    if (materialIndex > mMaterials.size())
        return nullptr;

    return &mMaterials.at(materialIndex);
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
    mVertexShaders.insert(std::pair<ShaderID, const VertexShader>(hash, cShader));
}

void ResourceCodex::AddPixelShader(ShaderID hash, const wchar_t* path, ID3D11Device* pDevice)
{   
    PixelShader shader;
    ShaderFactory::CreatePixelShader(path, &shader, pDevice);
    const PixelShader cShader = shader;
    mPixelShaders.insert(std::pair<ShaderID, const PixelShader>(hash, cShader));
}

void ResourceCodex::InsertTexture(TextureID UID, UINT slot, ID3D11ShaderResourceView* pSRV)
{
    auto itFind = mTextureMap.find(UID);
    if (itFind != mTextureMap.end())
    {
        ResourceBindChord& chord = itFind->second;
        if(chord.SRVs[slot])
            chord.SRVs[slot]->Release();

        mTextureMap[UID].SRVs[slot] = pSRV;
    }
    else
    {
        // TODO: This could probably avoid creating a temp variable
        ResourceBindChord rbc = {0};
        rbc.SRVs[slot] = pSRV;
        mTextureMap.insert(std::pair<TextureID, ResourceBindChord>(UID, rbc));
    }
}

MaterialIndex ResourceCodex::PushMaterial(const Material& material)
{
    mMaterials.push_back(material);
    return (MaterialIndex)(mMaterials.size() - 1);
}

}