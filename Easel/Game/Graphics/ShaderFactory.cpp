/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Implementation of ShaderFactory.h
----------------------------------------------*/
#include "ShaderFactory.h"
#include <stdexcept>
#include "../../System/PathMacros.h"

namespace Graphics {

void ShaderFactory::Init(ID3D11Device* device)
{
    // Add a couple shaders
    AddShader(L"VertexShader.cso", ShaderType::VERTEX, device);
    AddShader(L"PixelShader.cso", ShaderType::PIXEL, device);
}

VertexShader* ShaderFactory::GetVertexShader(std::wstring UniqueID)
{
    // Check if not found, else grab from collection
    if(m_pVertexShaders.find(UniqueID) == m_pVertexShaders.end()) // Not found in collection
        return nullptr;
    else
        return m_pVertexShaders[UniqueID];
}

PixelShader* ShaderFactory::GetPixelShader(std::wstring UniqueID)
{
    // Check if not found, else grab from collection
    if(m_pPixelShaders.find(UniqueID) == m_pPixelShaders.end()) // Not found in collection
        return nullptr;
    else
        return m_pPixelShaders[UniqueID];
}

inline void ShaderFactory::AddShader(std::wstring a_pFileName, ShaderType st, ID3D11Device* pDevice)
{
    // Call the corresponding utility methods to add/compile shaders
    switch (st)
    {
        case ShaderType::VERTEX:
            AddVertexShader(a_pFileName, pDevice);
            break;
        case ShaderType::PIXEL:
            AddPixelShader(a_pFileName, pDevice);
            break;
    }
}

inline void ShaderFactory::AddVertexShader(std::wstring UniqueID, ID3D11Device* device)
{
    if (m_pVertexShaders.find(UniqueID) == m_pVertexShaders.end()) // Safe to add
    {
        std::wstring p = System::GetShaderPathFromFile_W(UniqueID);
        m_pVertexShaders[UniqueID] = new VertexShader(p.c_str(), device);
    }      
    else // Unique ID already exists
        throw std::exception("Tried to add Vertex Shader, but Unique ID already exists!");
}

inline void ShaderFactory::AddPixelShader(std::wstring UniqueID, ID3D11Device* device)
{
    if (m_pPixelShaders.find(UniqueID) == m_pPixelShaders.end()) // Safe to add
    {
        std::wstring p = System::GetShaderPathFromFile_W(UniqueID);
        m_pPixelShaders[UniqueID] = new PixelShader(p.c_str(), device);
    }
        
    else // Unique ID already exists
        throw std::exception("Tried to add Pixel Shader, but Unique ID already exists!");
}

// Free all allocated shaders
ShaderFactory::~ShaderFactory()
{
    // Delete every vertex shader
    for (std::pair<std::wstring, VertexShader*> e : m_pVertexShaders)
    {
        delete e.second;
        e.second = nullptr;
    }

    // Delete every pixel shader
    for (std::pair<std::wstring, PixelShader*> e : m_pPixelShaders)
    {
        delete e.second;
        e.second = nullptr;
    }
}

}