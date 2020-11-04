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

    CodexInstance->BuildAllTextures(device, context);

    assert(CodexInstance->mPixelShaders.empty());
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

    size_t size = CodexInstance->mPixelShaders.size();
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

    free(CodexInstance->mMaterials);
    
    delete CodexInstance;
}

ShaderID ResourceCodex::AddVertexShader(const wchar_t* fileName, ID3D11Device* pDevice)
{
    ShaderID shaderId = fnv1a(fileName);
    VertexShader shader;
    TShaderFactory::CreateVertexShader(System::GetShaderPathFromFile_W(fileName).c_str(), &shader, pDevice);

    const VertexShader cShader = shader;
    CodexInstance->mVertexShaders.insert(eastl::pair<ShaderID, const VertexShader>(shaderId, cShader));
    
    const VertexShader* test = CodexInstance->GetVertexShader(shaderId);

    return shaderId;
}

ShaderID ResourceCodex::AddPixelShader(const wchar_t* fileName, ID3D11Device* pDevice)
{
    ShaderID shaderId = fnv1a(fileName);
    PixelShader shader;

    TShaderFactory::CreatePixelShader(System::GetShaderPathFromFile_W(fileName).c_str(), &shader, pDevice);
    const PixelShader cShader = shader;
    CodexInstance->mPixelShaders.insert(eastl::pair<ShaderID, const PixelShader>(shaderId, cShader));
    assert(CodexInstance->mPixelShaders.size() < 3);
    return shaderId;
}

const ResourceBindChord* ResourceCodex::GetTexture(TextureID UID)
{
    if(mTextureMap.find(UID) != mTextureMap.end())
        return &mTextureMap[UID];
    else
        return nullptr;
}

const VertexShader* ResourceCodex::GetVertexShader(ShaderID UID)
{
    if(mVertexShaders.find(UID) != mVertexShaders.end())
        return &mVertexShaders[UID];
    else
        return nullptr;
}

const PixelShader* ResourceCodex::GetPixelShader(ShaderID UID)
{
    if(mPixelShaders.find(UID) != mPixelShaders.end())
        return &mPixelShaders[UID];
    else
        return nullptr;
}

void ResourceCodex::BuildAllTextures(ID3D11Device* device, ID3D11DeviceContext* context)
{
    TTextureFactory::LoadAllTextures(device, context, mTextureMap);
}

void ResourceCodex::BuildAllMaterials()
{
    // All this code can probably be put into a factory class.
    const uint32_t kLunarId = fnv1a(L"Lunar");       // FNV1A of L"Lunar"
    
    const ShaderID kInstancedPhongVSID = 0xc8a366aa; // FNV1A of L"InstancedPhongVS.cso"
    const ShaderID kPhongPSID = 0x4dc6e249;          // FNV1A of L"PhongPS.cso"

    // Malloc the array
    uint8_t kMaterialCount = 1;
    MaterialCount = kMaterialCount;
    mMaterials = (TMaterial*)malloc(sizeof(TMaterial) * kMaterialCount);

    TMaterial lunarMaterial;
    lunarMaterial.VS = GetVertexShader(kInstancedPhongVSID);
    lunarMaterial.PS = GetPixelShader(kPhongPSID);
    lunarMaterial.Description.colorTint = DirectX::XMFLOAT4(DirectX::Colors::White);
    lunarMaterial.Description.specularExp = 128.0f;
    lunarMaterial.Resources = GetTexture(kLunarId);
    
    mMaterials[0] = lunarMaterial;
}

}