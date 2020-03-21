/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Creates and holds materials/textures 
to be used with game objects 
----------------------------------------------*/
#ifndef MATERIALFACTORY_H
#define MATERIALFACTORY_H

#include "DXCore.h"
#include "Material.h"
#include "ShaderFactory.h"
#include <string>
#include <unordered_map>
#include <memory>

namespace Graphics {

class MaterialFactory final
{
public:
    MaterialFactory() = default;
    ~MaterialFactory();

    void Init(ID3D11Device* device, ID3D11DeviceContext* context);

    Material* GetMaterial(std::wstring a_UID) const;

private:
    // Builds materials out of the textures gotten from LoadTextures
    void BuildMaterials();

    // Loads in every file in the designated textures folder and loads its SRV into the proper hash table
    void LoadTextures(ID3D11Device* device, ID3D11DeviceContext* context);

    // Data structures to hold textures
    std::unordered_map<std::wstring, ID3D11ShaderResourceView*> m_DiffuseTextures;
    std::unordered_map<std::wstring, ID3D11ShaderResourceView*> m_NormalMaps;
    //TODO: spec,roughness, light, etc maps

    // Hold created materials
    std::unordered_map<std::wstring, Material*>  m_Materials;

    // Unique instance of a ShaderFactory
    std::unique_ptr<ShaderFactory> m_pShaderFactory;

public: // Enforce default constructor
    MaterialFactory(MaterialFactory const&) = delete;
    MaterialFactory& operator=(MaterialFactory const&) = delete;
};

}

#endif