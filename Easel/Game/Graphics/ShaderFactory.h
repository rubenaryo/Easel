/*----------------------------------------------
Ruben Young (rubenaryo@gmail.com)
Date : 2020/2
Description : Handles the compilation and 
distribution of shaders across the program.
----------------------------------------------*/
#ifndef SHADERFACTORY_H
#define SHADERFACTORY_H

#include "../../System/PathMacros.h"
#include "DXCore.h"
#include "Shader.h"
#include <vector>
#include <unordered_map>
#include <string>

namespace Graphics {

// Types of shaders
enum class ShaderType
{
    VERTEX,
    PIXEL
};

class ShaderFactory final
{
public:
    // Force use of the default constructor
    ShaderFactory() = default;
    ~ShaderFactory();

    void Init(ID3D11Device* device);

    VertexShader* GetVertexShader(std::wstring UniqueID);
    PixelShader*  GetPixelShader(std::wstring UniqueID);
private:
    // Creates shader with that file name and adds it to internal collection
    // The file name becomes the unique identifier for the unordered maps
    inline void AddShader(std::wstring a_pFileName, ShaderType st, ID3D11Device* pDevice);
    inline void AddVertexShader(std::wstring UniqueID, ID3D11Device* pDevice);
    inline void AddPixelShader(std::wstring UniqueID, ID3D11Device* pDevice);

private:
    // Dictionaries that map Unique ID -> Shader*
    std::unordered_map<std::wstring, VertexShader*>    m_pVertexShaders;
    std::unordered_map<std::wstring, PixelShader*>     m_pPixelShaders;

public: // Force use of default constructor
    ShaderFactory(ShaderFactory const&)             = delete;
    ShaderFactory& operator=(ShaderFactory const&)  = delete;

};

}

#endif