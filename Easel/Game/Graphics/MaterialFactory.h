/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Creates and holds materials/textures 
to be used with game objects 
----------------------------------------------*/
#ifndef MATERIALFACTORY_H
#define MATERIALFACTORY_H

#include "DXCore.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace Graphics
{
class Material;
class ShaderFactory;
class Texture;
}

namespace Graphics {

class MaterialFactory final
{
public:
    MaterialFactory() = default;
    ~MaterialFactory();

    void Init(ID3D11Device* device, ID3D11DeviceContext* context);

    const Material* GetMaterial(std::wstring UID) const;

private:
    // Builds materials out of the textures gotten from LoadTextures
    void BuildMaterials();

    // Loads in every file in the designated textures folder and loads its SRV into the proper hash table
    void LoadTextures(ID3D11Device* device, ID3D11DeviceContext* context);

    // Data structures to hold textures 
    //      This is a good candidate for a reorganization based on a markup language of some kind to sort through textures. 
    //      Perhaps changing this to be a hash table of a hash tables, fi
    std::unordered_map<std::wstring, std::vector<Texture*>> mTextures;
    
    // Hold created materials
    std::unordered_map<std::wstring, const Material*>  mMaterials;

    // Unique instance of a ShaderFactory
    ShaderFactory* mpShaderFactory;

public: // Enforce default constructor
    MaterialFactory(MaterialFactory const&) = delete;
    MaterialFactory& operator=(MaterialFactory const&) = delete;
};

}

#endif