/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/10
Description : Loads and distributes all static resources (materials, textures, etc)
----------------------------------------------*/
#ifndef RESOURCECODEX_H
#define RESOURCECODEX_H

#include "DXCore.h"

#include "Material.h"
#include "Mesh.h"
#include "Shader.h"

#include <EASTL/hash_map.h>

namespace Rendering {

struct MeshFactory;
struct ShaderFactory;
struct TextureFactory;
}

namespace Rendering {

typedef uint32_t id_type;
typedef id_type ShaderID;
typedef id_type MeshID;
typedef id_type TextureID;

class alignas(8) ResourceCodex
{
public:
    static MeshID AddMeshFromFile(const char* fileName, const VertexBufferDescription* vertAttr, ID3D11Device* pDevice);
    
    // Singleton Stuff
    static void Init(ID3D11Device* device, ID3D11DeviceContext* context);
    static void Shutdown();

    inline static ResourceCodex const& GetSingleton() { return *CodexInstance; }

    const Mesh* GetMesh(MeshID UID) const;
    const Material* GetMaterial(uint8_t materialIndex) const;
    const ResourceBindChord* GetTexture(TextureID UID) const;
    const VertexShader* GetVertexShader(ShaderID UID) const;
    const PixelShader* GetPixelShader(ShaderID UID) const;
        
    void BuildAllMaterials();

private:

    eastl::hash_map<ShaderID,  const VertexShader>  mVertexShaders;
    eastl::hash_map<ShaderID,  const PixelShader>   mPixelShaders;
    eastl::hash_map<MeshID,    const Mesh>          mMeshMap;
    eastl::hash_map<TextureID, ResourceBindChord>   mTextureMap;

    // Materials are queried by index rather than by ID since it's done at runtime 
    // TODO: Need to do this for meshes as well.
    // TODO: Use fixed_vector?
    Material* Materials;

    // Singleton stuff
    static ResourceCodex* CodexInstance;

    uint8_t MaterialCount;

private:
    friend struct TextureFactory;
    void InsertTexture(TextureID hash, UINT slot, ID3D11ShaderResourceView* pSRV);

    friend struct ShaderFactory;
    void AddVertexShader(ShaderID hash, const wchar_t* path, ID3D11Device* pDevice);
    void AddPixelShader(ShaderID hash, const wchar_t* path, ID3D11Device* pDevice);
};
}
#endif