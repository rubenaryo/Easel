/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/10
Description : Loads and distributes all static resources (materials, textures, etc)
----------------------------------------------*/
#ifndef RESOURCECODEX_H
#define RESOURCECODEX_H

#include "DXCore.h"
#include <EASTL/hash_map.h>

#include "Material.h"
#include "Mesh.h"
#include "Shader.h"

namespace Rendering
{
}

namespace Rendering {

typedef uint32_t id_type;
typedef id_type ShaderID;
typedef id_type MeshID;
typedef id_type TextureID;

class ResourceCodex
{
public:
    static MeshID AddMeshFromFile(const char* fileName, const VertexBufferDescription* vertAttr, ID3D11Device* pDevice);
    
    // Singleton Stuff
    static void Init(ID3D11Device* device, ID3D11DeviceContext* context);
    static void Destroy();
    inline static ResourceCodex* GetSingleton()
        { return CodexInstance; }

    static ShaderID AddVertexShader(const wchar_t* fileName, ID3D11Device* pDevice);
    static ShaderID AddPixelShader(const wchar_t* fileName, ID3D11Device* pDevice);

    inline const Mesh* GetMesh(MeshID UID)
    {
        if(mMeshMap.find(UID) != mMeshMap.end())
            return &mMeshMap[UID];
        else
            return nullptr;
    }
    
    inline Material* GetMaterial(uint8_t materialIndex)
    {
        assert(materialIndex < MaterialCount);
        return &Materials[materialIndex];
    }

    inline const ResourceBindChord* GetTexture(TextureID UID);

    inline const VertexShader* GetVertexShader(ShaderID UID);

    inline const PixelShader* GetPixelShader(ShaderID UID);
        
    void BuildAllMaterials();

private:

    eastl::hash_map<ShaderID,  const VertexShader>      mVertexShaders;
    eastl::hash_map<ShaderID,  const PixelShader>       mPixelShaders;
    eastl::hash_map<MeshID,    const Mesh>              mMeshMap;
    eastl::hash_map<TextureID, const ResourceBindChord> mTextureMap;

    // Materials are queried by index rather than by ID since it's done at runtime 
    // TODO: Need to do this for meshes as well.
    // TODO: Use fixed_vector?
    Material* Materials;
    uint8_t MaterialCount;

    // Singleton stuff
    static ResourceCodex* CodexInstance;

private:
    void BuildAllTextures(ID3D11Device* device, ID3D11DeviceContext* context);

    void BuildAllShaders(ID3D11Device* device);
    
};
}
#endif