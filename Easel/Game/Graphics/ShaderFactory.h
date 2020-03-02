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
#include <vector>

namespace Graphics {

class ShaderFactory final
{
public:
    ShaderFactory();
    ~ShaderFactory();

    void Init();

private:
    // Compiles the shader with that file name and adds it to internal collection
    void CompileShader(const char* a_pFileName);

private:
    // Holds compiled shaders
    std::vector<ID3D11VertexShader*> m_pVertexShaders;



};

}

#endif