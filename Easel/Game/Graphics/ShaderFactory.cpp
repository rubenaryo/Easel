/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/3
Description : Implementation of ShaderFactory.h
----------------------------------------------*/
#include "ShaderFactory.h"

#include "../../System/PathMacros.h"
#include "Shader.h"

#include <stdexcept>
#include <filesystem>


namespace Graphics {

void ShaderFactory::Init(ID3D11Device* device)
{
    // AAA Scenario: Would parse the contents of these shaders using reflections/some other system, 
    //  automatically adding the necessary cbuffers, fields, etc
    // Me: Hardcode it! One day I might want to make a system to auto read in shader data, but lets stay focused for now

    // Load in every shader binary automatically
    namespace fs = std::filesystem;
    std::string shaderPath = SHADERPATH;
    if(!fs::exists(shaderPath))
        throw std::exception("Shaders folder doesn't exist!");
    
    // Iterate through shader folder
    for (const auto& entry : fs::directory_iterator(shaderPath))
    {
        std::wstring name = entry.path().filename().c_str();

        // Check for valid shader type first
        if (name.find(L".cso") != std::wstring::npos)
        {
            if(name.find(L"VS") != std::wstring::npos) // Vertex Shader file
            {
                AddShader(name, ShaderType::VERTEX, device);
            }
            else if (name.find(L"PS") != std::wstring::npos) // Pixel Shader file
            {
                AddShader(name, ShaderType::PIXEL, device);
            }
            #if defined(DEBUG)
            else // Neither, throw warning
            {
                OutputDebugStringA("INFO: Attempted to load non-supported .cso file, doing nothing.\n");
            }
            #endif
        }
        #if defined(DEBUG)
        else // Not a .cso file, throw warning
        {
            std::wstring errorMsg = L"WARNING: Invalid File Format found in " + entry.path().generic_wstring() + L"\n";
            OutputDebugStringW(errorMsg.c_str());
        }
        #endif
    }
}

VertexShader* ShaderFactory::GetVertexShader(std::wstring UniqueID)
{
    // Check if not found, else grab from collection
    if(mpVertexShaders.find(UniqueID) == mpVertexShaders.end()) // Not found in collection
        return nullptr;
    else
        return mpVertexShaders[UniqueID];
}

PixelShader* ShaderFactory::GetPixelShader(std::wstring UniqueID)
{
    // Check if not found, else grab from collection
    if(mpPixelShaders.find(UniqueID) == mpPixelShaders.end()) // Not found in collection
        return nullptr;
    else
        return mpPixelShaders[UniqueID];
}

inline void ShaderFactory::AddShader(std::wstring pFileName, ShaderType st, ID3D11Device* pDevice)
{
    // Call the corresponding utility methods to add/compile shaders
    switch (st)
    {
        case ShaderType::VERTEX:
            AddVertexShader(pFileName, pDevice);
            break;
        case ShaderType::PIXEL:
            AddPixelShader(pFileName, pDevice);
            break;
    }
}

inline void ShaderFactory::AddVertexShader(std::wstring UniqueID, ID3D11Device* device)
{
    if (mpVertexShaders.find(UniqueID) == mpVertexShaders.end()) // Safe to add
    {
        // Create a vertex shader on the heap.
        mpVertexShaders[UniqueID] = new VertexShader(System::GetShaderPathFromFile_W(UniqueID).c_str(), device);
    }      
    #if defined(DEBUG)
    else // Unique ID already exists
        throw std::exception("Tried to add Vertex Shader, but Unique ID already exists!");
    #endif
}

inline void ShaderFactory::AddPixelShader(std::wstring UniqueID, ID3D11Device* device)
{
    if (mpPixelShaders.find(UniqueID) == mpPixelShaders.end()) // Safe to add
    {
        // Create pixel shader on the heap. 
        mpPixelShaders[UniqueID] = new PixelShader(System::GetShaderPathFromFile_W(UniqueID).c_str(), device);
    }
    #if defined(DEBUG)
    else // Unique ID already exists
        throw std::exception("Tried to add Pixel Shader, but Unique ID already exists!");
    #endif
}

// Free all allocated shaders
ShaderFactory::~ShaderFactory()
{
    // Delete every vertex shader
    for (std::pair<std::wstring, VertexShader*> e : mpVertexShaders)
    {
        delete e.second;
        e.second = nullptr;
    }

    // Delete every pixel shader
    for (std::pair<std::wstring, PixelShader*> e : mpPixelShaders)
    {
        delete e.second;
        e.second = nullptr;
    }
}

}